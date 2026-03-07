// Steganos LockNote - self-modifying encrypted notepad
// Copyright (C) 2006-2023 Steganos GmbH
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <algorithm>
#include <array>
#include <map>
#include <vector>
#include <cwctype>
#include <atldlgs.h>
//#include <windows.h>
#include "utils.h"

inline constexpr const char* DEFAULT_FONT_NAME = NAME_FONT_CONSOLAS;
constexpr unsigned int DEFAULT_FONT_SIZE = 12;
constexpr unsigned int DEFAULT_WIDTH = 650;
constexpr unsigned int DEFAULT_HEIGHT = 500;

#ifndef chx1
#define chx1 0x0410
#define chx2 0x0411
#define rad1 0x0420
#define rad2 0x0421
#endif

#ifndef ECM_FIRST
#define ECM_FIRST 0x1500
#endif

#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER (ECM_FIRST + 1)
#endif

inline const UINT UWM_FINDMSGSTRING = CFindReplaceDialog::GetFindReplaceMsg();

enum class ThemeMode : int
{
	System = 0,
	Light = 1,
	Dark = 2
};

class CMainFrame 
	: public CFrameWindowImpl<CMainFrame>, public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	static DWORD GetWndStyle(DWORD dwStyle)
	{
		if (dwStyle == 0)
		{
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}

		dwStyle |= WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		dwStyle &= ~WS_CAPTION;
		return dwStyle;
	}

	static DWORD GetWndExStyle(DWORD dwExStyle)
	{
		return dwExStyle;
	}

	struct UndoBuffer
	{
		int m_nStartChar;
		int m_nEndChar;
		std::string m_strText;
	};

	std::list<UndoBuffer> m_listUndo;

	CLockNoteView m_view;
	CFont m_fontEdit;
	CFont m_fontUi;
	CFont m_fontToolbarIcons;
	CFont m_fontTitleBarIcons;
	CFont m_fontFindPanelText;
	CFont m_fontFindPanelIcons;
	std::string m_text;
	UndoBuffer m_currentBuffer;
	std::string m_password;

	DWORD m_dwSearchFlags = FR_DOWN;
	std::string m_strSearchString;
	CFindReplaceDialog* m_pCurrentFindReplaceDialog{ nullptr };

	int m_nWindowSizeX{ DEFAULT_WIDTH };
	int m_nWindowSizeY{ DEFAULT_HEIGHT };
	int m_nFontSize{ DEFAULT_FONT_SIZE };
	int m_nLanguage{ 0 };
	int m_nThemeMode{ static_cast<int>(ThemeMode::System) };
	std::map<WORD, std::wstring> m_languages{ {LANG_ENGLISH, L"en_US"}, { LANG_GERMAN, L"de_DE" }, { LANG_FRENCH, L"fr_FR" }, { LANG_SPANISH, L"es_ES" }, { LANG_PORTUGUESE, L"pt_PT" }, { LANG_ITALIAN, L"it_IT" }, { LANG_DUTCH, L"nl_NL" }, { LANG_SWEDISH, L"sv_SE" }, { LANG_POLISH, L"pl_PL" }, { LANG_RUSSIAN, L"ru_RU" } };

	std::string m_strFontName{ DEFAULT_FONT_NAME };
	AESLayer::KdfMode m_kdfMode{ AESLayer::KdfMode::Scrypt };
	bool m_bTraitsChanged{ false };
	bool m_ignoreEditNotifications{ false };
	bool m_isDarkThemeApplied{ false };
	bool m_isStatusBarVisible{ true };
	bool m_isWordWrapEnabled{ true };
	bool m_isFormattingEnabled{ false };
	static constexpr int kStatusBarParts = 6;
	static constexpr int kTopMenuOverflowMarker = -1;
	static constexpr UINT kTopMenuOverflowCommandBase = 0xE900;
	static constexpr UINT kMenuCmdFindPreviousPlaceholder = 0xE910;
	static constexpr UINT kMenuCmdReplacePlaceholder = 0xE911;
	static constexpr UINT kFindPanelToggleButtonId = 0xE920;
	static constexpr UINT kFindPanelFindEditId = 0xE921;
	static constexpr UINT kFindPanelFindNextButtonId = 0xE922;
	static constexpr UINT kFindPanelFindPrevButtonId = 0xE923;
	static constexpr UINT kFindPanelOptionsButtonId = 0xE924;
	static constexpr UINT kFindPanelCloseButtonId = 0xE925;
	static constexpr UINT kFindPanelReplaceEditId = 0xE926;
	static constexpr UINT kFindPanelReplaceButtonId = 0xE927;
	static constexpr UINT kFindPanelReplaceAllButtonId = 0xE928;
	static constexpr UINT kFindPanelOptionMatchCaseId = 0xE929;
	static constexpr UINT kFindPanelOptionWrapId = 0xE92A;
	static constexpr UINT_PTR kTopBarAnimationTimerId = 0xE92B;
	static constexpr UINT kTopBarAnimationIntervalMs = 16;
	static constexpr UINT kTopBarAnimationDurationMs = 144;
	static constexpr int kMinimumWindowTrackWidth = 680;
	static constexpr int kMinimumWindowTrackHeight = 440;
	static constexpr int kStoredWindowMinWidth = 320;
	static constexpr int kStoredWindowMinHeight = 240;
	static constexpr int kTitleBarButtonMinimize = 0;
	static constexpr int kTitleBarButtonMaximize = 1;
	static constexpr int kTitleBarButtonClose = 2;
	static constexpr int kTitleBarButtonCount = 3;
	COLORREF m_viewTextColor{ ::GetSysColor(COLOR_WINDOWTEXT) };
	COLORREF m_viewBackgroundColor{ ::GetSysColor(COLOR_WINDOW) };
	COLORREF m_frameBackgroundColor{ ::GetSysColor(COLOR_3DFACE) };
	COLORREF m_titleBarBackgroundColor{ RGB(24, 24, 24) };
	COLORREF m_topBarBackgroundColor{ RGB(32, 32, 32) };
	COLORREF m_topBarTextColor{ RGB(232, 232, 232) };
	COLORREF m_topBarHoverColor{ RGB(57, 57, 57) };
	COLORREF m_topBarPressedColor{ RGB(72, 72, 72) };
	COLORREF m_topBarDividerColor{ RGB(62, 62, 62) };
	COLORREF m_toolbarBackgroundColor{ RGB(32, 32, 32) };
	COLORREF m_toolbarHoverColor{ RGB(57, 57, 57) };
	COLORREF m_toolbarPressedColor{ RGB(72, 72, 72) };
	COLORREF m_statusBarBackgroundColor{ RGB(32, 32, 32) };
	COLORREF m_statusBarDividerColor{ RGB(62, 62, 62) };
	COLORREF m_statusBarTextColor{ RGB(222, 222, 222) };
	COLORREF m_findPanelBackgroundColor{ RGB(40, 40, 40) };
	COLORREF m_findPanelBorderColor{ RGB(84, 84, 84) };
	COLORREF m_findPanelButtonHoverColor{ RGB(60, 60, 60) };
	COLORREF m_findPanelButtonPressedColor{ RGB(72, 72, 72) };
	COLORREF m_findPanelButtonTextColor{ RGB(230, 230, 230) };
	COLORREF m_findPanelInputBackgroundColor{ RGB(30, 30, 30) };
	COLORREF m_findPanelInputBorderColor{ RGB(98, 98, 98) };
	CBrush m_viewBackgroundBrush;
	CBrush m_frameBackgroundBrush;
	CBrush m_statusBarBackgroundBrush;
	CBrush m_findPanelInputBrush;
	HMENU m_hMainMenu{ nullptr };
	WNDPROC m_findDialogOriginalProc{ nullptr };
	int m_nTopBarHeight{ 50 };
	int m_nTopTitleBarHeight{ 50 };
	int m_nTopMenuHeight{ 50 };
	int m_nToolbarHeight{ 0 };
	std::vector<int> m_topMenuPopupPositions;
	std::vector<RECT> m_topMenuButtonRects;
	std::vector<std::wstring> m_topMenuCaptions;
	std::vector<int> m_topMenuOverflowPopupPositions;
	std::vector<std::wstring> m_topMenuOverflowCaptions;
	std::vector<UINT> m_toolbarCommandIds;
	std::vector<RECT> m_toolbarButtonRects;
	std::vector<std::wstring> m_toolbarButtonCaptions;
	RECT m_topBarTitleBandRect{};
	RECT m_topBarIconRect{};
	RECT m_topBarTitleRect{};
	RECT m_topBarTabCloseRect{};
	RECT m_topBarAddButtonRect{};
	std::array<RECT, kTitleBarButtonCount> m_titleBarButtonRects{};
	std::wstring m_topBarTitleText;
	int m_titleBarHoveredButtonIndex{ -1 };
	int m_titleBarPressedButtonIndex{ -1 };
	int m_topMenuHoveredIndex{ -1 };
	int m_topMenuPressedIndex{ -1 };
	int m_toolbarHoveredIndex{ -1 };
	int m_toolbarPressedIndex{ -1 };
	BYTE m_topMenuHoverBlend{ 0 };
	BYTE m_titleBarHoverBlend{ 0 };
	bool m_topBarAnimationTimerActive{ false };
	bool m_hasPreparedInitialWindowSize{ false };
	bool m_hasLoadedPersistedWindowSize{ false };
	UINT m_lastSizeMessageType{ SIZE_RESTORED };
	std::wstring m_statusBarText;
	std::array<std::wstring, kStatusBarParts> m_statusBarPartTexts;
	HWND m_hFindPanelToggleButton{ nullptr };
	HWND m_hFindPanelFindEdit{ nullptr };
	HWND m_hFindPanelFindNextButton{ nullptr };
	HWND m_hFindPanelFindPrevButton{ nullptr };
	HWND m_hFindPanelOptionsButton{ nullptr };
	HWND m_hFindPanelCloseButton{ nullptr };
	HWND m_hFindPanelReplaceEdit{ nullptr };
	HWND m_hFindPanelReplaceButton{ nullptr };
	HWND m_hFindPanelReplaceAllButton{ nullptr };
	HWND m_hFindPanelSearchIcon{ nullptr };
	bool m_isFindPanelVisible{ false };
	bool m_isReplacePanelVisible{ false };
	bool m_isFindWrapEnabled{ true };
	HWND m_hHotFindPanelButton{ nullptr };
	RECT m_findPanelRect{};
	RECT m_findPanelFindEditFrameRect{};
	RECT m_findPanelReplaceEditFrameRect{};
	RECT m_findPanelSearchIconRect{};

	CMainFrame()
	{
	}

	virtual ~CMainFrame() = default;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg != nullptr && pMsg->message == WM_KEYDOWN)
		{
			const bool ctrlPressed = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
			const bool shiftPressed = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
			if (ctrlPressed && shiftPressed && pMsg->wParam == 'P')
			{
				ShowCommandPalette();
				return TRUE;
			}

			const HWND hFocus = ::GetFocus();
			if (hFocus == m_hFindPanelFindEdit && pMsg->wParam == VK_RETURN)
			{
				m_dwSearchFlags |= FR_DOWN;
				FindNext();
				return TRUE;
			}
			if (hFocus == m_hFindPanelReplaceEdit && pMsg->wParam == VK_RETURN)
			{
				ReplaceCurrentSelection();
				return TRUE;
			}
			if (m_isFindPanelVisible && pMsg->wParam == VK_ESCAPE)
			{
				SetFindPanelVisible(false);
				m_view.SetFocus();
				return TRUE;
			}
		}

		if (m_pCurrentFindReplaceDialog)
		{
			if (m_pCurrentFindReplaceDialog->IsDialogMessage(pMsg))
			{
				return TRUE;
			}
		}

		if (/*pMsg->message == WM_KEYDOWN || */pMsg->message == WM_KEYUP)
		{
			if (pMsg->wParam >= VK_PRIOR && pMsg->wParam <= VK_DOWN)
			{
				UpdateStatusBar();
			}
		}
		else if (/*pMsg->message == WM_LBUTTONDOWN || */pMsg->message == WM_LBUTTONUP/* || pMsg->message == WM_LBUTTONDBLCLK*/)
		{
			UpdateStatusBar();
		}
		else if (pMsg->message == WM_LBUTTONDBLCLK)
		{
			UpdateStatusBar();
		}

		if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		{
			return TRUE;
		}

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
		MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColorEdit)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_NCRBUTTONUP, OnNcRButtonUp)
		MESSAGE_HANDLER(WM_CANCELMODE, OnCancelMode)
		MESSAGE_HANDLER(WM_ENTERMENULOOP, OnEnterMenuLoop)
		MESSAGE_HANDLER(WM_EXITMENULOOP, OnExitMenuLoop)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		MESSAGE_HANDLER(UWM_FINDMSGSTRING, OnFindMsgString)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_CHANGEPASSWORD, OnFileChangePassword)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_9, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_10, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_12, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_14, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_FONT_ARIAL, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_COURIER_NEW, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_LUCIDA_CONSOLE, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_TAHOMA, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_VERDANA, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_CONSOLAS, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_CASCADIA_CODE, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_CHANGE_LANGUAGE, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_ENGLISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_GERMAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_FRENCH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_SPANISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_ITALIAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_PORTUGUESE, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_DUTCH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_SWEDISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_POLISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_RUSSIAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(ID_STEGANOS_PASSWORD_MANAGER, OnSetEncryptionMode)
		COMMAND_ID_HANDLER(ID_STEGANOS_SAFE, OnSetEncryptionMode)
		COMMAND_ID_HANDLER(ID_THEME_SYSTEM, OnChangeTheme)
		COMMAND_ID_HANDLER(ID_THEME_LIGHT, OnChangeTheme)
		COMMAND_ID_HANDLER(ID_THEME_DARK, OnChangeTheme)
		COMMAND_ID_HANDLER(kFindPanelToggleButtonId, OnFindPanelToggleReplace)
		COMMAND_ID_HANDLER(kFindPanelFindNextButtonId, OnFindPanelFindNext)
		COMMAND_ID_HANDLER(kFindPanelFindPrevButtonId, OnFindPanelFindPrev)
		COMMAND_ID_HANDLER(kFindPanelOptionsButtonId, OnFindPanelOptions)
		COMMAND_ID_HANDLER(kFindPanelCloseButtonId, OnFindPanelClose)
		COMMAND_ID_HANDLER(kFindPanelReplaceButtonId, OnFindPanelReplaceOne)
		COMMAND_ID_HANDLER(kFindPanelReplaceAllButtonId, OnFindPanelReplaceAll)
		COMMAND_HANDLER(kFindPanelFindEditId, EN_CHANGE, OnFindPanelFindTextChange)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChange)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	std::string GetText()
	{
		const int length = m_view.GetWindowTextLength();
		std::wstring wideText(static_cast<size_t>(length) + 1, L'\0');
		if (length > 0)
		{
			::GetWindowTextW(m_view, wideText.data(), length + 1);
		}
		if (!wideText.empty() && wideText.back() == L'\0')
		{
			wideText.pop_back();
		}
		return wstring_to_utf8(wideText);
	}

	bool SaveTextToFile(const std::string& path, const std::string& text, std::string& password, HWND hWnd = 0)
	{
		LOCKNOTEWINTRAITS wintraits;
		wintraits.m_nFontSize = m_nFontSize;
		wintraits.m_nWindowSizeX = m_nWindowSizeX;
		wintraits.m_nWindowSizeY = m_nWindowSizeY;
		wintraits.m_nLangId = m_nLanguage;
		wintraits.m_nKdfMode = static_cast<int>(m_kdfMode);
		wintraits.m_nThemeMode = m_nThemeMode;
		wintraits.m_strFontName = m_strFontName;
		return Utils::SaveTextToFile(path, text, password, *this, &wintraits);
	}

	void SetKdfMode(const int rawMode)
	{
		m_kdfMode = Utils::ParseKdfModeValue(rawMode);
	}

	int GetKdfMode() const
	{
		return static_cast<int>(m_kdfMode);
	}

	static ThemeMode ParseThemeMode(const int rawMode)
	{
		switch (rawMode)
		{
		case static_cast<int>(ThemeMode::Light):
			return ThemeMode::Light;
		case static_cast<int>(ThemeMode::Dark):
			return ThemeMode::Dark;
		case static_cast<int>(ThemeMode::System):
		default:
			return ThemeMode::System;
		}
	}

	int GetThemeMode() const
	{
		return m_nThemeMode;
	}

	void SetThemeMode(const int rawMode)
	{
		m_nThemeMode = static_cast<int>(ParseThemeMode(rawMode));
	}

	bool IsRussianUi() const
	{
		return static_cast<WORD>(m_nLanguage) == LANG_RUSSIAN;
	}

	std::wstring GetFindPreviousMenuCaption() const
	{
		return IsRussianUi() ? L"\u041D\u0430\u0439\u0442\u0438 \u0440\u0430\u043D\u0435\u0435\tShift+F3" : L"Find previous\tShift+F3";
	}

	std::wstring GetReplaceMenuCaption() const
	{
		return IsRussianUi() ? L"\u0417\u0430\u043C\u0435\u043D\u0438\u0442\u044C...\tCtrl+H" : L"Replace...\tCtrl+H";
	}

	std::wstring GetThemeMenuCaption() const
	{
		return IsRussianUi() ? L"\u0422\u0435\u043C\u0430" : L"Theme";
	}

	std::wstring GetThemeSystemCaption() const
	{
		return IsRussianUi() ? L"\u0421\u0438\u0441\u0442\u0435\u043C\u043D\u0430\u044F" : L"System";
	}

	std::wstring GetThemeLightCaption() const
	{
		return IsRussianUi() ? L"\u0421\u0432\u0435\u0442\u043B\u0430\u044F" : L"Light";
	}

	std::wstring GetThemeDarkCaption() const
	{
		return IsRussianUi() ? L"\u0422\u0435\u043C\u043D\u0430\u044F" : L"Dark";
	}

	std::wstring GetEncryptionMenuCaption() const
	{
		return IsRussianUi() ? L"\u0428\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435" : L"Encryption";
	}

	std::wstring GetEncryptionModernCaption() const
	{
		return IsRussianUi()
			? L"\u0421\u043E\u0432\u0440\u0435\u043C\u0435\u043D\u043D\u043E\u0435 \u0448\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435 (scrypt)"
			: L"Modern encryption (scrypt)";
	}

	std::wstring GetEncryptionCompatibilityCaption() const
	{
		return IsRussianUi()
			? L"\u0421\u043E\u0432\u043C\u0435\u0441\u0442\u0438\u043C\u043E\u0435 \u0448\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435 (PBKDF2-SHA256)"
			: L"Compatibility encryption (PBKDF2-SHA256)";
	}

	std::wstring GetFindPanelMatchCaseCaption() const
	{
		return IsRussianUi() ? L"\u0421 \u0443\u0447\u0435\u0442\u043E\u043C \u0440\u0435\u0433\u0438\u0441\u0442\u0440\u0430" : L"Match case";
	}

	std::wstring GetFindPanelWrapCaption() const
	{
		return IsRussianUi() ? L"\u0426\u0438\u043A\u043B\u0438\u0447\u0435\u0441\u043A\u0438\u0439 \u043F\u0435\u0440\u0435\u0445\u043E\u0434" : L"Wrap around";
	}

	std::wstring GetFindPanelReplaceCaption() const
	{
		return IsRussianUi() ? L"\u0417\u0430\u043C\u0435\u043D\u0438\u0442\u044C" : L"Replace";
	}

	std::wstring GetFindPanelReplaceAllCaption() const
	{
		return IsRussianUi() ? L"\u0417\u0430\u043C\u0435\u043D\u0438\u0442\u044C \u0432\u0441\u0435" : L"Replace all";
	}

	std::wstring GetFindPanelFindCueText() const
	{
		return IsRussianUi() ? L"\u041D\u0430\u0439\u0442\u0438" : L"Find";
	}

	std::wstring GetFindPanelReplaceCueText() const
	{
		return IsRussianUi() ? L"\u0417\u0430\u043C\u0435\u043D\u0438\u0442\u044C" : L"Replace";
	}

	bool IsStatusBarVisible() const
	{
		return m_isStatusBarVisible;
	}

	void SetStatusBarVisible(const bool visible)
	{
		if (m_isStatusBarVisible == visible)
		{
			return;
		}
		m_isStatusBarVisible = visible;
		m_bTraitsChanged = true;
		LayoutMainControls();
		UpdateStatusBar();
		if (m_hWndStatusBar)
		{
			::InvalidateRect(m_hWndStatusBar, nullptr, FALSE);
		}
		Invalidate(FALSE);
	}

	bool IsWordWrapEnabled() const
	{
		return m_isWordWrapEnabled;
	}

	void SetWordWrapEnabled(const bool enabled)
	{
		if (!m_view.m_hWnd)
		{
			m_isWordWrapEnabled = enabled;
			return;
		}

		const LONG_PTR oldStyle = ::GetWindowLongPtrW(m_view, GWL_STYLE);
		LONG_PTR newStyle = oldStyle;
		newStyle |= (WS_VSCROLL | ES_AUTOVSCROLL);
		if (enabled)
		{
			newStyle &= ~WS_HSCROLL;
			newStyle &= ~ES_AUTOHSCROLL;
		}
		else
		{
			newStyle |= WS_HSCROLL;
			newStyle |= ES_AUTOHSCROLL;
		}

		if (newStyle != oldStyle)
		{
			::SetWindowLongPtrW(m_view, GWL_STYLE, newStyle);
			::SetWindowPos(
				m_view,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			m_bTraitsChanged = true;
		}
		m_isWordWrapEnabled = enabled;
		UpdateEditorVerticalScrollbarVisibility();
		UpdateEditorFormattingRect();
		UpdateStatusBar();
		m_view.Invalidate();
	}

	bool ShouldShowEditorVerticalScrollbar() const
	{
		if (!m_view.m_hWnd || !::IsWindow(m_view.m_hWnd))
		{
			return false;
		}

		RECT textRect{};
		::SendMessageW(m_view.m_hWnd, EM_GETRECT, 0, reinterpret_cast<LPARAM>(&textRect));
		const int textHeight = static_cast<int>(textRect.bottom - textRect.top);
		if (textHeight <= 0)
		{
			return false;
		}

		int lineHeight = 0;
		HDC hdc = ::GetDC(m_view.m_hWnd);
		if (hdc != nullptr)
		{
			const HFONT hEditFont = reinterpret_cast<HFONT>(::SendMessageW(m_view.m_hWnd, WM_GETFONT, 0, 0));
			HGDIOBJ hOldObject = nullptr;
			if (hEditFont != nullptr)
			{
				hOldObject = ::SelectObject(hdc, hEditFont);
			}

			TEXTMETRICW tm{};
			if (::GetTextMetricsW(hdc, &tm))
			{
				lineHeight = tm.tmHeight + tm.tmExternalLeading;
			}

			if (hOldObject != nullptr)
			{
				::SelectObject(hdc, hOldObject);
			}
			::ReleaseDC(m_view.m_hWnd, hdc);
		}

		if (lineHeight <= 0)
		{
			lineHeight = 1;
		}

		const int visibleLines = std::max(1, textHeight / lineHeight);
		const int lineCount = static_cast<int>(::SendMessageW(m_view.m_hWnd, EM_GETLINECOUNT, 0, 0));
		return lineCount > visibleLines;
	}

	void UpdateEditorVerticalScrollbarVisibility() const
	{
		if (!m_view.m_hWnd || !::IsWindow(m_view.m_hWnd))
		{
			return;
		}

		const bool shouldShowScrollbar = ShouldShowEditorVerticalScrollbar();
		const LONG_PTR oldStyle = ::GetWindowLongPtrW(m_view.m_hWnd, GWL_STYLE);
		LONG_PTR newStyle = oldStyle;
		if (shouldShowScrollbar)
		{
			newStyle |= (WS_VSCROLL | ES_AUTOVSCROLL);
		}
		else
		{
			newStyle &= ~WS_VSCROLL;
		}

		if (newStyle != oldStyle)
		{
			::SetWindowLongPtrW(m_view.m_hWnd, GWL_STYLE, newStyle);
			::SetWindowPos(
				m_view.m_hWnd,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}

		::ShowScrollBar(m_view.m_hWnd, SB_VERT, shouldShowScrollbar ? TRUE : FALSE);
	}

	void UpdateEditorFormattingRect() const
	{
		if (!m_view.m_hWnd || !::IsWindow(m_view.m_hWnd))
		{
			return;
		}

		// Keep native edit formatting area bound to full client size.
		// Custom EM_SETRECT widths can leave a stale internal wrap boundary
		// after window resize and look like an "inner text resize zone".
		::SendMessageW(m_view.m_hWnd, EM_SETRECTNP, 0, 0);
	}

	bool IsFormattingEnabled() const
	{
		return m_isFormattingEnabled;
	}

	void SetFormattingEnabled(const bool enabled)
	{
		m_isFormattingEnabled = enabled;
	}

	void ApplyThemeSetting(const ThemeMode mode)
	{
		const UINT commandId = mode == ThemeMode::Dark
			? ID_THEME_DARK
			: (mode == ThemeMode::Light ? ID_THEME_LIGHT : ID_THEME_SYSTEM);
		BOOL bHandled = FALSE;
		OnChangeTheme(0, static_cast<WORD>(commandId), nullptr, bHandled);
	}

	void ApplyFontFaceByName(const std::string& fontName)
	{
		UINT commandId = ID_FONT_CONSOLAS;
		if (fontName == NAME_FONT_ARIAL)
		{
			commandId = ID_FONT_ARIAL;
		}
		else if (fontName == NAME_FONT_COURIER_NEW)
		{
			commandId = ID_FONT_COURIER_NEW;
		}
		else if (fontName == NAME_FONT_LUCIDA_CONSOLE)
		{
			commandId = ID_FONT_LUCIDA_CONSOLE;
		}
		else if (fontName == NAME_FONT_TAHOMA)
		{
			commandId = ID_FONT_TAHOMA;
		}
		else if (fontName == NAME_FONT_VERDANA)
		{
			commandId = ID_FONT_VERDANA;
		}
		else if (fontName == NAME_FONT_CASCADIA_CODE)
		{
			commandId = ID_FONT_CASCADIA_CODE;
		}

		BOOL bHandled = FALSE;
		OnViewFontTypeFace(0, static_cast<WORD>(commandId), nullptr, bHandled);
	}

	void ApplyFontSizeValue(const int fontSize)
	{
		UINT commandId = ID_VIEW_FONTSIZE_10;
		switch (fontSize)
		{
		case 9:
			commandId = ID_VIEW_FONTSIZE_9;
			break;
		case 12:
			commandId = ID_VIEW_FONTSIZE_12;
			break;
		case 14:
			commandId = ID_VIEW_FONTSIZE_14;
			break;
		case 10:
		default:
			commandId = ID_VIEW_FONTSIZE_10;
			break;
		}

		BOOL bHandled = FALSE;
		OnViewFontSize(0, static_cast<WORD>(commandId), nullptr, bHandled);
	}

	void ApplyLanguageById(const WORD langId)
	{
		BOOL bHandled = FALSE;
		OnChangeLanguage(0, langId, nullptr, bHandled);
	}

	std::wstring GetLanguageDisplayName() const
	{
		switch (static_cast<WORD>(m_nLanguage))
		{
		case LANG_RUSSIAN:
			return L"\u0420\u0443\u0441\u0441\u043a\u0438\u0439";
		case LANG_GERMAN:
			return L"Deutsch";
		case LANG_FRENCH:
			return L"Fran\u00E7ais";
		case LANG_SPANISH:
			return L"Espa\u00F1ol";
		case LANG_ITALIAN:
			return L"Italiano";
		case LANG_PORTUGUESE:
			return L"Portugu\u00EAs";
		case LANG_DUTCH:
			return L"Nederlands";
		case LANG_SWEDISH:
			return L"Svenska";
		case LANG_POLISH:
			return L"Polski";
		case LANG_ENGLISH:
		default:
			return L"English";
		}
	}

	bool IsHighContrastEnabled() const
	{
		HIGHCONTRASTW hc{ sizeof(hc) };
		if (!::SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0))
		{
			return false;
		}
		return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
	}

	bool IsSystemDarkThemeEnabled() const
	{
		DWORD value = 1;
		DWORD valueSize = sizeof(value);
		const LSTATUS status = ::RegGetValueW(
			HKEY_CURRENT_USER,
			L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
			L"AppsUseLightTheme",
			RRF_RT_REG_DWORD,
			nullptr,
			&value,
			&valueSize);
		if (status != ERROR_SUCCESS)
		{
			return false;
		}
		return value == 0;
	}

	bool ShouldUseDarkTheme() const
	{
		if (IsHighContrastEnabled())
		{
			return false;
		}

		switch (ParseThemeMode(m_nThemeMode))
		{
		case ThemeMode::Dark:
			return true;
		case ThemeMode::Light:
			return false;
		case ThemeMode::System:
		default:
			return IsSystemDarkThemeEnabled();
		}
	}

	void ApplyTitleBarTheme()
	{
		using DwmSetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
		if (!m_hWnd)
		{
			return;
		}

		HMODULE hDwmApi = ::LoadLibraryW(L"dwmapi.dll");
		if (!hDwmApi)
		{
			return;
		}

		const auto dwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
			::GetProcAddress(hDwmApi, "DwmSetWindowAttribute"));
		if (dwmSetWindowAttribute)
		{
			const BOOL useDark = m_isDarkThemeApplied ? TRUE : FALSE;
			constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
			constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
			constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
			constexpr DWORD DWMWA_BORDER_COLOR = 34;
			constexpr DWORD DWMWA_CAPTION_COLOR = 35;
			constexpr DWORD DWMWA_TEXT_COLOR = 36;
			constexpr DWORD DWMWA_SYSTEMBACKDROP_TYPE = 38;
			constexpr DWORD DWMWA_MICA_EFFECT = 1029;
			constexpr DWORD DWMSBT_NONE = 1;
			const COLORREF captionColor = m_titleBarBackgroundColor;
			const COLORREF borderColor = BlendColors(captionColor, m_topBarTextColor, m_isDarkThemeApplied ? 22 : 14);
			const COLORREF textColor = m_topBarTextColor;
			const DWORD cornerPreference = UseCustomTitleBar() ? 2U : 0U; // DWMWCP_ROUND / DWMWCP_DEFAULT
			const DWORD backdropType = DWMSBT_NONE;
			const BOOL micaEnabled = FALSE;
			dwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
			dwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &useDark, sizeof(useDark));
			dwmSetWindowAttribute(m_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
			dwmSetWindowAttribute(m_hWnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(borderColor));
			dwmSetWindowAttribute(m_hWnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));
			dwmSetWindowAttribute(m_hWnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
			dwmSetWindowAttribute(m_hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
			dwmSetWindowAttribute(m_hWnd, DWMWA_MICA_EFFECT, &micaEnabled, sizeof(micaEnabled));
		}

		::FreeLibrary(hDwmApi);
	}

	void ApplyExplorerTheme(HWND hWndTarget, const bool dark) const
	{
		using SetWindowThemeFn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
		if (hWndTarget == nullptr)
		{
			return;
		}

		HMODULE hUxTheme = ::LoadLibraryW(L"uxtheme.dll");
		if (!hUxTheme)
		{
			return;
		}

		const auto setWindowTheme = reinterpret_cast<SetWindowThemeFn>(::GetProcAddress(hUxTheme, "SetWindowTheme"));
		if (setWindowTheme)
		{
			setWindowTheme(hWndTarget, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
		}

		::FreeLibrary(hUxTheme);
	}

	void ApplyWindowThemeName(HWND hWndTarget, LPCWSTR themeName) const
	{
		using SetWindowThemeFn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
		if (hWndTarget == nullptr)
		{
			return;
		}

		HMODULE hUxTheme = ::LoadLibraryW(L"uxtheme.dll");
		if (!hUxTheme)
		{
			return;
		}

		const auto setWindowTheme = reinterpret_cast<SetWindowThemeFn>(::GetProcAddress(hUxTheme, "SetWindowTheme"));
		if (setWindowTheme)
		{
			setWindowTheme(hWndTarget, themeName, nullptr);
		}

		::FreeLibrary(hUxTheme);
	}

	void ApplyStatusBarTheme() const
	{
		using SetWindowThemeFn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
		if (m_hWndStatusBar == nullptr)
		{
			return;
		}

		HMODULE hUxTheme = ::LoadLibraryW(L"uxtheme.dll");
		if (!hUxTheme)
		{
			return;
		}

		const auto setWindowTheme = reinterpret_cast<SetWindowThemeFn>(::GetProcAddress(hUxTheme, "SetWindowTheme"));
		if (setWindowTheme)
		{
			// Status bar is fully owner-drawn; disable themed borders/separators.
			setWindowTheme(m_hWndStatusBar, L"", L"");
		}

		::FreeLibrary(hUxTheme);
	}

	void NormalizeStatusBarStyles() const
	{
		if (m_hWndStatusBar == nullptr)
		{
			return;
		}

		const LONG_PTR style = ::GetWindowLongPtrW(m_hWndStatusBar, GWL_STYLE);
		::SetWindowLongPtrW(
			m_hWndStatusBar,
			GWL_STYLE,
			(style & ~SBARS_SIZEGRIP & ~WS_BORDER) | CCS_NODIVIDER);

		const LONG_PTR exStyle = ::GetWindowLongPtrW(m_hWndStatusBar, GWL_EXSTYLE);
		::SetWindowLongPtrW(
			m_hWndStatusBar,
			GWL_EXSTYLE,
			exStyle & ~WS_EX_CLIENTEDGE & ~WS_EX_STATICEDGE & ~WS_EX_WINDOWEDGE);

		::SetWindowPos(
			m_hWndStatusBar,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void ApplyDarkModeForWindow(HWND hWndTarget) const
	{
		using AllowDarkModeForWindowFn = BOOL(WINAPI*)(HWND, BOOL);
		if (hWndTarget == nullptr)
		{
			return;
		}

		HMODULE hUxTheme = ::LoadLibraryW(L"uxtheme.dll");
		if (!hUxTheme)
		{
			return;
		}

		const auto allowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindowFn>(
			::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133)));
		if (allowDarkModeForWindow)
		{
			allowDarkModeForWindow(hWndTarget, m_isDarkThemeApplied ? TRUE : FALSE);
		}

		::FreeLibrary(hUxTheme);
	}

	static LPCWSTR GetFindDialogOwnerPropName()
	{
		return L"LockNote2.FindDialogOwner";
	}

	static LPCWSTR GetChromeResizeOwnerPropName()
	{
		return L"LockNote2.ChromeResizeOwner";
	}

	static LPCWSTR GetChromeResizeOriginalProcPropName()
	{
		return L"LockNote2.ChromeResizeOriginalProc";
	}

	void AttachChromeResizeSubclass(HWND hChild)
	{
		if (hChild == nullptr || !::IsWindow(hChild))
		{
			return;
		}

		const auto currentProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(hChild, GWLP_WNDPROC));
		const auto resizeProc = static_cast<WNDPROC>(&CMainFrame::ChromeResizeWindowProc);
		if (currentProc == resizeProc)
		{
			::SetPropW(hChild, GetChromeResizeOwnerPropName(), reinterpret_cast<HANDLE>(this));
			return;
		}

		::SetPropW(hChild, GetChromeResizeOwnerPropName(), reinterpret_cast<HANDLE>(this));
		::SetPropW(hChild, GetChromeResizeOriginalProcPropName(), reinterpret_cast<HANDLE>(currentProc));
		::SetWindowLongPtrW(hChild, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(resizeProc));
	}

	void DetachChromeResizeSubclass(HWND hChild) const
	{
		if (hChild == nullptr || !::IsWindow(hChild))
		{
			return;
		}

		const auto resizeProc = static_cast<WNDPROC>(&CMainFrame::ChromeResizeWindowProc);
		const auto currentProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(hChild, GWLP_WNDPROC));
		const auto originalProc = reinterpret_cast<WNDPROC>(::GetPropW(hChild, GetChromeResizeOriginalProcPropName()));
		if (currentProc == resizeProc && originalProc != nullptr)
		{
			::SetWindowLongPtrW(hChild, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalProc));
		}

		::RemovePropW(hChild, GetChromeResizeOwnerPropName());
		::RemovePropW(hChild, GetChromeResizeOriginalProcPropName());
	}

	static LRESULT CALLBACK ChromeResizeWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto* self = reinterpret_cast<CMainFrame*>(::GetPropW(hWnd, GetChromeResizeOwnerPropName()));
		auto originalProc = reinterpret_cast<WNDPROC>(::GetPropW(hWnd, GetChromeResizeOriginalProcPropName()));

		if (uMsg == WM_NCHITTEST && self != nullptr)
		{
			const POINT ptScreen{
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam)
			};
			POINT ptClient = ptScreen;
			::ScreenToClient(self->m_hWnd, &ptClient);
			const int resizeHit = self->HitTestWindowResizeEdges(ptClient, false);
			if (resizeHit != HTCLIENT)
			{
				// Hand edge hit-testing back to the frame so system sizing targets the top-level window.
				return HTTRANSPARENT;
			}
		}

		if (uMsg == WM_NCDESTROY)
		{
			::RemovePropW(hWnd, GetChromeResizeOwnerPropName());
			::RemovePropW(hWnd, GetChromeResizeOriginalProcPropName());
		}

		if (originalProc != nullptr)
		{
			return ::CallWindowProcW(originalProc, hWnd, uMsg, wParam, lParam);
		}
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	void AttachFindDialogSubclass(HWND hFindDlg)
	{
		if (hFindDlg == nullptr || !::IsWindow(hFindDlg))
		{
			return;
		}

		const auto currentProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(hFindDlg, GWLP_WNDPROC));
		const auto findDialogProc = static_cast<WNDPROC>(&CMainFrame::FindDialogWindowProc);
		if (currentProc == findDialogProc && m_findDialogOriginalProc != nullptr)
		{
			::SetPropW(hFindDlg, GetFindDialogOwnerPropName(), reinterpret_cast<HANDLE>(this));
			return;
		}

		m_findDialogOriginalProc = currentProc;
		::SetPropW(hFindDlg, GetFindDialogOwnerPropName(), reinterpret_cast<HANDLE>(this));
		::SetWindowLongPtrW(hFindDlg, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(findDialogProc));
	}

	void DetachFindDialogSubclass(HWND hFindDlg)
	{
		if (hFindDlg == nullptr || !::IsWindow(hFindDlg))
		{
			return;
		}

		const auto findDialogProc = static_cast<WNDPROC>(&CMainFrame::FindDialogWindowProc);
		const auto currentProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(hFindDlg, GWLP_WNDPROC));
		if (currentProc == findDialogProc && m_findDialogOriginalProc != nullptr)
		{
			::SetWindowLongPtrW(hFindDlg, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_findDialogOriginalProc));
		}

		::RemovePropW(hFindDlg, GetFindDialogOwnerPropName());
		m_findDialogOriginalProc = nullptr;
	}

	static LRESULT CALLBACK FindDialogWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto* self = reinterpret_cast<CMainFrame*>(::GetPropW(hWnd, GetFindDialogOwnerPropName()));
		WNDPROC originalProc = (self != nullptr) ? self->m_findDialogOriginalProc : nullptr;
		if (uMsg == WM_PAINT && originalProc != nullptr)
		{
			const LRESULT result = ::CallWindowProcW(originalProc, hWnd, uMsg, wParam, lParam);
			if (self != nullptr)
			{
				self->DrawFindDialogGroupBoxOverlay(hWnd);
			}
			return result;
		}
		if ((uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGED) && originalProc != nullptr)
		{
			const LRESULT result = ::CallWindowProcW(originalProc, hWnd, uMsg, wParam, lParam);
			if (self != nullptr && self->m_isDarkThemeApplied)
			{
				self->DrawFindDialogGroupBoxOverlay(hWnd);
			}
			return result;
		}

		if (self != nullptr && self->m_isDarkThemeApplied)
		{
			switch (uMsg)
			{
			case WM_COMMAND:
				break;
			case WM_DRAWITEM:
			{
				const auto* pDrawItem = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
				if (self->DrawFindDialogButton(pDrawItem))
				{
					return TRUE;
				}
				break;
			}
			case WM_ERASEBKGND:
			{
				if (self->m_frameBackgroundBrush.m_hBrush)
				{
					RECT rcClient{};
					::GetClientRect(hWnd, &rcClient);
					::FillRect(reinterpret_cast<HDC>(wParam), &rcClient, self->m_frameBackgroundBrush.m_hBrush);
					return TRUE;
				}
				break;
			}
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSTATIC:
			{
				if (self->m_frameBackgroundBrush.m_hBrush)
				{
					HDC hdc = reinterpret_cast<HDC>(wParam);
					::SetBkMode(hdc, TRANSPARENT);
					::SetBkColor(hdc, self->m_frameBackgroundColor);
					::SetTextColor(hdc, self->m_topBarTextColor);
					return reinterpret_cast<LRESULT>(self->m_frameBackgroundBrush.m_hBrush);
				}
				break;
			}
			case WM_CTLCOLORBTN:
			{
				if (self->m_frameBackgroundBrush.m_hBrush)
				{
					HDC hdc = reinterpret_cast<HDC>(wParam);
					HWND hControl = reinterpret_cast<HWND>(lParam);
					const LONG_PTR style = (hControl != nullptr) ? ::GetWindowLongPtrW(hControl, GWL_STYLE) : 0;
					const LONG_PTR buttonType = style & BS_TYPEMASK;
					const bool isRadioOrCheck =
						buttonType == BS_AUTORADIOBUTTON ||
						buttonType == BS_RADIOBUTTON ||
						buttonType == BS_AUTO3STATE ||
						buttonType == BS_3STATE ||
						buttonType == BS_AUTOCHECKBOX ||
						buttonType == BS_CHECKBOX ||
						buttonType == BS_GROUPBOX;
					if (isRadioOrCheck)
					{
						::SetBkMode(hdc, TRANSPARENT);
						::SetBkColor(hdc, self->m_frameBackgroundColor);
						::SetTextColor(hdc, self->m_topBarTextColor);
						return reinterpret_cast<LRESULT>(self->m_frameBackgroundBrush.m_hBrush);
					}
				}
				break;
			}
			case WM_CTLCOLOREDIT:
			{
				HDC hdc = reinterpret_cast<HDC>(wParam);
				::SetBkMode(hdc, OPAQUE);
				::SetBkColor(hdc, self->m_viewBackgroundColor);
				::SetTextColor(hdc, self->m_viewTextColor);
				if (self->m_viewBackgroundBrush.m_hBrush)
				{
					return reinterpret_cast<LRESULT>(self->m_viewBackgroundBrush.m_hBrush);
				}
				if (self->m_frameBackgroundBrush.m_hBrush)
				{
					return reinterpret_cast<LRESULT>(self->m_frameBackgroundBrush.m_hBrush);
				}
				break;
			}
			default:
				break;
			}
		}

		if (uMsg == WM_NCDESTROY && self != nullptr)
		{
			::RemovePropW(hWnd, GetFindDialogOwnerPropName());
			self->m_findDialogOriginalProc = nullptr;
		}

		if (originalProc != nullptr)
		{
			return ::CallWindowProcW(originalProc, hWnd, uMsg, wParam, lParam);
		}
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	static HWND FindFirstGroupBoxControl(HWND hParent)
	{
		if (hParent == nullptr)
		{
			return nullptr;
		}

		HWND hChild = nullptr;
		while ((hChild = ::FindWindowExW(hParent, hChild, L"Button", nullptr)) != nullptr)
		{
			const LONG_PTR style = ::GetWindowLongPtrW(hChild, GWL_STYLE);
			if ((style & BS_TYPEMASK) == BS_GROUPBOX)
			{
				return hChild;
			}
		}
		return nullptr;
	}

	static bool IsFindDialogPushButtonType(const LONG_PTR buttonType)
	{
		return
			buttonType == BS_PUSHBUTTON ||
			buttonType == BS_DEFPUSHBUTTON ||
			buttonType == BS_OWNERDRAW;
	}

	static bool IsFindDialogCheckboxControlId(const int controlId)
	{
		return controlId == chx1 || controlId == chx2;
	}

	static bool IsFindDialogDirectionRadioControlId(const int controlId)
	{
		return controlId == rad1 || controlId == rad2;
	}

	static bool IsFindDialogToggleControlId(const int controlId)
	{
		return IsFindDialogCheckboxControlId(controlId) || IsFindDialogDirectionRadioControlId(controlId);
	}

	void NormalizeFindDialogEditStyles(HWND hEdit) const
	{
		if (!m_isDarkThemeApplied || hEdit == nullptr || !::IsWindow(hEdit))
		{
			return;
		}

		bool changed = false;
		const LONG_PTR exStyle = ::GetWindowLongPtrW(hEdit, GWL_EXSTYLE);
		const LONG_PTR desiredExStyle = exStyle & ~WS_EX_CLIENTEDGE & ~WS_EX_STATICEDGE & ~WS_EX_WINDOWEDGE;
		if (desiredExStyle != exStyle)
		{
			::SetWindowLongPtrW(hEdit, GWL_EXSTYLE, desiredExStyle);
			changed = true;
		}

		const LONG_PTR style = ::GetWindowLongPtrW(hEdit, GWL_STYLE);
		const LONG_PTR desiredStyle = style | WS_BORDER;
		if (desiredStyle != style)
		{
			::SetWindowLongPtrW(hEdit, GWL_STYLE, desiredStyle);
			changed = true;
		}

		if (changed)
		{
			::SetWindowPos(
				hEdit,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
	}

	void UpdateFindDialogButtonStyles(HWND hButton) const
	{
		if (hButton == nullptr || !::IsWindow(hButton))
		{
			return;
		}

		const int controlId = ::GetDlgCtrlID(hButton);
		const LONG_PTR style = ::GetWindowLongPtrW(hButton, GWL_STYLE);
		const LONG_PTR buttonType = style & BS_TYPEMASK;
		const bool isToggleControl = IsFindDialogToggleControlId(controlId);
		const bool useOwnerDraw = m_isDarkThemeApplied &&
			(controlId == IDOK || controlId == IDCANCEL || (IsFindDialogPushButtonType(buttonType) && !isToggleControl));

		LONG_PTR desiredStyle = style;
		if (useOwnerDraw)
		{
			desiredStyle = (style & ~BS_TYPEMASK) | BS_OWNERDRAW;
		}
		else if (buttonType == BS_OWNERDRAW)
		{
			LONG_PTR restoreButtonType = BS_PUSHBUTTON;
			if (controlId == IDOK)
			{
				restoreButtonType = BS_DEFPUSHBUTTON;
			}
			else if (IsFindDialogDirectionRadioControlId(controlId))
			{
				restoreButtonType = BS_AUTORADIOBUTTON;
			}
			else if (IsFindDialogCheckboxControlId(controlId))
			{
				restoreButtonType = BS_AUTOCHECKBOX;
			}
			desiredStyle = (style & ~BS_TYPEMASK) | restoreButtonType;
		}

		if (desiredStyle != style)
		{
			::SetWindowLongPtrW(hButton, GWL_STYLE, desiredStyle);
			::SetWindowPos(
				hButton,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
	}

	bool DrawFindDialogToggleButton(const DRAWITEMSTRUCT* pDrawItem, const HWND hButton) const
	{
		if (pDrawItem == nullptr || hButton == nullptr || !::IsWindow(hButton))
		{
			return false;
		}

		const int controlId = ::GetDlgCtrlID(hButton);
		const bool isCheckbox = IsFindDialogCheckboxControlId(controlId);
		const bool isDirectionRadio = IsFindDialogDirectionRadioControlId(controlId);
		if (!isCheckbox && !isDirectionRadio)
		{
			return false;
		}

		HDC hdc = pDrawItem->hDC;
		const RECT rc = pDrawItem->rcItem;
		const bool isDisabled = (pDrawItem->itemState & ODS_DISABLED) != 0;
		const bool hasFocus = (pDrawItem->itemState & ODS_FOCUS) != 0;
		const bool isChecked = (::SendMessageW(hButton, BM_GETCHECK, 0, 0) == BST_CHECKED);
		const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);

		CBrush backgroundBrush;
		backgroundBrush.CreateSolidBrush(m_frameBackgroundColor);
		::FillRect(hdc, &rc, backgroundBrush);

		const int glyphSize = MulDiv(14, dpiX, 96);
		const int leftInset = MulDiv(2, dpiX, 96);
		const int glyphTop = rc.top + ((rc.bottom - rc.top - glyphSize) / 2);
		RECT rcGlyph{
			rc.left + leftInset,
			glyphTop,
			rc.left + leftInset + glyphSize,
			glyphTop + glyphSize
		};

		const COLORREF borderColor = isDisabled ? RGB(112, 112, 112) : RGB(178, 178, 178);
		const COLORREF fillColor = RGB(35, 35, 35);
		const COLORREF accentColor = RGB(77, 166, 255);
		const COLORREF textColor = isDisabled ? RGB(128, 128, 128) : m_topBarTextColor;

		if (isCheckbox)
		{
			HBRUSH hFillBrush = ::CreateSolidBrush(fillColor);
			HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, borderColor);
			HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
			HGDIOBJ hOldBrush = hFillBrush ? ::SelectObject(hdc, hFillBrush) : nullptr;
			::Rectangle(hdc, rcGlyph.left, rcGlyph.top, rcGlyph.right, rcGlyph.bottom);
			if (hOldBrush != nullptr)
			{
				::SelectObject(hdc, hOldBrush);
			}
			if (hOldPen != nullptr)
			{
				::SelectObject(hdc, hOldPen);
			}
			if (hBorderPen != nullptr)
			{
				::DeleteObject(hBorderPen);
			}
			if (hFillBrush != nullptr)
			{
				::DeleteObject(hFillBrush);
			}

			if (isChecked)
			{
				HPEN hCheckPen = ::CreatePen(PS_SOLID, (std::max)(1, MulDiv(2, dpiX, 96)), accentColor);
				HGDIOBJ hOldCheckPen = hCheckPen ? ::SelectObject(hdc, hCheckPen) : nullptr;
				::MoveToEx(hdc, rcGlyph.left + MulDiv(3, dpiX, 96), rcGlyph.top + MulDiv(7, dpiX, 96), nullptr);
				::LineTo(hdc, rcGlyph.left + MulDiv(6, dpiX, 96), rcGlyph.bottom - MulDiv(3, dpiX, 96));
				::LineTo(hdc, rcGlyph.right - MulDiv(2, dpiX, 96), rcGlyph.top + MulDiv(3, dpiX, 96));
				if (hOldCheckPen != nullptr)
				{
					::SelectObject(hdc, hOldCheckPen);
				}
				if (hCheckPen != nullptr)
				{
					::DeleteObject(hCheckPen);
				}
			}
		}
		else
		{
			HBRUSH hFillBrush = ::CreateSolidBrush(fillColor);
			HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, borderColor);
			HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
			HGDIOBJ hOldBrush = hFillBrush ? ::SelectObject(hdc, hFillBrush) : nullptr;
			::Ellipse(hdc, rcGlyph.left, rcGlyph.top, rcGlyph.right, rcGlyph.bottom);
			if (hOldBrush != nullptr)
			{
				::SelectObject(hdc, hOldBrush);
			}
			if (hOldPen != nullptr)
			{
				::SelectObject(hdc, hOldPen);
			}
			if (hBorderPen != nullptr)
			{
				::DeleteObject(hBorderPen);
			}
			if (hFillBrush != nullptr)
			{
				::DeleteObject(hFillBrush);
			}

			if (isChecked)
			{
				RECT rcDot = rcGlyph;
				::InflateRect(&rcDot, -MulDiv(4, dpiX, 96), -MulDiv(4, dpiX, 96));
				HBRUSH hDotBrush = ::CreateSolidBrush(accentColor);
				HGDIOBJ hOldDotPen = ::SelectObject(hdc, ::GetStockObject(NULL_PEN));
				HGDIOBJ hOldDotBrush = hDotBrush ? ::SelectObject(hdc, hDotBrush) : nullptr;
				::Ellipse(hdc, rcDot.left, rcDot.top, rcDot.right, rcDot.bottom);
				if (hOldDotBrush != nullptr)
				{
					::SelectObject(hdc, hOldDotBrush);
				}
				if (hOldDotPen != nullptr)
				{
					::SelectObject(hdc, hOldDotPen);
				}
				if (hDotBrush != nullptr)
				{
					::DeleteObject(hDotBrush);
				}
			}
		}

		wchar_t caption[160]{};
		::GetWindowTextW(hButton, caption, static_cast<int>(std::size(caption)));
		::SetBkMode(hdc, TRANSPARENT);
		::SetTextColor(hdc, textColor);
		RECT rcText = rc;
		rcText.left = rcGlyph.right + MulDiv(8, dpiX, 96);
		::DrawTextW(hdc, caption, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

		if (hasFocus && !isDisabled)
		{
			RECT rcFocus = rcText;
			rcFocus.right = (std::max)(rcFocus.left + 1, rcFocus.right - MulDiv(2, dpiX, 96));
			::InflateRect(&rcFocus, -1, -2);
			::DrawFocusRect(hdc, &rcFocus);
		}

		return true;
	}

	bool DrawFindDialogButton(const DRAWITEMSTRUCT* pDrawItem) const
	{
		if (!m_isDarkThemeApplied || pDrawItem == nullptr || pDrawItem->CtlType != ODT_BUTTON)
		{
			return false;
		}

		const HWND hButton = pDrawItem->hwndItem;
		if (hButton == nullptr || !::IsWindow(hButton))
		{
			return false;
		}

		const LONG_PTR style = ::GetWindowLongPtrW(hButton, GWL_STYLE);
		if ((style & BS_TYPEMASK) != BS_OWNERDRAW)
		{
			return false;
		}

		if (DrawFindDialogToggleButton(pDrawItem, hButton))
		{
			return true;
		}

		HDC hdc = pDrawItem->hDC;
		RECT rc = pDrawItem->rcItem;
		const bool isDisabled = (pDrawItem->itemState & ODS_DISABLED) != 0;
		const bool isPressed = (pDrawItem->itemState & ODS_SELECTED) != 0;
		const bool isHot = (pDrawItem->itemState & ODS_HOTLIGHT) != 0;
		const bool isDefault = (pDrawItem->itemState & ODS_DEFAULT) != 0;
		const bool hasFocus = (pDrawItem->itemState & ODS_FOCUS) != 0;

		COLORREF backgroundColor = RGB(44, 44, 44);
		COLORREF borderColor = RGB(124, 124, 124);
		COLORREF textColor = m_topBarTextColor;
		if (isDisabled)
		{
			backgroundColor = RGB(50, 50, 50);
			borderColor = RGB(88, 88, 88);
			textColor = RGB(130, 130, 130);
		}
		else if (isPressed)
		{
			backgroundColor = RGB(62, 62, 62);
			borderColor = RGB(140, 140, 140);
		}
		else if (isHot)
		{
			backgroundColor = RGB(55, 55, 55);
			borderColor = RGB(150, 150, 150);
		}
		else if (isDefault)
		{
			borderColor = RGB(168, 168, 168);
		}

		CBrush backgroundBrush;
		backgroundBrush.CreateSolidBrush(backgroundColor);
		::FillRect(hdc, &rc, backgroundBrush);

		HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, borderColor);
		HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
		HGDIOBJ hOldBrush = ::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
		::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		if (hOldBrush != nullptr)
		{
			::SelectObject(hdc, hOldBrush);
		}
		if (hOldPen != nullptr)
		{
			::SelectObject(hdc, hOldPen);
		}
		if (hBorderPen != nullptr)
		{
			::DeleteObject(hBorderPen);
		}

		wchar_t caption[128]{};
		::GetWindowTextW(hButton, caption, static_cast<int>(std::size(caption)));
		::SetBkMode(hdc, TRANSPARENT);
		::SetTextColor(hdc, textColor);
		RECT rcText = rc;
		rcText.left += 6;
		rcText.right -= 6;
		::DrawTextW(hdc, caption, -1, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_HIDEPREFIX | DT_END_ELLIPSIS);

		if (hasFocus && !isDisabled)
		{
			RECT rcFocus = rc;
			::InflateRect(&rcFocus, -4, -4);
			::DrawFocusRect(hdc, &rcFocus);
		}

		return true;
	}

	void SyncFindDialogGroupBoxVisibility(HWND hFindDlg) const
	{
		const HWND hGroupBox = FindFirstGroupBoxControl(hFindDlg);
		if (hGroupBox == nullptr)
		{
			return;
		}

		::ShowWindow(hGroupBox, m_isDarkThemeApplied ? SW_HIDE : SW_SHOWNA);
	}

	void DrawFindDialogGroupBoxOverlay(HWND hFindDlg) const
	{
		if (!m_isDarkThemeApplied || hFindDlg == nullptr || !::IsWindow(hFindDlg))
		{
			return;
		}

		const HWND hGroupBox = FindFirstGroupBoxControl(hFindDlg);
		if (hGroupBox == nullptr || ::IsWindowVisible(hGroupBox))
		{
			return;
		}

		RECT rcGroup{};
		::GetWindowRect(hGroupBox, &rcGroup);
		::MapWindowPoints(nullptr, hFindDlg, reinterpret_cast<LPPOINT>(&rcGroup), 2);
		if (rcGroup.right <= rcGroup.left || rcGroup.bottom <= rcGroup.top)
		{
			return;
		}

		wchar_t caption[128]{};
		::GetWindowTextW(hGroupBox, caption, static_cast<int>(std::size(caption)));
		const int captionLength = lstrlenW(caption);

		HDC hdc = ::GetDC(hFindDlg);
		if (hdc == nullptr)
		{
			return;
		}

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(hdc, GetUiFontHandle()));
		::SetTextColor(hdc, m_topBarTextColor);
		::SetBkColor(hdc, m_frameBackgroundColor);
		::SetBkMode(hdc, TRANSPARENT);

		const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		const int titleInset = MulDiv(9, dpiX, 96);
		const int captionPadding = MulDiv(4, dpiX, 96);
		const int titleGap = MulDiv(2, dpiX, 96);
		const int minBorderTopOffset = MulDiv(6, dpiX, 96);

		SIZE captionSize{};
		if (captionLength > 0)
		{
			::GetTextExtentPoint32W(hdc, caption, captionLength, &captionSize);
		}

		const int textHeight = (std::max)(static_cast<int>(captionSize.cy), MulDiv(13, dpiX, 96));
		const int borderTop = rcGroup.top + (std::max)(textHeight / 2, minBorderTopOffset);
		const int borderLeft = rcGroup.left;
		const int borderRight = rcGroup.right - 1;
		const int borderBottom = rcGroup.bottom - 1;
		if (borderRight <= borderLeft || borderBottom <= borderTop)
		{
			if (hOldFont != nullptr)
			{
				::SelectObject(hdc, hOldFont);
			}
			::ReleaseDC(hFindDlg, hdc);
			return;
		}

		const RECT rcTitleText{
			rcGroup.left + titleInset,
			rcGroup.top,
			rcGroup.left + titleInset + static_cast<int>(captionSize.cx),
			rcGroup.top + textHeight
		};

		RECT rcTitleBackground{
			rcTitleText.left - captionPadding,
			rcTitleText.top,
			rcTitleText.right + captionPadding,
			rcTitleText.bottom
		};
		if (m_frameBackgroundBrush.m_hBrush)
		{
			::FillRect(hdc, &rcTitleBackground, m_frameBackgroundBrush.m_hBrush);
		}
		else
		{
			::SetDCBrushColor(hdc, m_frameBackgroundColor);
			::FillRect(hdc, &rcTitleBackground, static_cast<HBRUSH>(::GetStockObject(DC_BRUSH)));
		}

		HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, RGB(98, 98, 98));
		HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
		if (hOldPen != nullptr)
		{
			if (captionLength > 0)
			{
				const int topLineLeftEnd = (std::max)(borderLeft, static_cast<int>(rcTitleBackground.left) - titleGap);
				const int topLineRightStart = (std::min)(borderRight, static_cast<int>(rcTitleBackground.right) + titleGap);
				if (topLineLeftEnd > borderLeft)
				{
					::MoveToEx(hdc, borderLeft, borderTop, nullptr);
					::LineTo(hdc, topLineLeftEnd, borderTop);
				}
				if (topLineRightStart < borderRight)
				{
					::MoveToEx(hdc, topLineRightStart, borderTop, nullptr);
					::LineTo(hdc, borderRight, borderTop);
				}
			}
			else
			{
				::MoveToEx(hdc, borderLeft, borderTop, nullptr);
				::LineTo(hdc, borderRight, borderTop);
			}
			::MoveToEx(hdc, borderLeft, borderTop, nullptr);
			::LineTo(hdc, borderLeft, borderBottom);
			::MoveToEx(hdc, borderRight, borderTop, nullptr);
			::LineTo(hdc, borderRight, borderBottom);
			::MoveToEx(hdc, borderLeft, borderBottom, nullptr);
			::LineTo(hdc, borderRight, borderBottom);
		}

		if (captionLength > 0)
		{
			RECT rcCaptionDraw = rcTitleText;
			::DrawTextW(hdc, caption, captionLength, &rcCaptionDraw, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
		}

		if (hOldPen != nullptr)
		{
			::SelectObject(hdc, hOldPen);
		}
		if (hBorderPen != nullptr)
		{
			::DeleteObject(hBorderPen);
		}
		if (hOldFont != nullptr)
		{
			::SelectObject(hdc, hOldFont);
		}
		::ReleaseDC(hFindDlg, hdc);
	}

	void CenterFindDialogToMainWindow(HWND hFindDlg) const
	{
		if (hFindDlg == nullptr || !::IsWindow(hFindDlg) || m_hWnd == nullptr || !::IsWindow(m_hWnd))
		{
			return;
		}

		RECT rcDialog{};
		RECT rcMain{};
		::GetWindowRect(hFindDlg, &rcDialog);
		::GetWindowRect(m_hWnd, &rcMain);

		const int dialogWidth = rcDialog.right - rcDialog.left;
		const int dialogHeight = rcDialog.bottom - rcDialog.top;
		if (dialogWidth <= 0 || dialogHeight <= 0)
		{
			return;
		}

		HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{ sizeof(monitorInfo) };
		if (hMonitor == nullptr || !::GetMonitorInfoW(hMonitor, &monitorInfo))
		{
			return;
		}

		const RECT rcWork = monitorInfo.rcWork;
		int x = rcMain.left + ((rcMain.right - rcMain.left) - dialogWidth) / 2;
		int y = rcMain.top + ((rcMain.bottom - rcMain.top) - dialogHeight) / 2;
		const int workLeft = static_cast<int>(rcWork.left);
		const int workTop = static_cast<int>(rcWork.top);
		const int workRight = static_cast<int>(rcWork.right);
		const int workBottom = static_cast<int>(rcWork.bottom);
		x = (std::max)(workLeft, (std::min)(x, workRight - dialogWidth));
		y = (std::max)(workTop, (std::min)(y, workBottom - dialogHeight));

		::SetWindowPos(
			hFindDlg,
			nullptr,
			x,
			y,
			0,
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	bool HandleFindDialogToggleClick(HWND hFindDlg, HWND hControl) const
	{
		if (!m_isDarkThemeApplied || hFindDlg == nullptr || hControl == nullptr || !::IsWindow(hControl))
		{
			return false;
		}

		const LONG_PTR style = ::GetWindowLongPtrW(hControl, GWL_STYLE);
		if ((style & BS_TYPEMASK) != BS_OWNERDRAW)
		{
			return false;
		}

		const int controlId = ::GetDlgCtrlID(hControl);
		if (IsFindDialogCheckboxControlId(controlId))
		{
			const LRESULT oldState = ::SendMessageW(hControl, BM_GETCHECK, 0, 0);
			::SendMessageW(hControl, BM_SETCHECK, (oldState == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED, 0);
			::InvalidateRect(hControl, nullptr, FALSE);
			return true;
		}

		if (IsFindDialogDirectionRadioControlId(controlId))
		{
			HWND hUp = ::GetDlgItem(hFindDlg, rad1);
			HWND hDown = ::GetDlgItem(hFindDlg, rad2);
			if (hUp != nullptr)
			{
				::SendMessageW(hUp, BM_SETCHECK, (controlId == rad1) ? BST_CHECKED : BST_UNCHECKED, 0);
				::InvalidateRect(hUp, nullptr, FALSE);
			}
			if (hDown != nullptr)
			{
				::SendMessageW(hDown, BM_SETCHECK, (controlId == rad2) ? BST_CHECKED : BST_UNCHECKED, 0);
				::InvalidateRect(hDown, nullptr, FALSE);
			}
			return true;
		}

		return false;
	}

	void ApplyWindowDarkFrame(HWND hWndTarget) const
	{
		using DwmSetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
		if (hWndTarget == nullptr)
		{
			return;
		}

		HMODULE hDwmApi = ::LoadLibraryW(L"dwmapi.dll");
		if (!hDwmApi)
		{
			return;
		}

		const auto dwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
			::GetProcAddress(hDwmApi, "DwmSetWindowAttribute"));
		if (dwmSetWindowAttribute)
		{
			const BOOL useDark = m_isDarkThemeApplied ? TRUE : FALSE;
			constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
			constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
			constexpr DWORD DWMWA_SYSTEMBACKDROP_TYPE = 38;
			constexpr DWORD DWMWA_MICA_EFFECT = 1029;
			constexpr DWORD DWMSBT_NONE = 1;
			dwmSetWindowAttribute(hWndTarget, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
			dwmSetWindowAttribute(hWndTarget, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &useDark, sizeof(useDark));
			const DWORD backdropType = DWMSBT_NONE;
			dwmSetWindowAttribute(hWndTarget, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
			const BOOL micaEnabled = FALSE;
			dwmSetWindowAttribute(hWndTarget, DWMWA_MICA_EFFECT, &micaEnabled, sizeof(micaEnabled));
		}

		::FreeLibrary(hDwmApi);
	}

	void ApplyFindDialogTheme()
	{
		if (m_pCurrentFindReplaceDialog == nullptr || !::IsWindow(m_pCurrentFindReplaceDialog->m_hWnd))
		{
			return;
		}

		const HWND hFindDlg = m_pCurrentFindReplaceDialog->m_hWnd;
		AttachFindDialogSubclass(hFindDlg);
		ApplyDarkModeForWindow(hFindDlg);
		ApplyExplorerTheme(hFindDlg, m_isDarkThemeApplied);

		::EnumChildWindows(
			hFindDlg,
			[](HWND hWndChild, LPARAM lParam) -> BOOL
			{
				const auto* self = reinterpret_cast<const CMainFrame*>(lParam);
				if (self != nullptr)
				{
					wchar_t className[32]{};
					::GetClassNameW(hWndChild, className, static_cast<int>(std::size(className)));
					const bool isButtonClass = ::lstrcmpiW(className, L"Button") == 0;
					const bool isEditClass = ::lstrcmpiW(className, L"Edit") == 0;
					const LONG_PTR style = isButtonClass ? ::GetWindowLongPtrW(hWndChild, GWL_STYLE) : 0;
					const LONG_PTR buttonType = style & BS_TYPEMASK;
					const int controlId = ::GetDlgCtrlID(hWndChild);

					self->ApplyDarkModeForWindow(hWndChild);
					if (self->m_isDarkThemeApplied && isButtonClass && buttonType == BS_GROUPBOX)
					{
						self->ApplyWindowThemeName(hWndChild, L"");
					}
					else if (isButtonClass && (self->IsFindDialogPushButtonType(buttonType) || self->IsFindDialogToggleControlId(controlId)))
					{
						self->ApplyWindowThemeName(hWndChild, self->m_isDarkThemeApplied ? L"" : L"Explorer");
						self->UpdateFindDialogButtonStyles(hWndChild);
					}
					else if (self->m_isDarkThemeApplied && isEditClass)
					{
						self->ApplyWindowThemeName(hWndChild, L"");
						self->NormalizeFindDialogEditStyles(hWndChild);
					}
					else
					{
						self->ApplyExplorerTheme(hWndChild, self->m_isDarkThemeApplied);
					}
					if (self->m_fontUi.m_hFont != nullptr)
					{
						::SendMessageW(hWndChild, WM_SETFONT, reinterpret_cast<WPARAM>(static_cast<HFONT>(self->m_fontUi)), TRUE);
					}
					::SendMessageW(hWndChild, WM_THEMECHANGED, 0, 0);
				}
				return TRUE;
			},
			reinterpret_cast<LPARAM>(this));

		SyncFindDialogGroupBoxVisibility(hFindDlg);
		ApplyWindowDarkFrame(hFindDlg);
		::SendMessageW(hFindDlg, WM_THEMECHANGED, 0, 0);
		::RedrawWindow(hFindDlg, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		DrawFindDialogGroupBoxOverlay(hFindDlg);
	}

	void ApplyPopupMenuTheme() const
	{
		using SetPreferredAppModeFn = int(WINAPI*)(int);
		using FlushMenuThemesFn = void(WINAPI*)();

		HMODULE hUxTheme = ::LoadLibraryW(L"uxtheme.dll");
		if (!hUxTheme)
		{
			return;
		}

		const auto setPreferredAppMode = reinterpret_cast<SetPreferredAppModeFn>(::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135)));
		const auto flushMenuThemes = reinterpret_cast<FlushMenuThemesFn>(::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(136)));
		if (setPreferredAppMode)
		{
			constexpr int AppModeDefault = 0;
			constexpr int AppModeForceDark = 2;
			setPreferredAppMode(m_isDarkThemeApplied ? AppModeForceDark : AppModeDefault);
		}
		if (flushMenuThemes)
		{
			flushMenuThemes();
		}

		::FreeLibrary(hUxTheme);
	}

	void ApplyTheme()
	{
		m_isDarkThemeApplied = ShouldUseDarkTheme();
		if (m_isDarkThemeApplied)
		{
			m_viewTextColor = RGB(231, 231, 231);
			m_viewBackgroundColor = RGB(32, 32, 32);
			m_frameBackgroundColor = RGB(32, 32, 32);
			m_titleBarBackgroundColor = RGB(24, 24, 24);
			m_topBarBackgroundColor = RGB(24, 24, 24);
			m_topBarTextColor = RGB(232, 232, 232);
			m_topBarHoverColor = RGB(57, 57, 57);
			m_topBarPressedColor = RGB(72, 72, 72);
			m_topBarDividerColor = RGB(58, 58, 58);
			m_toolbarBackgroundColor = RGB(24, 24, 24);
			m_toolbarHoverColor = RGB(57, 57, 57);
			m_toolbarPressedColor = RGB(72, 72, 72);
			m_statusBarBackgroundColor = RGB(32, 32, 32);
			m_statusBarDividerColor = RGB(62, 62, 62);
			m_statusBarTextColor = RGB(222, 222, 222);
			m_findPanelBackgroundColor = RGB(48, 48, 50);
			m_findPanelBorderColor = RGB(74, 76, 80);
			m_findPanelButtonHoverColor = RGB(64, 65, 68);
			m_findPanelButtonPressedColor = RGB(74, 76, 80);
			m_findPanelButtonTextColor = RGB(236, 236, 236);
			m_findPanelInputBackgroundColor = RGB(58, 58, 60);
			m_findPanelInputBorderColor = RGB(86, 88, 92);
		}
		else
		{
			m_viewTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
			m_viewBackgroundColor = ::GetSysColor(COLOR_WINDOW);
			m_frameBackgroundColor = RGB(245, 246, 248);
			m_titleBarBackgroundColor = RGB(245, 246, 248);
			m_topBarBackgroundColor = RGB(245, 246, 248);
			m_topBarTextColor = RGB(26, 26, 26);
			m_topBarHoverColor = RGB(231, 233, 238);
			m_topBarPressedColor = RGB(219, 223, 230);
			m_topBarDividerColor = RGB(208, 212, 219);
			m_toolbarBackgroundColor = RGB(245, 246, 248);
			m_toolbarHoverColor = RGB(226, 230, 238);
			m_toolbarPressedColor = RGB(214, 220, 232);
			m_statusBarBackgroundColor = RGB(248, 249, 252);
			m_statusBarDividerColor = RGB(216, 220, 227);
			m_statusBarTextColor = RGB(40, 40, 40);
			m_findPanelBackgroundColor = RGB(244, 246, 250);
			m_findPanelBorderColor = RGB(205, 210, 219);
			m_findPanelButtonHoverColor = RGB(230, 235, 244);
			m_findPanelButtonPressedColor = RGB(220, 227, 239);
			m_findPanelButtonTextColor = RGB(52, 52, 52);
			m_findPanelInputBackgroundColor = RGB(255, 255, 255);
			m_findPanelInputBorderColor = RGB(189, 196, 209);
		}

		if (m_viewBackgroundBrush.m_hBrush)
		{
			m_viewBackgroundBrush.DeleteObject();
		}
		m_viewBackgroundBrush.CreateSolidBrush(m_viewBackgroundColor);

		if (m_frameBackgroundBrush.m_hBrush)
		{
			m_frameBackgroundBrush.DeleteObject();
		}
		m_frameBackgroundBrush.CreateSolidBrush(m_frameBackgroundColor);
		if (m_statusBarBackgroundBrush.m_hBrush)
		{
			m_statusBarBackgroundBrush.DeleteObject();
		}
		m_statusBarBackgroundBrush.CreateSolidBrush(m_statusBarBackgroundColor);
		if (m_findPanelInputBrush.m_hBrush)
		{
			m_findPanelInputBrush.DeleteObject();
		}
		m_findPanelInputBrush.CreateSolidBrush(m_findPanelInputBackgroundColor);

		if (m_hWndStatusBar)
		{
			::SendMessageW(m_hWndStatusBar, SB_SETBKCOLOR, 0, static_cast<LPARAM>(m_statusBarBackgroundColor));
		}

		ApplyExplorerTheme(m_hWnd, m_isDarkThemeApplied);
		ApplyExplorerTheme(m_view.m_hWnd, m_isDarkThemeApplied);
		NormalizeStatusBarStyles();
		ApplyStatusBarTheme();

		ApplyTitleBarTheme();
		ApplyPopupMenuTheme();
		ApplyFindDialogTheme();
		EnsureFindPanelControlsCreated();
		ApplyFindPanelFontsAndTexts();
		UpdateFindPanelVisibility();
		const std::array<HWND, 2> findEdits{
			m_hFindPanelFindEdit,
			m_hFindPanelReplaceEdit
		};
		for (const HWND hWndEdit : findEdits)
		{
			if (hWndEdit != nullptr)
			{
				ApplyDarkModeForWindow(hWndEdit);
				ApplyExplorerTheme(hWndEdit, m_isDarkThemeApplied);
			}
		}
		const std::array<HWND, 7> findButtons{
			m_hFindPanelToggleButton,
			m_hFindPanelFindNextButton,
			m_hFindPanelFindPrevButton,
			m_hFindPanelOptionsButton,
			m_hFindPanelCloseButton,
			m_hFindPanelReplaceButton,
			m_hFindPanelReplaceAllButton
		};
		for (const HWND hWndButton : findButtons)
		{
			if (hWndButton != nullptr)
			{
				ApplyDarkModeForWindow(hWndButton);
				ApplyWindowThemeName(hWndButton, m_isDarkThemeApplied ? L"" : L"Explorer");
			}
		}
		if (m_hFindPanelSearchIcon != nullptr)
		{
			ApplyDarkModeForWindow(m_hFindPanelSearchIcon);
			ApplyWindowThemeName(m_hFindPanelSearchIcon, L"");
		}

		Invalidate(FALSE);
		if (m_view.m_hWnd)
		{
			m_view.Invalidate();
		}
		if (m_hWndStatusBar)
		{
			::InvalidateRect(m_hWndStatusBar, nullptr, FALSE);
		}
		InvalidateTopBar();
	}

	HMENU GetMainMenuHandle() const
	{
		if (m_hMainMenu != nullptr)
		{
			return m_hMainMenu;
		}
		return ::GetMenu(m_hWnd);
	}

	void InvalidateTopBar()
	{
		if (!m_hWnd)
		{
			return;
		}
		RECT rc{};
		::GetClientRect(m_hWnd, &rc);
		rc.bottom = m_nTopBarHeight;
		::InvalidateRect(m_hWnd, &rc, FALSE);
	}

	void EnsureTopBarAnimationTimer()
	{
		if (!m_hWnd || m_topBarAnimationTimerActive)
		{
			return;
		}

		if (::SetTimer(m_hWnd, kTopBarAnimationTimerId, kTopBarAnimationIntervalMs, nullptr) != 0)
		{
			m_topBarAnimationTimerActive = true;
		}
	}

	void StopTopBarAnimationTimer()
	{
		if (!m_hWnd || !m_topBarAnimationTimerActive)
		{
			return;
		}

		::KillTimer(m_hWnd, kTopBarAnimationTimerId);
		m_topBarAnimationTimerActive = false;
	}

	static bool StepBlendTowardsTarget(BYTE& blendValue, const BYTE targetValue)
	{
		if (blendValue == targetValue)
		{
			return false;
		}

		const BYTE step = static_cast<BYTE>((255 * kTopBarAnimationIntervalMs) / (std::max)(static_cast<int>(kTopBarAnimationDurationMs), 1));
		if (blendValue < targetValue)
		{
			const int next = (std::min)(static_cast<int>(targetValue), static_cast<int>(blendValue) + static_cast<int>(step));
			blendValue = static_cast<BYTE>(next);
		}
		else
		{
			const int next = (std::max)(static_cast<int>(targetValue), static_cast<int>(blendValue) - static_cast<int>(step));
			blendValue = static_cast<BYTE>(next);
		}
		return true;
	}

	void UpdateTopBarAnimationState()
	{
		const BYTE targetMenuBlend = (m_topMenuHoveredIndex >= 0 || m_topMenuPressedIndex >= 0) ? 255 : 0;
		const BYTE targetTitleBlend = (m_titleBarHoveredButtonIndex >= 0 || m_titleBarPressedButtonIndex >= 0) ? 255 : 0;

		const bool menuChanged = StepBlendTowardsTarget(m_topMenuHoverBlend, targetMenuBlend);
		const bool titleChanged = StepBlendTowardsTarget(m_titleBarHoverBlend, targetTitleBlend);
		if (menuChanged || titleChanged)
		{
			InvalidateTopBar();
		}

		const bool menuDone = (m_topMenuHoverBlend == targetMenuBlend);
		const bool titleDone = (m_titleBarHoverBlend == targetTitleBlend);
		if (menuDone && titleDone)
		{
			StopTopBarAnimationTimer();
		}
	}

	bool UseCustomTitleBar() const
	{
		return true;
	}

	bool IsPointInTitleBar(const POINT& ptClient) const
	{
		return UseCustomTitleBar() && ptClient.y >= 0 && ptClient.y < m_nTopTitleBarHeight;
	}

	bool IsPointInMenuBar(const POINT& ptClient) const
	{
		if (ptClient.y < 0 || ptClient.y >= m_nTopBarHeight)
		{
			return false;
		}
		for (const RECT& rcButton : m_topMenuButtonRects)
		{
			if (::PtInRect(&rcButton, ptClient))
			{
				return true;
			}
		}
		return false;
	}

	int HitTestTitleBarButton(const POINT& ptClient) const
	{
		if (!IsPointInTitleBar(ptClient))
		{
			return -1;
		}

		for (int index = 0; index < kTitleBarButtonCount; ++index)
		{
			if (!::IsRectEmpty(&m_titleBarButtonRects[static_cast<size_t>(index)]) &&
				::PtInRect(&m_titleBarButtonRects[static_cast<size_t>(index)], ptClient))
			{
				return index;
			}
		}

		return -1;
	}

	HFONT GetUiFontHandle() const
	{
		if (m_fontUi.m_hFont != nullptr)
		{
			return m_fontUi;
		}
		return static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
	}

	HFONT GetToolbarIconFontHandle() const
	{
		if (m_fontToolbarIcons.m_hFont != nullptr)
		{
			return m_fontToolbarIcons;
		}
		return GetUiFontHandle();
	}

	HFONT GetTitleBarIconFontHandle() const
	{
		if (m_fontTitleBarIcons.m_hFont != nullptr)
		{
			return m_fontTitleBarIcons;
		}
		return GetToolbarIconFontHandle();
	}

	std::wstring GetTopBarTitleText() const
	{
		return L"";
	}

	struct FontFaceProbeData
	{
		const wchar_t* targetFace;
		bool found;
	};

	static int CALLBACK EnumFontFaceProbeProc(
		const LOGFONTW* lpelfe,
		const TEXTMETRICW* /*lpntme*/,
		DWORD /*FontType*/,
		LPARAM lParam)
	{
		auto* probe = reinterpret_cast<FontFaceProbeData*>(lParam);
		if (probe == nullptr || lpelfe == nullptr || probe->targetFace == nullptr)
		{
			return 0;
		}

		if (_wcsicmp(lpelfe->lfFaceName, probe->targetFace) == 0)
		{
			probe->found = true;
			return 0;
		}
		return 1;
	}

	static bool IsFontFaceAvailable(const wchar_t* faceName)
	{
		if (faceName == nullptr || faceName[0] == L'\0')
		{
			return false;
		}

		HDC hdcScreen = ::GetDC(nullptr);
		if (hdcScreen == nullptr)
		{
			return false;
		}

		LOGFONTW lf{};
		lf.lfCharSet = DEFAULT_CHARSET;
		wcsncpy_s(lf.lfFaceName, faceName, _TRUNCATE);
		FontFaceProbeData probe{ faceName, false };
		::EnumFontFamiliesExW(hdcScreen, &lf, EnumFontFaceProbeProc, reinterpret_cast<LPARAM>(&probe), 0);
		::ReleaseDC(nullptr, hdcScreen);
		return probe.found;
	}

	static COLORREF BlendColors(const COLORREF base, const COLORREF accent, const BYTE accentAlpha)
	{
		const BYTE baseAlpha = static_cast<BYTE>(255 - accentAlpha);
		const int r = (GetRValue(base) * baseAlpha + GetRValue(accent) * accentAlpha) / 255;
		const int g = (GetGValue(base) * baseAlpha + GetGValue(accent) * accentAlpha) / 255;
		const int b = (GetBValue(base) * baseAlpha + GetBValue(accent) * accentAlpha) / 255;
		return RGB(r, g, b);
	}

	static UINT GetWindowDpiValue(HWND hWnd)
	{
		HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
		if (hUser32 != nullptr)
		{
			using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
			const auto getDpiForWindow = reinterpret_cast<GetDpiForWindowFn>(
				::GetProcAddress(hUser32, "GetDpiForWindow"));
			if (getDpiForWindow != nullptr && hWnd != nullptr)
			{
				return (std::max)(96U, getDpiForWindow(hWnd));
			}
		}

		const SIZE systemDpi = GetSystemDpi();
		return static_cast<UINT>((std::max)(systemDpi.cx, 96L));
	}

	static int GetSystemMetricForDpi(const int index, const UINT dpi)
	{
		HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
		if (hUser32 != nullptr)
		{
			using GetSystemMetricsForDpiFn = int(WINAPI*)(int, UINT);
			const auto getSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiFn>(
				::GetProcAddress(hUser32, "GetSystemMetricsForDpi"));
			if (getSystemMetricsForDpi != nullptr)
			{
				return getSystemMetricsForDpi(index, (std::max)(dpi, 96U));
			}
		}

		return MulDiv(::GetSystemMetrics(index), static_cast<int>((std::max)(dpi, 96U)), 96);
	}

	static int GetCaptionButtonWidthForDpi(const int dpiX)
	{
		return GetSystemMetricForDpi(SM_CXSIZE, static_cast<UINT>((std::max)(dpiX, 96)));
	}

	static int GetResizeBorderThicknessXForDpi(const UINT dpi)
	{
		return GetSystemMetricForDpi(SM_CXFRAME, dpi) + GetSystemMetricForDpi(SM_CXPADDEDBORDER, dpi);
	}

	static int GetResizeBorderThicknessYForDpi(const UINT dpi)
	{
		return GetSystemMetricForDpi(SM_CYFRAME, dpi) + GetSystemMetricForDpi(SM_CXPADDEDBORDER, dpi);
	}

	void GetClientResizeInsetsForLayout(const int /*dpiX*/, const int /*dpiY*/, int& insetX, int& insetBottom) const
	{
		insetX = 0;
		insetBottom = 0;
		if (!UseCustomTitleBar() || !m_hWnd || ::IsZoomed(m_hWnd))
		{
			return;
		}

		const UINT dpiWindow = GetWindowDpiValue(m_hWnd);
		insetX = (std::max)(
			1,
			GetResizeBorderThicknessXForDpi(dpiWindow));
		insetBottom = (std::max)(
			1,
			GetResizeBorderThicknessYForDpi(dpiWindow));
	}

	int HitTestWindowResizeEdges(const POINT& ptClient, const bool includeTopEdge) const
	{
		if (!UseCustomTitleBar() || !m_hWnd || ::IsZoomed(m_hWnd))
		{
			return HTCLIENT;
		}

		const UINT dpi = GetWindowDpiValue(m_hWnd);
		const int resizeBorderX = (std::max)(1, GetResizeBorderThicknessXForDpi(dpi));
		const int resizeBorderY = (std::max)(1, GetResizeBorderThicknessYForDpi(dpi));
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);

		const bool isLeft = ptClient.x < (rcClient.left + resizeBorderX);
		const bool isRight = ptClient.x >= (rcClient.right - resizeBorderX);
		const bool isTop = includeTopEdge && ptClient.y < (rcClient.top + resizeBorderY);
		const bool isBottom = ptClient.y >= (rcClient.bottom - resizeBorderY);

		if (isTop && isLeft)
		{
			return HTTOPLEFT;
		}
		if (isTop && isRight)
		{
			return HTTOPRIGHT;
		}
		if (isBottom && isLeft)
		{
			return HTBOTTOMLEFT;
		}
		if (isBottom && isRight)
		{
			return HTBOTTOMRIGHT;
		}
		if (isTop)
		{
			return HTTOP;
		}
		if (isLeft)
		{
			return HTLEFT;
		}
		if (isRight)
		{
			return HTRIGHT;
		}
		if (isBottom)
		{
			return HTBOTTOM;
		}

		return HTCLIENT;
	}

	int GetCaptionButtonsReservedWidth(const int dpiX) const
	{
		return GetCaptionButtonWidthForDpi(dpiX) * kTitleBarButtonCount;
	}

	void UpdateWindowFrameStyle() const
	{
		if (!m_hWnd || !UseCustomTitleBar())
		{
			return;
		}

		const LONG_PTR currentStyle = ::GetWindowLongPtrW(m_hWnd, GWL_STYLE);
		const LONG_PTR targetStyle =
			(currentStyle | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) &
			~static_cast<LONG_PTR>(WS_CAPTION);
		if (targetStyle != currentStyle)
		{
			::SetWindowLongPtrW(m_hWnd, GWL_STYLE, targetStyle);
		}
	}

	void UpdateCustomChromeFrame() const
	{
		using DwmExtendFrameIntoClientAreaFn = HRESULT(WINAPI*)(HWND, const void*);
		if (!m_hWnd || !UseCustomTitleBar())
		{
			return;
		}

		HMODULE hDwmApi = ::LoadLibraryW(L"dwmapi.dll");
		if (hDwmApi == nullptr)
		{
			return;
		}

		const auto dwmExtendFrameIntoClientArea = reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
			::GetProcAddress(hDwmApi, "DwmExtendFrameIntoClientArea"));
		if (dwmExtendFrameIntoClientArea != nullptr)
		{
			struct DwmMargins
			{
				int cxLeftWidth;
				int cxRightWidth;
				int cyTopHeight;
				int cyBottomHeight;
			};

			const DwmMargins margins{
				0,
				0,
				0,
				0
			};
			dwmExtendFrameIntoClientArea(m_hWnd, &margins);
		}

		::FreeLibrary(hDwmApi);
	}

	void UpdateRoundedWindowRegion() const
	{
		if (!m_hWnd || !UseCustomTitleBar())
		{
			return;
		}

		if (::IsZoomed(m_hWnd) || ::IsIconic(m_hWnd))
		{
			::SetWindowRgn(m_hWnd, nullptr, TRUE);
			return;
		}

		RECT rcWindow{};
		if (!::GetWindowRect(m_hWnd, &rcWindow))
		{
			return;
		}

		const int windowWidth = rcWindow.right - rcWindow.left;
		const int windowHeight = rcWindow.bottom - rcWindow.top;
		if (windowWidth <= 0 || windowHeight <= 0)
		{
			return;
		}

		const UINT dpi = GetWindowDpiValue(m_hWnd);
		const int cornerRadius = (std::max)(MulDiv(12, static_cast<int>(dpi), 96), 8);
		HRGN hRoundedRegion = ::CreateRoundRectRgn(0, 0, windowWidth + 1, windowHeight + 1, cornerRadius, cornerRadius);
		if (hRoundedRegion == nullptr)
		{
			return;
		}

		if (!::SetWindowRgn(m_hWnd, hRoundedRegion, TRUE))
		{
			::DeleteObject(hRoundedRegion);
		}
	}

	static LPCWSTR GetUiStateRegistryPath()
	{
		return L"Software\\Steganos\\LockNote2";
	}

	bool LoadWindowSizeFromRegistry()
	{
		CRegKey key;
		if (key.Open(HKEY_CURRENT_USER, GetUiStateRegistryPath(), KEY_READ) != ERROR_SUCCESS)
		{
			return false;
		}

		DWORD width = 0;
		DWORD height = 0;
		if (key.QueryDWORDValue(L"WindowWidth", width) != ERROR_SUCCESS ||
			key.QueryDWORDValue(L"WindowHeight", height) != ERROR_SUCCESS)
		{
			return false;
		}

		if (width < static_cast<DWORD>(kStoredWindowMinWidth) ||
			height < static_cast<DWORD>(kStoredWindowMinHeight))
		{
			return false;
		}

		m_nWindowSizeX = static_cast<int>(width);
		m_nWindowSizeY = static_cast<int>(height);
		return true;
	}

	void SaveWindowSizeToRegistry() const
	{
		if (m_nWindowSizeX < kStoredWindowMinWidth || m_nWindowSizeY < kStoredWindowMinHeight)
		{
			return;
		}

		CRegKey key;
		if (key.Create(HKEY_CURRENT_USER, GetUiStateRegistryPath()) != ERROR_SUCCESS)
		{
			return;
		}

		key.SetDWORDValue(L"WindowWidth", static_cast<DWORD>(m_nWindowSizeX));
		key.SetDWORDValue(L"WindowHeight", static_cast<DWORD>(m_nWindowSizeY));
	}

	static SIZE GetSystemDpi()
	{
		SIZE dpi{ 96, 96 };
		HDC hdcScreen = ::GetDC(nullptr);
		if (hdcScreen != nullptr)
		{
			dpi.cx = (std::max)(96, GetDeviceCaps(hdcScreen, LOGPIXELSX));
			dpi.cy = (std::max)(96, GetDeviceCaps(hdcScreen, LOGPIXELSY));
			::ReleaseDC(nullptr, hdcScreen);
		}
		return dpi;
	}

	static RECT GetPreferredWorkAreaRect()
	{
		POINT anchorPoint{};
		if (!::GetCursorPos(&anchorPoint))
		{
			anchorPoint = POINT{ 0, 0 };
		}

		HMONITOR hMonitor = ::MonitorFromPoint(anchorPoint, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{ sizeof(monitorInfo) };
		if (hMonitor != nullptr && ::GetMonitorInfoW(hMonitor, &monitorInfo))
		{
			return monitorInfo.rcWork;
		}

		return RECT{
			0,
			0,
			::GetSystemMetrics(SM_CXSCREEN),
			::GetSystemMetrics(SM_CYSCREEN)
		};
	}

	void ClampWindowSizeToWorkArea(const RECT& rcWork)
	{
		const int workWidth = (std::max)(1, static_cast<int>(rcWork.right - rcWork.left));
		const int workHeight = (std::max)(1, static_cast<int>(rcWork.bottom - rcWork.top));
		const int systemMinWidth = ::GetSystemMetrics(SM_CXMINTRACK);
		const int systemMinHeight = ::GetSystemMetrics(SM_CYMINTRACK);
		const int minWidth = (std::min)((std::max)(kStoredWindowMinWidth, systemMinWidth), workWidth);
		const int minHeight = (std::min)((std::max)(kStoredWindowMinHeight, systemMinHeight), workHeight);

		m_nWindowSizeX = (std::clamp)(m_nWindowSizeX, minWidth, workWidth);
		m_nWindowSizeY = (std::clamp)(m_nWindowSizeY, minHeight, workHeight);
	}

	void PrepareInitialWindowSizeForCreate()
	{
		if (m_hasPreparedInitialWindowSize)
		{
			return;
		}

		m_hasLoadedPersistedWindowSize = LoadWindowSizeFromRegistry();
		if (!m_hasLoadedPersistedWindowSize &&
			m_nWindowSizeX == DEFAULT_WIDTH &&
			m_nWindowSizeY == DEFAULT_HEIGHT)
		{
			const SIZE dpi = GetSystemDpi();
			m_nWindowSizeX = MulDiv(m_nWindowSizeX, dpi.cx, 96);
			m_nWindowSizeY = MulDiv(m_nWindowSizeY, dpi.cy, 96);
		}

		ClampWindowSizeToWorkArea(GetPreferredWorkAreaRect());
		m_hasPreparedInitialWindowSize = true;
	}

	void ClampWindowSizeToCurrentWorkArea()
	{
		if (!m_hWnd)
		{
			return;
		}

		HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{ sizeof(monitorInfo) };
		if (hMonitor == nullptr || !::GetMonitorInfoW(hMonitor, &monitorInfo))
		{
			return;
		}

		ClampWindowSizeToWorkArea(monitorInfo.rcWork);
	}

	void CaptureCurrentWindowSizeIfRestored()
	{
		if (!m_hWnd || !::IsWindow(m_hWnd))
		{
			return;
		}

		WINDOWPLACEMENT placement{};
		placement.length = sizeof(placement);
		if (!::GetWindowPlacement(m_hWnd, &placement))
		{
			return;
		}
		if (placement.showCmd != SW_SHOWNORMAL)
		{
			return;
		}

		RECT rc{};
		if (!::GetWindowRect(m_hWnd, &rc))
		{
			return;
		}
		const int width = static_cast<int>(rc.right - rc.left);
		const int height = static_cast<int>(rc.bottom - rc.top);
		if (width >= kStoredWindowMinWidth && height >= kStoredWindowMinHeight)
		{
			m_nWindowSizeX = width;
			m_nWindowSizeY = height;
		}
	}

	void RefreshTopMenuLayout()
	{
		m_topMenuPopupPositions.clear();
		m_topMenuButtonRects.clear();
		m_topMenuCaptions.clear();
		m_topMenuOverflowPopupPositions.clear();
		m_topMenuOverflowCaptions.clear();
		for (RECT& rcButton : m_titleBarButtonRects)
		{
			::SetRectEmpty(&rcButton);
		}
		m_titleBarHoveredButtonIndex = -1;
		m_titleBarPressedButtonIndex = -1;
		m_topMenuHoveredIndex = -1;
		m_topMenuPressedIndex = -1;
		m_topMenuHoverBlend = 0;
		m_titleBarHoverBlend = 0;
		StopTopBarAnimationTimer();
		::SetRectEmpty(&m_topBarTitleBandRect);
		::SetRectEmpty(&m_topBarIconRect);
		::SetRectEmpty(&m_topBarTitleRect);
		::SetRectEmpty(&m_topBarTabCloseRect);
		::SetRectEmpty(&m_topBarAddButtonRect);
		m_topBarTitleText.clear();

		const HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr || !m_hWnd)
		{
			return;
		}
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);

		CClientDC dc(*this);
		const int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
		if (UseCustomTitleBar())
		{
			const int captionButtonWidth = GetCaptionButtonWidthForDpi(dpiX);
			int buttonRight = static_cast<int>(rcClient.right);
			for (int buttonIndex = kTitleBarButtonCount - 1; buttonIndex >= 0; --buttonIndex)
			{
				const int buttonLeft = buttonRight - captionButtonWidth;
				m_titleBarButtonRects[static_cast<size_t>(buttonIndex)] = RECT{
					buttonLeft,
					0,
					buttonRight,
					m_nTopTitleBarHeight
				};
				buttonRight = buttonLeft;
			}

			// Tab UI is intentionally hidden: custom frame keeps only caption buttons + menu row.
		}
		const int menuTop = 0;
		const int menuBottom = UseCustomTitleBar() ? m_nTopTitleBarHeight : m_nTopMenuHeight;
		const int leftPadding = MulDiv(16, dpiX, 96);
		const int rightPadding = MulDiv(12, dpiX, 96);
		const int itemPaddingX = MulDiv(12, dpiX, 96);
		const int itemSpacing = MulDiv(8, dpiX, 96);
		const int topInset = menuTop + MulDiv(8, dpiX, 96);
		const int bottomInset = MulDiv(8, dpiX, 96);

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(dc, GetUiFontHandle()));

		struct TopMenuEntry
		{
			int position;
			std::wstring caption;
		};
		std::vector<TopMenuEntry> entries;

		const auto getPrimaryTopCaption = [&](const int pos) -> std::wstring
		{
			if (IsRussianUi())
			{
				switch (pos)
				{
				case 0:
					return L"\u0424\u0430\u0439\u043B"; // Файл
				case 1:
					return L"\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C"; // Изменить
				case 2:
					return L"\u041F\u0440\u043E\u0441\u043C\u043E\u0442\u0440"; // Просмотр
				case 3:
					return L"\u0421\u043F\u0440\u0430\u0432\u043A\u0430"; // Справка
				default:
					break;
				}
			}

			const int menuTextLength = ::GetMenuStringW(hMenu, pos, nullptr, 0, MF_BYPOSITION);
			if (menuTextLength <= 0)
			{
				return L"";
			}
			std::wstring rawCaption;
			rawCaption.resize(static_cast<size_t>(menuTextLength) + 1);
			::GetMenuStringW(hMenu, pos, rawCaption.data(), menuTextLength + 1, MF_BYPOSITION);
			rawCaption.resize(static_cast<size_t>(menuTextLength));
			return BuildToolbarCaption(rawCaption);
		};

		const int itemCount = ::GetMenuItemCount(hMenu);
		for (int pos = 0; pos < itemCount; ++pos)
		{
			HMENU hSubMenu = ::GetSubMenu(hMenu, pos);
			if (hSubMenu == nullptr)
			{
				continue;
			}

			const bool hasEncryptionItems =
				::GetMenuState(hSubMenu, ID_STEGANOS_PASSWORD_MANAGER, MF_BYCOMMAND) != static_cast<UINT>(-1) ||
				::GetMenuState(hSubMenu, ID_STEGANOS_SAFE, MF_BYCOMMAND) != static_cast<UINT>(-1);

			std::wstring caption;
			if (hasEncryptionItems)
			{
				caption = GetEncryptionMenuCaption();
			}
			else
			{
				// Keep the primary Notepad-like menu row: File/Edit/View/Help plus Encryption.
				if (pos > 3)
				{
					continue;
				}
				caption = getPrimaryTopCaption(pos);
			}

			if (caption.empty())
			{
				continue;
			}

			entries.push_back(TopMenuEntry{ pos, caption });
		}

		struct MeasuredMenuEntry
		{
			int position;
			std::wstring caption;
			int width;
		};

		std::vector<MeasuredMenuEntry> measuredEntries;
		measuredEntries.reserve(entries.size());
		for (const TopMenuEntry& entry : entries)
		{
			const std::wstring& caption = entry.caption;
			SIZE textSize{};
			::GetTextExtentPoint32W(dc, caption.c_str(), static_cast<int>(caption.size()), &textSize);
			const int textWidth = static_cast<int>(textSize.cx);
			const int buttonWidth = (std::max)(textWidth + (itemPaddingX * 2), MulDiv(36, dpiX, 96));
			measuredEntries.push_back(MeasuredMenuEntry{ entry.position, caption, buttonWidth });
		}

		const std::wstring overflowCaption = L">>";
		SIZE overflowSize{};
		::GetTextExtentPoint32W(dc, overflowCaption.c_str(), static_cast<int>(overflowCaption.size()), &overflowSize);
		const int overflowTextWidth = static_cast<int>(overflowSize.cx);
		const int overflowWidth = (std::max)(overflowTextWidth + (itemPaddingX * 2), MulDiv(36, dpiX, 96));

		int menuRight = static_cast<int>(rcClient.right) - rightPadding;
		if (UseCustomTitleBar())
		{
			menuRight -= GetCaptionButtonsReservedWidth(dpiX) + MulDiv(8, dpiX, 96);
		}
		menuRight = (std::max)(menuRight, leftPadding);

		const int availableWidth = menuRight - leftPadding;
		if (availableWidth > 0 && !measuredEntries.empty())
		{
			int fullWidth = 0;
			for (size_t i = 0; i < measuredEntries.size(); ++i)
			{
				if (i > 0)
				{
					fullWidth += itemSpacing;
				}
				fullWidth += measuredEntries[i].width;
			}

			int visibleCount = static_cast<int>(measuredEntries.size());
			bool useOverflow = false;
			if (fullWidth > availableWidth)
			{
				visibleCount = 0;
				for (size_t i = 0; i < measuredEntries.size(); ++i)
				{
					int candidateWidth = 0;
					for (size_t j = 0; j <= i; ++j)
					{
						if (j > 0)
						{
							candidateWidth += itemSpacing;
						}
						candidateWidth += measuredEntries[j].width;
					}
					const bool hasHiddenItems = (i + 1) < measuredEntries.size();
					if (hasHiddenItems)
					{
						if (candidateWidth > 0)
						{
							candidateWidth += itemSpacing;
						}
						candidateWidth += overflowWidth;
					}
					if (candidateWidth > availableWidth)
					{
						break;
					}
					visibleCount = static_cast<int>(i + 1);
				}
				useOverflow = visibleCount < static_cast<int>(measuredEntries.size());
				if (visibleCount == 0 && overflowWidth > availableWidth)
				{
					useOverflow = false;
				}
			}

			int x = leftPadding;
			for (int i = 0; i < visibleCount; ++i)
			{
				const auto& measuredEntry = measuredEntries[static_cast<size_t>(i)];
				RECT buttonRect{
					x,
					topInset,
					x + measuredEntry.width,
					menuBottom - bottomInset
				};
				m_topMenuPopupPositions.push_back(measuredEntry.position);
				m_topMenuButtonRects.push_back(buttonRect);
				m_topMenuCaptions.push_back(measuredEntry.caption);
				x = buttonRect.right + itemSpacing;
			}

			if (useOverflow)
			{
				RECT overflowRect{
					x,
					topInset,
					x + overflowWidth,
					menuBottom - bottomInset
				};
				m_topMenuPopupPositions.push_back(kTopMenuOverflowMarker);
				m_topMenuButtonRects.push_back(overflowRect);
				m_topMenuCaptions.push_back(overflowCaption);

				for (size_t i = static_cast<size_t>(visibleCount); i < measuredEntries.size(); ++i)
				{
					m_topMenuOverflowPopupPositions.push_back(measuredEntries[i].position);
					m_topMenuOverflowCaptions.push_back(measuredEntries[i].caption);
				}
			}
		}

		::SelectObject(dc, hOldFont);
		RefreshToolbarLayout();
	}

	static std::wstring BuildToolbarCaption(const std::wstring& sourceText)
	{
		std::wstring caption = sourceText;
		caption.erase(std::remove(caption.begin(), caption.end(), L'&'), caption.end());
		const size_t tabPos = caption.find(L'\t');
		if (tabPos != std::wstring::npos)
		{
			caption = caption.substr(0, tabPos);
		}
		while (!caption.empty() && std::iswspace(caption.back()))
		{
			caption.pop_back();
		}
		return caption;
	}

	void RefreshToolbarLayout()
	{
		m_toolbarCommandIds.clear();
		m_toolbarButtonRects.clear();
		m_toolbarButtonCaptions.clear();
		m_toolbarHoveredIndex = -1;
		m_toolbarPressedIndex = -1;
	}

	bool IsFindPanelControl(const HWND hWndChild) const
	{
		return
			hWndChild == m_hFindPanelToggleButton ||
			hWndChild == m_hFindPanelFindEdit ||
			hWndChild == m_hFindPanelFindNextButton ||
			hWndChild == m_hFindPanelFindPrevButton ||
			hWndChild == m_hFindPanelOptionsButton ||
			hWndChild == m_hFindPanelCloseButton ||
			hWndChild == m_hFindPanelReplaceEdit ||
			hWndChild == m_hFindPanelReplaceButton ||
			hWndChild == m_hFindPanelReplaceAllButton ||
			hWndChild == m_hFindPanelSearchIcon;
	}

	static LRESULT CALLBACK FindPanelButtonSubclassProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		UINT_PTR /*uIdSubclass*/,
		DWORD_PTR dwRefData)
	{
		auto* self = reinterpret_cast<CMainFrame*>(dwRefData);
		if (self != nullptr)
		{
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
			{
				self->UpdateHotFindPanelButton(hWnd);
				TRACKMOUSEEVENT tme{ sizeof(tme) };
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				::TrackMouseEvent(&tme);
				break;
			}
			case WM_MOUSELEAVE:
				if (self->m_hHotFindPanelButton == hWnd)
				{
					self->UpdateHotFindPanelButton(nullptr);
				}
				break;
			case WM_NCDESTROY:
				if (self->m_hHotFindPanelButton == hWnd)
				{
					self->m_hHotFindPanelButton = nullptr;
				}
				::RemoveWindowSubclass(hWnd, FindPanelButtonSubclassProc, 0);
				break;
			default:
				break;
			}
		}

		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	void UpdateHotFindPanelButton(HWND hWndHot)
	{
		if (hWndHot != nullptr && !::IsWindow(hWndHot))
		{
			hWndHot = nullptr;
		}

		if (m_hHotFindPanelButton == hWndHot)
		{
			return;
		}

		const HWND hOldHot = m_hHotFindPanelButton;
		m_hHotFindPanelButton = hWndHot;
		if (hOldHot != nullptr && ::IsWindow(hOldHot))
		{
			::InvalidateRect(hOldHot, nullptr, TRUE);
		}
		if (m_hHotFindPanelButton != nullptr && ::IsWindow(m_hHotFindPanelButton))
		{
			::InvalidateRect(m_hHotFindPanelButton, nullptr, TRUE);
		}
	}

	void SubclassFindPanelButton(HWND hWndButton)
	{
		if (hWndButton == nullptr || !::IsWindow(hWndButton))
		{
			return;
		}

		::SetWindowSubclass(
			hWndButton,
			FindPanelButtonSubclassProc,
			0,
			reinterpret_cast<DWORD_PTR>(this));
	}

	void AttachChromeResizeSubclassesForFindPanelControls()
	{
		const std::array<HWND, 10> findControls{
			m_hFindPanelToggleButton,
			m_hFindPanelFindEdit,
			m_hFindPanelFindNextButton,
			m_hFindPanelFindPrevButton,
			m_hFindPanelOptionsButton,
			m_hFindPanelCloseButton,
			m_hFindPanelReplaceEdit,
			m_hFindPanelReplaceButton,
			m_hFindPanelReplaceAllButton,
			m_hFindPanelSearchIcon
		};
		for (const HWND hWndControl : findControls)
		{
			AttachChromeResizeSubclass(hWndControl);
		}
	}

	void DetachChromeResizeSubclassesForFindPanelControls() const
	{
		const std::array<HWND, 10> findControls{
			m_hFindPanelToggleButton,
			m_hFindPanelFindEdit,
			m_hFindPanelFindNextButton,
			m_hFindPanelFindPrevButton,
			m_hFindPanelOptionsButton,
			m_hFindPanelCloseButton,
			m_hFindPanelReplaceEdit,
			m_hFindPanelReplaceButton,
			m_hFindPanelReplaceAllButton,
			m_hFindPanelSearchIcon
		};
		for (const HWND hWndControl : findControls)
		{
			DetachChromeResizeSubclass(hWndControl);
		}
	}

	void EnsureFindPanelControlsCreated()
	{
		if (!m_hWnd || m_hFindPanelFindEdit != nullptr)
		{
			return;
		}

		const DWORD iconButtonStyle = WS_CHILD | WS_TABSTOP | BS_OWNERDRAW;
		const DWORD textButtonStyle = WS_CHILD | WS_TABSTOP | BS_OWNERDRAW;
		const DWORD editStyle = WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL;

		m_hFindPanelToggleButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			iconButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelToggleButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelFindEdit = ::CreateWindowExW(
			0,
			L"Edit",
			L"",
			editStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelFindEditId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelFindNextButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			iconButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelFindNextButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelFindPrevButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			iconButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelFindPrevButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelOptionsButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			iconButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelOptionsButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelCloseButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			iconButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelCloseButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelReplaceEdit = ::CreateWindowExW(
			0,
			L"Edit",
			L"",
			editStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelReplaceEditId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelReplaceButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			textButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelReplaceButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelReplaceAllButton = ::CreateWindowExW(
			0,
			L"Button",
			L"",
			textButtonStyle,
			0,
			0,
			0,
			0,
			m_hWnd,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kFindPanelReplaceAllButtonId)),
			_Module.GetModuleInstance(),
			nullptr);
		m_hFindPanelSearchIcon = ::CreateWindowExW(
			0,
			L"Static",
			L"",
			WS_CHILD | SS_LEFT | SS_CENTERIMAGE,
			0,
			0,
			0,
			0,
			m_hWnd,
			nullptr,
			_Module.GetModuleInstance(),
			nullptr);

		const std::array<HWND, 7> findButtons{
			m_hFindPanelToggleButton,
			m_hFindPanelFindNextButton,
			m_hFindPanelFindPrevButton,
			m_hFindPanelOptionsButton,
			m_hFindPanelCloseButton,
			m_hFindPanelReplaceButton,
			m_hFindPanelReplaceAllButton
		};
		for (const HWND hWndButton : findButtons)
		{
			SubclassFindPanelButton(hWndButton);
		}
		AttachChromeResizeSubclassesForFindPanelControls();
	}

	void ApplyFindPanelFontsAndTexts()
	{
		EnsureFindPanelControlsCreated();
		const HFONT uiFont = (m_fontFindPanelText.m_hFont != nullptr)
			? static_cast<HFONT>(m_fontFindPanelText)
			: GetUiFontHandle();
		const HFONT iconFont = (m_fontFindPanelIcons.m_hFont != nullptr)
			? static_cast<HFONT>(m_fontFindPanelIcons)
			: GetToolbarIconFontHandle();

		const std::array<HWND, 4> textControls{
			m_hFindPanelFindEdit,
			m_hFindPanelReplaceEdit,
			m_hFindPanelReplaceButton,
			m_hFindPanelReplaceAllButton
		};
		for (const HWND hWndControl : textControls)
		{
			if (hWndControl != nullptr && uiFont != nullptr)
			{
				::SendMessageW(hWndControl, WM_SETFONT, reinterpret_cast<WPARAM>(uiFont), TRUE);
			}
		}

		const std::array<HWND, 6> iconControls{
			m_hFindPanelToggleButton,
			m_hFindPanelFindNextButton,
			m_hFindPanelFindPrevButton,
			m_hFindPanelOptionsButton,
			m_hFindPanelCloseButton,
			m_hFindPanelSearchIcon
		};
		for (const HWND hWndControl : iconControls)
		{
			if (hWndControl != nullptr && iconFont != nullptr)
			{
				::SendMessageW(hWndControl, WM_SETFONT, reinterpret_cast<WPARAM>(iconFont), TRUE);
			}
		}

		if (m_hFindPanelReplaceButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelReplaceButton, GetFindPanelReplaceCaption().c_str());
		}
		if (m_hFindPanelReplaceAllButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelReplaceAllButton, GetFindPanelReplaceAllCaption().c_str());
		}

		if (m_hFindPanelFindEdit != nullptr)
		{
			const std::wstring cue = GetFindPanelFindCueText();
			::SendMessageW(m_hFindPanelFindEdit, EM_SETCUEBANNER, 0, reinterpret_cast<LPARAM>(cue.c_str()));
			::SendMessageW(m_hFindPanelFindEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(12, 30));
		}
		if (m_hFindPanelReplaceEdit != nullptr)
		{
			const std::wstring cue = GetFindPanelReplaceCueText();
			::SendMessageW(m_hFindPanelReplaceEdit, EM_SETCUEBANNER, 0, reinterpret_cast<LPARAM>(cue.c_str()));
			::SendMessageW(m_hFindPanelReplaceEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(12, 8));
		}

		if (m_hFindPanelToggleButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelToggleButton, m_isReplacePanelVisible ? L"\uE70D" : L"\uE70E");
		}
		if (m_hFindPanelFindNextButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelFindNextButton, L"\uE74B");
		}
		if (m_hFindPanelFindPrevButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelFindPrevButton, L"\uE74A");
		}
		if (m_hFindPanelOptionsButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelOptionsButton, L"\uE9E9");
		}
		if (m_hFindPanelCloseButton != nullptr)
		{
			::SetWindowTextW(m_hFindPanelCloseButton, L"\uE711");
		}
		if (m_hFindPanelSearchIcon != nullptr)
		{
			::SetWindowTextW(m_hFindPanelSearchIcon, L"\uE721");
		}
	}

	void UpdateFindPanelButtonsEnabledState()
	{
		if (!m_hFindPanelFindEdit)
		{
			return;
		}

		const bool hasSearchText = ::GetWindowTextLengthW(m_hFindPanelFindEdit) > 0;
		if (m_hFindPanelFindNextButton != nullptr)
		{
			::EnableWindow(m_hFindPanelFindNextButton, hasSearchText ? TRUE : FALSE);
		}
		if (m_hFindPanelFindPrevButton != nullptr)
		{
			::EnableWindow(m_hFindPanelFindPrevButton, hasSearchText ? TRUE : FALSE);
		}
		if (m_hFindPanelReplaceButton != nullptr)
		{
			::EnableWindow(m_hFindPanelReplaceButton, hasSearchText ? TRUE : FALSE);
		}
		if (m_hFindPanelReplaceAllButton != nullptr)
		{
			::EnableWindow(m_hFindPanelReplaceAllButton, hasSearchText ? TRUE : FALSE);
		}
	}

	void UpdateFindPanelVisibility()
	{
		EnsureFindPanelControlsCreated();
		const auto setVisible = [](HWND hWndControl, const bool visible)
		{
			if (hWndControl != nullptr)
			{
				::ShowWindow(hWndControl, visible ? SW_SHOWNA : SW_HIDE);
			}
		};

		const bool showFindRow = m_isFindPanelVisible;
		setVisible(m_hFindPanelToggleButton, showFindRow);
		setVisible(m_hFindPanelFindEdit, showFindRow);
		setVisible(m_hFindPanelFindNextButton, showFindRow);
		setVisible(m_hFindPanelFindPrevButton, showFindRow);
		setVisible(m_hFindPanelOptionsButton, showFindRow);
		setVisible(m_hFindPanelCloseButton, showFindRow);
		setVisible(m_hFindPanelSearchIcon, false);

		const bool showReplaceRow = showFindRow && m_isReplacePanelVisible;
		setVisible(m_hFindPanelReplaceEdit, showReplaceRow);
		setVisible(m_hFindPanelReplaceButton, showReplaceRow);
		setVisible(m_hFindPanelReplaceAllButton, showReplaceRow);

		if (m_hHotFindPanelButton != nullptr && !::IsWindowVisible(m_hHotFindPanelButton))
		{
			UpdateHotFindPanelButton(nullptr);
		}

		ApplyFindPanelFontsAndTexts();
		UpdateFindPanelButtonsEnabledState();
	}

	int GetFindPanelAreaHeightForDpi(const int dpiY) const
	{
		if (!m_isFindPanelVisible)
		{
			return 0;
		}

		const int topPadding = MulDiv(13, dpiY, 96);
		const int bottomPadding = MulDiv(13, dpiY, 96);
		const int rowHeight = MulDiv(32, dpiY, 96);
		const int rowSpacing = MulDiv(8, dpiY, 96);
		return topPadding + rowHeight + (m_isReplacePanelVisible ? rowSpacing + rowHeight : 0) + bottomPadding;
	}

	void LayoutFindPanelControls(const RECT& rcClient, const int dpiX, const int dpiY)
	{
		const RECT previousPanelRect = m_findPanelRect;
		RECT previousAreaRect{};
		if (!::IsRectEmpty(&previousPanelRect))
		{
			const int previousAreaBottom = previousPanelRect.bottom + MulDiv(4, dpiY, 96);
			previousAreaRect = RECT{
				rcClient.left,
				m_nTopBarHeight,
				rcClient.right,
				(std::max)(m_nTopBarHeight, previousAreaBottom)
			};
		}

		UpdateFindPanelVisibility();
		if (!m_isFindPanelVisible)
		{
			::SetRectEmpty(&m_findPanelRect);
			::SetRectEmpty(&m_findPanelFindEditFrameRect);
			::SetRectEmpty(&m_findPanelReplaceEditFrameRect);
			::SetRectEmpty(&m_findPanelSearchIconRect);
			if (!::IsRectEmpty(&previousAreaRect))
			{
				::InvalidateRect(m_hWnd, &previousAreaRect, TRUE);
			}
			return;
		}

		const int clientWidth = rcClient.right - rcClient.left;
		const int areaTop = m_nTopBarHeight;
		const int areaHeight = GetFindPanelAreaHeightForDpi(dpiY);
		const int panelTop = areaTop + MulDiv(8, dpiY, 96);
		const int panelBottom = areaTop + areaHeight - MulDiv(4, dpiY, 96);

		const int leftOuterMargin = MulDiv(16, dpiX, 96);
		const int rightOuterMargin = MulDiv(16, dpiX, 96);
		int panelWidth = clientWidth - leftOuterMargin - rightOuterMargin;
		const int desiredWidth = MulDiv(860, dpiX, 96);
		panelWidth = (std::min)(panelWidth, desiredWidth);
		panelWidth = (std::min)(panelWidth, MulDiv(clientWidth, 68, 100));
		panelWidth = (std::max)(panelWidth, MulDiv(520, dpiX, 96));
		panelWidth = (std::min)(panelWidth, (std::max)(1, clientWidth - MulDiv(8, dpiX, 96)));
		int panelLeft = (clientWidth - panelWidth) / 2;
		panelLeft = (std::max)(0, panelLeft);
		const int panelRight = panelLeft + panelWidth;

		m_findPanelRect.left = panelLeft;
		m_findPanelRect.top = panelTop;
		m_findPanelRect.right = panelRight;
		m_findPanelRect.bottom = panelBottom;

		const int innerPadding = MulDiv(10, dpiX, 96);
		const int rowHeight = MulDiv(32, dpiY, 96);
		const int rowSpacing = MulDiv(8, dpiY, 96);
		const int rowTop = panelTop + MulDiv(7, dpiY, 96);
		const int iconButtonWidth = rowHeight;
		const int iconGap = MulDiv(4, dpiX, 96);
		const int editToIconsGap = MulDiv(10, dpiX, 96);

		const int rightClusterWidth = (iconButtonWidth * 4) + (iconGap * 3);
		const int rightClusterLeft = panelRight - innerPadding - rightClusterWidth;
		const int toggleLeft = panelLeft + innerPadding;
		const int toggleRight = toggleLeft + iconButtonWidth;
		const int findEditLeft = toggleRight + iconGap;
		const int findEditRight = rightClusterLeft - editToIconsGap;
		const int minEditWidth = MulDiv(170, dpiX, 96);
		const int safeFindEditRight = (std::max)(findEditLeft + minEditWidth, findEditRight);

		auto moveControl = [](HWND hWndControl, int left, int top, int width, int height)
		{
			if (hWndControl != nullptr)
			{
				::SetWindowPos(
					hWndControl,
					nullptr,
					left,
					top,
					(std::max)(1, width),
					(std::max)(1, height),
					SWP_NOZORDER | SWP_NOACTIVATE);
				::InvalidateRect(hWndControl, nullptr, TRUE);
			}
		};

		moveControl(m_hFindPanelToggleButton, toggleLeft, rowTop, iconButtonWidth, rowHeight);
		m_findPanelFindEditFrameRect = RECT{ findEditLeft, rowTop, safeFindEditRight, rowTop + rowHeight };
		const int editInsetX = (std::max)(1, MulDiv(2, dpiX, 96));
		const int editInsetTop = (std::max)(1, MulDiv(3, dpiY, 96));
		const int editInsetBottom = (std::max)(1, MulDiv(2, dpiY, 96));
		moveControl(
			m_hFindPanelFindEdit,
			m_findPanelFindEditFrameRect.left + editInsetX,
			m_findPanelFindEditFrameRect.top + editInsetTop,
			(m_findPanelFindEditFrameRect.right - m_findPanelFindEditFrameRect.left) - (editInsetX * 2),
			(m_findPanelFindEditFrameRect.bottom - m_findPanelFindEditFrameRect.top) - editInsetTop - editInsetBottom);
		moveControl(m_hFindPanelFindNextButton, rightClusterLeft, rowTop, iconButtonWidth, rowHeight);
		moveControl(m_hFindPanelFindPrevButton, rightClusterLeft + iconButtonWidth + iconGap, rowTop, iconButtonWidth, rowHeight);
		moveControl(m_hFindPanelOptionsButton, rightClusterLeft + (iconButtonWidth + iconGap) * 2, rowTop, iconButtonWidth, rowHeight);
		moveControl(m_hFindPanelCloseButton, rightClusterLeft + (iconButtonWidth + iconGap) * 3, rowTop, iconButtonWidth, rowHeight);

		const int searchIconSize = MulDiv(12, dpiY, 96);
		const int searchIconLeft = m_findPanelFindEditFrameRect.right - searchIconSize - MulDiv(12, dpiX, 96);
		const int searchIconTop = rowTop + ((rowHeight - searchIconSize) / 2);
		m_findPanelSearchIconRect = RECT{
			searchIconLeft,
			searchIconTop,
			searchIconLeft + searchIconSize,
			searchIconTop + searchIconSize
		};
		moveControl(
			m_hFindPanelSearchIcon,
			m_findPanelSearchIconRect.left,
			m_findPanelSearchIconRect.top,
			searchIconSize,
			searchIconSize);
		if (m_hFindPanelSearchIcon != nullptr)
		{
			::SetWindowPos(m_hFindPanelSearchIcon, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (m_isReplacePanelVisible)
		{
			const int replaceTop = rowTop + rowHeight + rowSpacing;
			const int replaceButtonWidth = MulDiv(132, dpiX, 96);
			const int replaceAllButtonWidth = MulDiv(152, dpiX, 96);
			const int replaceButtonsTotal = replaceButtonWidth + iconGap + replaceAllButtonWidth;
			const int replaceEditRight = panelRight - innerPadding - replaceButtonsTotal - iconGap;
			const int replaceEditLeft = findEditLeft;
			const int safeReplaceEditRight = (std::max)(replaceEditLeft + minEditWidth, replaceEditRight);
			m_findPanelReplaceEditFrameRect = RECT{ replaceEditLeft, replaceTop, safeReplaceEditRight, replaceTop + rowHeight };
			moveControl(
				m_hFindPanelReplaceEdit,
				m_findPanelReplaceEditFrameRect.left + editInsetX,
				m_findPanelReplaceEditFrameRect.top + editInsetTop,
				(m_findPanelReplaceEditFrameRect.right - m_findPanelReplaceEditFrameRect.left) - (editInsetX * 2),
				(m_findPanelReplaceEditFrameRect.bottom - m_findPanelReplaceEditFrameRect.top) - editInsetTop - editInsetBottom);
			moveControl(m_hFindPanelReplaceButton, safeReplaceEditRight + iconGap, replaceTop, replaceButtonWidth, rowHeight);
			moveControl(m_hFindPanelReplaceAllButton, safeReplaceEditRight + iconGap + replaceButtonWidth + iconGap, replaceTop, replaceAllButtonWidth, rowHeight);
		}
		else
		{
			::SetRectEmpty(&m_findPanelReplaceEditFrameRect);
		}

		UpdateFindPanelButtonsEnabledState();
		const RECT currentAreaRect{
			rcClient.left,
			m_nTopBarHeight,
			rcClient.right,
			m_nTopBarHeight + areaHeight
		};
		RECT invalidateRect = currentAreaRect;
		if (!::IsRectEmpty(&previousAreaRect))
		{
			::UnionRect(&invalidateRect, &previousAreaRect, &currentAreaRect);
		}
		::InvalidateRect(m_hWnd, &invalidateRect, TRUE);
	}

	void LayoutMainControls()
	{
		if (!m_hWnd || !m_view.m_hWnd)
		{
			return;
		}

		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		const int clientWidth = static_cast<int>(rcClient.right - rcClient.left);
		const int clientHeight = static_cast<int>(rcClient.bottom - rcClient.top);
		CClientDC dc(*this);
		const int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
		const int dpiY = GetDeviceCaps(dc, LOGPIXELSY);

		int statusHeight = 0;
		if (m_hWndStatusBar)
		{
			const bool isVisible = ::IsWindowVisible(m_hWndStatusBar) != FALSE;
			if (m_isStatusBarVisible && !isVisible)
			{
				::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
			}
			else if (!m_isStatusBarVisible && isVisible)
			{
				::ShowWindow(m_hWndStatusBar, SW_HIDE);
			}
			if (m_isStatusBarVisible)
			{
				::SendMessage(m_hWndStatusBar, WM_SIZE, 0, 0);
				RECT rcStatus{};
				::GetWindowRect(m_hWndStatusBar, &rcStatus);
				::MapWindowPoints(nullptr, m_hWnd, reinterpret_cast<LPPOINT>(&rcStatus), 2);
				statusHeight = rcStatus.bottom - rcStatus.top;
				if (statusHeight <= 0)
				{
					statusHeight = MulDiv(24, dpiY, 96);
				}

				const int statusLeft = 0;
				const int statusWidth = (std::max)(0, clientWidth);
				const int statusTop = (std::max)(0, clientHeight - statusHeight);
				::SetWindowPos(
					m_hWndStatusBar,
					nullptr,
					statusLeft,
					statusTop,
					statusWidth,
					statusHeight,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

				RECT rcStatusClient{};
				::GetClientRect(m_hWndStatusBar, &rcStatusClient);
				if (rcStatusClient.right > 0)
				{
					const int partChars = MulDiv(150, dpiX, 96);
					const int partPlainText = MulDiv(190, dpiX, 96);
					const int partZoom = MulDiv(90, dpiX, 96);
					const int partEol = MulDiv(180, dpiX, 96);
					const int partEncoding = MulDiv(110, dpiX, 96);
					const int minFlexible = MulDiv(190, dpiX, 96);
					const int fixedWidth = partChars + partPlainText + partZoom + partEol + partEncoding;
					const int firstRight = (std::max)(minFlexible, static_cast<int>(rcStatusClient.right) - fixedWidth);
					int rightEdges[kStatusBarParts]{
						firstRight,
						firstRight + partChars,
						firstRight + partChars + partPlainText,
						firstRight + partChars + partPlainText + partZoom,
						firstRight + partChars + partPlainText + partZoom + partEol,
						-1
					};
					::SendMessageW(m_hWndStatusBar, SB_SETPARTS, kStatusBarParts, reinterpret_cast<LPARAM>(rightEdges));
				}
			}
		}

		LayoutFindPanelControls(rcClient, dpiX, dpiY);
		const int editTop = m_nTopBarHeight;
		const int editLeft = 0;
		const int editRight = clientWidth;
		const int editBottom = clientHeight - statusHeight;
		const int editWidth = (std::max)(0, editRight - editLeft);
		const int editHeight = (std::max)(0, editBottom - editTop);
		::MoveWindow(m_view, editLeft, editTop, editWidth, editHeight, FALSE);
		UpdateEditorFormattingRect();
		UpdateViewClipRegionForFindPanel(RECT{});
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();
	}

	void UpdateViewClipRegionForFindPanel(const RECT& /*rcViewInClient*/) const
	{
		if (m_view.m_hWnd == nullptr || !::IsWindow(m_view.m_hWnd))
		{
			return;
		}

		// Keep the editor region rectangular; non-rectangular clipping causes
		// incorrect internal wrap width in multiline edit controls.
		::SetWindowRgn(m_view.m_hWnd, nullptr, TRUE);
		::InvalidateRect(m_view.m_hWnd, nullptr, FALSE);
	}

	bool IsFindPanelButtonControlId(const UINT controlId) const
	{
		switch (controlId)
		{
		case kFindPanelToggleButtonId:
		case kFindPanelFindNextButtonId:
		case kFindPanelFindPrevButtonId:
		case kFindPanelOptionsButtonId:
		case kFindPanelCloseButtonId:
		case kFindPanelReplaceButtonId:
		case kFindPanelReplaceAllButtonId:
			return true;
		default:
			return false;
		}
	}

	void DrawFindPanelBackground(HDC hdc) const
	{
		if (!m_isFindPanelVisible || ::IsRectEmpty(&m_findPanelRect))
		{
			return;
		}

		HBRUSH hFillBrush = ::CreateSolidBrush(m_findPanelBackgroundColor);
		HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, m_findPanelBorderColor);
		HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
		HGDIOBJ hOldBrush = hFillBrush ? ::SelectObject(hdc, hFillBrush) : nullptr;

		::RoundRect(
			hdc,
			m_findPanelRect.left,
			m_findPanelRect.top,
			m_findPanelRect.right - 1,
			m_findPanelRect.bottom - 1,
			10,
			10);

		if (hOldBrush != nullptr)
		{
			::SelectObject(hdc, hOldBrush);
		}
		if (hOldPen != nullptr)
		{
			::SelectObject(hdc, hOldPen);
		}
		if (hBorderPen != nullptr)
		{
			::DeleteObject(hBorderPen);
		}
		if (hFillBrush != nullptr)
		{
			::DeleteObject(hFillBrush);
		}

		const auto drawInputFrame = [&](const RECT& rcFrame)
		{
			if (::IsRectEmpty(&rcFrame))
			{
				return;
			}

			HBRUSH hInputBrush = ::CreateSolidBrush(m_findPanelInputBackgroundColor);
			HPEN hInputPen = ::CreatePen(PS_SOLID, 1, m_findPanelInputBorderColor);
			HGDIOBJ hOldInputPen = hInputPen ? ::SelectObject(hdc, hInputPen) : nullptr;
			HGDIOBJ hOldInputBrush = hInputBrush ? ::SelectObject(hdc, hInputBrush) : nullptr;
			::RoundRect(hdc, rcFrame.left, rcFrame.top, rcFrame.right - 1, rcFrame.bottom - 1, 7, 7);
			if (hOldInputBrush != nullptr)
			{
				::SelectObject(hdc, hOldInputBrush);
			}
			if (hOldInputPen != nullptr)
			{
				::SelectObject(hdc, hOldInputPen);
			}
			if (hInputPen != nullptr)
			{
				::DeleteObject(hInputPen);
			}
			if (hInputBrush != nullptr)
			{
				::DeleteObject(hInputBrush);
			}
		};

		drawInputFrame(m_findPanelFindEditFrameRect);
		if (m_isReplacePanelVisible)
		{
			drawInputFrame(m_findPanelReplaceEditFrameRect);
		}

		if (!::IsRectEmpty(&m_findPanelSearchIconRect))
		{
			RECT rcIcon = m_findPanelSearchIconRect;
			const HFONT iconFont = (m_fontFindPanelIcons.m_hFont != nullptr)
				? static_cast<HFONT>(m_fontFindPanelIcons)
				: GetToolbarIconFontHandle();
			HFONT hOldFont = nullptr;
			if (iconFont != nullptr)
			{
				hOldFont = reinterpret_cast<HFONT>(::SelectObject(hdc, iconFont));
			}
			::SetBkMode(hdc, TRANSPARENT);
			::SetTextColor(hdc, m_isDarkThemeApplied ? RGB(176, 176, 176) : RGB(108, 108, 108));
			::DrawTextW(hdc, L"\uE721", 1, &rcIcon, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
			if (hOldFont != nullptr)
			{
				::SelectObject(hdc, hOldFont);
			}
		}
	}

	bool DrawFindPanelButton(const DRAWITEMSTRUCT* pDrawItem) const
	{
		if (pDrawItem == nullptr || pDrawItem->CtlType != ODT_BUTTON || !IsFindPanelButtonControlId(pDrawItem->CtlID))
		{
			return false;
		}

		const bool isDisabled = (pDrawItem->itemState & ODS_DISABLED) != 0;
		const bool isPressed = (pDrawItem->itemState & ODS_SELECTED) != 0;
		const bool isHot =
			(pDrawItem->itemState & ODS_HOTLIGHT) != 0 ||
			pDrawItem->hwndItem == m_hHotFindPanelButton;
		const bool isIconButton =
			pDrawItem->CtlID != kFindPanelReplaceButtonId &&
			pDrawItem->CtlID != kFindPanelReplaceAllButtonId;

		COLORREF backgroundColor = m_findPanelBackgroundColor;
		COLORREF borderColor = m_findPanelBackgroundColor;
		COLORREF textColor = m_findPanelButtonTextColor;
		bool drawRoundedButton = false;

		if (isIconButton)
		{
			if (isPressed)
			{
				backgroundColor = m_findPanelButtonPressedColor;
				borderColor = m_findPanelInputBorderColor;
				drawRoundedButton = true;
			}
			else if (isHot)
			{
				backgroundColor = m_findPanelButtonHoverColor;
				borderColor = m_findPanelInputBorderColor;
				drawRoundedButton = true;
			}
		}
		else
		{
			backgroundColor = m_findPanelInputBackgroundColor;
			borderColor = m_findPanelInputBorderColor;
			drawRoundedButton = true;
			if (isPressed)
			{
				backgroundColor = m_findPanelButtonPressedColor;
			}
			else if (isHot)
			{
				backgroundColor = m_findPanelButtonHoverColor;
			}
		}
		if (isDisabled)
		{
			textColor = m_isDarkThemeApplied ? RGB(132, 132, 132) : RGB(145, 145, 145);
			if (!isIconButton)
			{
				backgroundColor = m_isDarkThemeApplied ? m_findPanelInputBackgroundColor : RGB(238, 240, 245);
				borderColor = m_isDarkThemeApplied ? m_findPanelInputBorderColor : RGB(214, 218, 226);
			}
		}

		HDC hdc = pDrawItem->hDC;
		RECT rc = pDrawItem->rcItem;
		HBRUSH hBaseBrush = ::CreateSolidBrush(m_findPanelBackgroundColor);
		if (hBaseBrush != nullptr)
		{
			::FillRect(hdc, &rc, hBaseBrush);
			::DeleteObject(hBaseBrush);
		}
		HBRUSH hFillBrush = ::CreateSolidBrush(backgroundColor);
		if (drawRoundedButton)
		{
			HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, borderColor);
			HGDIOBJ hOldPen = hBorderPen ? ::SelectObject(hdc, hBorderPen) : nullptr;
			HGDIOBJ hOldBrush = hFillBrush ? ::SelectObject(hdc, hFillBrush) : nullptr;
			const int radius = isIconButton ? 6 : 7;
			::RoundRect(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1, radius, radius);
			if (hOldBrush != nullptr)
			{
				::SelectObject(hdc, hOldBrush);
			}
			if (hOldPen != nullptr)
			{
				::SelectObject(hdc, hOldPen);
			}
			if (hBorderPen != nullptr)
			{
				::DeleteObject(hBorderPen);
			}
		}
		else
		{
			::FillRect(hdc, &rc, hFillBrush);
		}
		if (hFillBrush != nullptr)
		{
			::DeleteObject(hFillBrush);
		}

		::SetBkMode(hdc, TRANSPARENT);
		wchar_t caption[128]{};
		::GetWindowTextW(pDrawItem->hwndItem, caption, static_cast<int>(std::size(caption)));
		const HFONT drawFont = isIconButton
			? ((m_fontFindPanelIcons.m_hFont != nullptr) ? static_cast<HFONT>(m_fontFindPanelIcons) : GetToolbarIconFontHandle())
			: ((m_fontFindPanelText.m_hFont != nullptr) ? static_cast<HFONT>(m_fontFindPanelText) : GetUiFontHandle());
		HFONT hOldFont = nullptr;
		if (drawFont != nullptr)
		{
			hOldFont = reinterpret_cast<HFONT>(::SelectObject(hdc, drawFont));
		}
		::SetTextColor(hdc, textColor);
		RECT rcText = rc;
		if (!isIconButton)
		{
			rcText.left += 8;
			rcText.right -= 8;
		}
		::DrawTextW(hdc, caption, -1, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
		if (hOldFont != nullptr)
		{
			::SelectObject(hdc, hOldFont);
		}

		return true;
	}

	int HitTestTopMenuButton(const POINT ptClient) const
	{
		for (size_t i = 0; i < m_topMenuButtonRects.size(); ++i)
		{
			if (::PtInRect(&m_topMenuButtonRects[i], ptClient))
			{
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int HitTestToolbarButton(const POINT ptClient) const
	{
		if (ptClient.y < 0 || ptClient.y >= m_nTopBarHeight)
		{
			return -1;
		}

		for (size_t i = 0; i < m_toolbarButtonRects.size(); ++i)
		{
			if (::PtInRect(&m_toolbarButtonRects[i], ptClient))
			{
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	void OpenTopMenuPopupByPosition(const int menuPos, const RECT& anchorRect, const int pressedButtonIndex)
	{
		const HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr || menuPos < 0)
		{
			return;
		}

		HMENU hSubMenu = ::GetSubMenu(hMenu, menuPos);
		if (hSubMenu == nullptr)
		{
			return;
		}

		POINT popupOrigin{
			anchorRect.left,
			anchorRect.bottom
		};
		::ClientToScreen(m_hWnd, &popupOrigin);

		m_topMenuPressedIndex = pressedButtonIndex;
		InvalidateTopBar();
		EnsureStatusBarVisibleInPartsMode();

		const UINT commandId = ::TrackPopupMenuEx(
			hSubMenu,
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NOANIMATION,
			popupOrigin.x,
			popupOrigin.y,
			m_hWnd,
			nullptr);

		m_topMenuPressedIndex = -1;
		m_topMenuHoveredIndex = -1;
		InvalidateTopBar();
		EnsureStatusBarVisibleInPartsMode();
		LayoutMainControls();
		UpdateStatusBar();

		if (commandId != 0)
		{
			::PostMessageW(m_hWnd, WM_COMMAND, MAKEWPARAM(commandId, 0), 0);
		}
	}

	void OpenOverflowTopMenuPopup(const int buttonIndex)
	{
		if (buttonIndex < 0 || buttonIndex >= static_cast<int>(m_topMenuButtonRects.size()))
		{
			return;
		}
		if (m_topMenuOverflowPopupPositions.empty() || m_topMenuOverflowCaptions.empty())
		{
			return;
		}

		CMenu overflowMenu;
		if (!overflowMenu.CreatePopupMenu())
		{
			return;
		}

		for (size_t i = 0; i < m_topMenuOverflowPopupPositions.size() && i < m_topMenuOverflowCaptions.size(); ++i)
		{
			const UINT commandId = kTopMenuOverflowCommandBase + static_cast<UINT>(i);
			::AppendMenuW(overflowMenu, MF_STRING, commandId, m_topMenuOverflowCaptions[i].c_str());
		}

		const RECT anchorRect = m_topMenuButtonRects[static_cast<size_t>(buttonIndex)];
		POINT popupOrigin{
			anchorRect.left,
			anchorRect.bottom
		};
		::ClientToScreen(m_hWnd, &popupOrigin);

		m_topMenuPressedIndex = buttonIndex;
		InvalidateTopBar();
		EnsureStatusBarVisibleInPartsMode();

		const UINT selectedId = ::TrackPopupMenuEx(
			overflowMenu,
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NOANIMATION,
			popupOrigin.x,
			popupOrigin.y,
			m_hWnd,
			nullptr);

		m_topMenuPressedIndex = -1;
		m_topMenuHoveredIndex = -1;
		InvalidateTopBar();
		EnsureStatusBarVisibleInPartsMode();

		int selectedMenuPos = -1;
		if (selectedId >= kTopMenuOverflowCommandBase)
		{
			const size_t selectedIndex = static_cast<size_t>(selectedId - kTopMenuOverflowCommandBase);
			if (selectedIndex < m_topMenuOverflowPopupPositions.size())
			{
				selectedMenuPos = m_topMenuOverflowPopupPositions[selectedIndex];
			}
		}
		if (selectedMenuPos >= 0)
		{
			OpenTopMenuPopupByPosition(selectedMenuPos, anchorRect, buttonIndex);
			return;
		}

		LayoutMainControls();
		UpdateStatusBar();
	}

	void OpenTopMenuAtIndex(const int buttonIndex)
	{
		if (buttonIndex < 0 || buttonIndex >= static_cast<int>(m_topMenuPopupPositions.size()) || buttonIndex >= static_cast<int>(m_topMenuButtonRects.size()))
		{
			return;
		}

		const int menuPos = m_topMenuPopupPositions[buttonIndex];
		if (menuPos == kTopMenuOverflowMarker)
		{
			OpenOverflowTopMenuPopup(buttonIndex);
			return;
		}

		OpenTopMenuPopupByPosition(menuPos, m_topMenuButtonRects[static_cast<size_t>(buttonIndex)], buttonIndex);
	}

	void ShowCommandPalette()
	{
		if (!m_hWnd)
		{
			return;
		}

		ApplyPopupMenuTheme();
		CMenu paletteMenu;
		if (!paletteMenu.CreatePopupMenu())
		{
			return;
		}

		const bool isRu = IsRussianUi();
		::AppendMenuW(paletteMenu, MF_STRING, ID_FILE_SAVE, isRu ? L"\u0421\u043E\u0445\u0440\u0430\u043D\u0438\u0442\u044C\tCtrl+S" : L"Save\tCtrl+S");
		::AppendMenuW(paletteMenu, MF_STRING, ID_FILE_SAVE_AS, isRu ? L"\u0421\u043E\u0445\u0440\u0430\u043D\u0438\u0442\u044C \u043A\u0430\u043A..." : L"Save As...");
		::AppendMenuW(paletteMenu, MF_SEPARATOR, 0, nullptr);
		::AppendMenuW(paletteMenu, MF_STRING, ID_EDIT_FIND, isRu ? L"\u041D\u0430\u0439\u0442\u0438...\tCtrl+F" : L"Find...\tCtrl+F");
		::AppendMenuW(paletteMenu, MF_STRING, ID_EDIT_FINDNEXT, isRu ? L"\u041D\u0430\u0439\u0442\u0438 \u0434\u0430\u043B\u0435\u0435\tF3" : L"Find Next\tF3");
		::AppendMenuW(paletteMenu, MF_STRING, ID_EDIT_SELECT_ALL, isRu ? L"\u0412\u044B\u0434\u0435\u043B\u0438\u0442\u044C \u0432\u0441\u0435\tCtrl+A" : L"Select All\tCtrl+A");
		::AppendMenuW(paletteMenu, MF_SEPARATOR, 0, nullptr);
		::AppendMenuW(paletteMenu, MF_STRING, ID_STEGANOS_PASSWORD_MANAGER, isRu ? L"\u0428\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435: \u041C\u0435\u043D\u0435\u0434\u0436\u0435\u0440 \u043F\u0430\u0440\u043E\u043B\u0435\u0439" : L"Encryption: Password Manager");
		::AppendMenuW(paletteMenu, MF_STRING, ID_STEGANOS_SAFE, isRu ? L"\u0428\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435: Safe" : L"Encryption: Safe");
		::AppendMenuW(paletteMenu, MF_SEPARATOR, 0, nullptr);
		::AppendMenuW(paletteMenu, MF_STRING, ID_THEME_SYSTEM, isRu ? L"\u0422\u0435\u043C\u0430: \u0421\u0438\u0441\u0442\u0435\u043C\u043D\u0430\u044F" : L"Theme: System");
		::AppendMenuW(paletteMenu, MF_STRING, ID_THEME_LIGHT, isRu ? L"\u0422\u0435\u043C\u0430: \u0421\u0432\u0435\u0442\u043B\u0430\u044F" : L"Theme: Light");
		::AppendMenuW(paletteMenu, MF_STRING, ID_THEME_DARK, isRu ? L"\u0422\u0435\u043C\u0430: \u0422\u0435\u043C\u043D\u0430\u044F" : L"Theme: Dark");
		::AppendMenuW(paletteMenu, MF_SEPARATOR, 0, nullptr);
		::AppendMenuW(paletteMenu, MF_STRING, ID_APP_ABOUT, isRu ? L"\u041E \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0435" : L"About");

		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		CClientDC dc(*this);
		const int dpiX = (std::max)(GetDeviceCaps(dc, LOGPIXELSX), 96);
		const int clientWidth = static_cast<int>(rcClient.right - rcClient.left);
		POINT popupOrigin{
			(std::max)(0, (clientWidth / 2) - MulDiv(120, dpiX, 96)),
			m_nTopBarHeight + MulDiv(8, dpiX, 96)
		};
		::ClientToScreen(m_hWnd, &popupOrigin);

		const UINT commandId = ::TrackPopupMenuEx(
			paletteMenu,
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
			popupOrigin.x,
			popupOrigin.y,
			m_hWnd,
			nullptr);
		if (commandId != 0)
		{
			::PostMessageW(m_hWnd, WM_COMMAND, MAKEWPARAM(commandId, 0), 0);
		}
	}

	bool HandleTopBarRightClick(const POINT& ptClient)
	{
		if (!UseCustomTitleBar())
		{
			return false;
		}
		if (ptClient.y < 0 || ptClient.y >= m_nTopBarHeight)
		{
			return false;
		}

		const int menuIndex = HitTestTopMenuButton(ptClient);
		if (menuIndex >= 0)
		{
			OpenTopMenuAtIndex(menuIndex);
			return true;
		}

		if (!IsPointInTitleBar(ptClient) && !IsPointInMenuBar(ptClient))
		{
			return false;
		}

		POINT ptScreen = ptClient;
		::ClientToScreen(m_hWnd, &ptScreen);
		HMENU hSystemMenu = ::GetSystemMenu(m_hWnd, FALSE);
		if (hSystemMenu == nullptr)
		{
			return true;
		}

		const UINT command = static_cast<UINT>(::TrackPopupMenu(
			hSystemMenu,
			TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN,
			ptScreen.x,
			ptScreen.y,
			0,
			m_hWnd,
			nullptr));
		if (command != 0U)
		{
			::PostMessageW(m_hWnd, WM_SYSCOMMAND, command, 0);
		}
		return true;
	}

	void ExecuteToolbarCommand(const int buttonIndex)
	{
		if (buttonIndex < 0 || buttonIndex >= static_cast<int>(m_toolbarCommandIds.size()))
		{
			return;
		}

		m_toolbarPressedIndex = buttonIndex;
		InvalidateTopBar();
		const UINT commandId = m_toolbarCommandIds[buttonIndex];
		if (commandId != 0)
		{
			::PostMessageW(m_hWnd, WM_COMMAND, MAKEWPARAM(commandId, 0), 0);
		}
		m_toolbarPressedIndex = -1;
		InvalidateTopBar();
	}

	void ExecuteTitleBarButtonCommand(const int buttonIndex)
	{
		UINT systemCommand = 0;
		switch (buttonIndex)
		{
		case kTitleBarButtonMinimize:
			systemCommand = SC_MINIMIZE;
			break;
		case kTitleBarButtonMaximize:
			systemCommand = ::IsZoomed(m_hWnd) ? SC_RESTORE : SC_MAXIMIZE;
			break;
		case kTitleBarButtonClose:
			systemCommand = SC_CLOSE;
			break;
		default:
			return;
		}

		::PostMessageW(m_hWnd, WM_SYSCOMMAND, systemCommand, 0);
	}

	COLORREF GetTitleBarButtonFillColor(const int buttonIndex) const
	{
		const bool isHovered = buttonIndex == m_titleBarHoveredButtonIndex;
		const bool isPressed = buttonIndex == m_titleBarPressedButtonIndex;
		if (buttonIndex == kTitleBarButtonClose)
		{
			if (isPressed)
			{
				return m_isDarkThemeApplied ? RGB(168, 42, 32) : RGB(196, 58, 46);
			}
			if (isHovered)
			{
				return BlendColors(
					m_titleBarBackgroundColor,
					RGB(196, 43, 28),
					static_cast<BYTE>(MulDiv(255, m_titleBarHoverBlend, 255)));
			}
			return m_titleBarBackgroundColor;
		}

		if (isPressed)
		{
			return BlendColors(m_titleBarBackgroundColor, m_topBarTextColor, m_isDarkThemeApplied ? 48 : 30);
		}
		if (isHovered)
		{
			const BYTE hoverBlend = static_cast<BYTE>(MulDiv(m_isDarkThemeApplied ? 28 : 18, m_titleBarHoverBlend, 255));
			return BlendColors(m_titleBarBackgroundColor, m_topBarTextColor, hoverBlend);
		}
		return m_titleBarBackgroundColor;
	}

	COLORREF GetTitleBarButtonGlyphColor(const int buttonIndex) const
	{
		if (buttonIndex == kTitleBarButtonClose &&
			(buttonIndex == m_titleBarHoveredButtonIndex || buttonIndex == m_titleBarPressedButtonIndex))
		{
			return RGB(255, 255, 255);
		}
		return m_topBarTextColor;
	}

	void DrawTitleBarButtonGlyph(HDC hdc, const int buttonIndex, const RECT& rcButton) const
	{
		if (::IsRectEmpty(&rcButton))
		{
			return;
		}

		const wchar_t* glyph = L"";
		switch (buttonIndex)
		{
		case kTitleBarButtonMinimize:
			glyph = L"\xE921";
			break;
		case kTitleBarButtonMaximize:
			glyph = (::IsZoomed(m_hWnd) != FALSE) ? L"\xE923" : L"\xE922";
			break;
		case kTitleBarButtonClose:
			glyph = L"\xE8BB";
			break;
		default:
			return;
		}

		const HGDIOBJ oldFont = ::SelectObject(hdc, GetTitleBarIconFontHandle());
		::SetBkMode(hdc, TRANSPARENT);
		::SetTextColor(hdc, GetTitleBarButtonGlyphColor(buttonIndex));
		RECT rcGlyph = rcButton;
		::DrawTextW(hdc, glyph, 1, &rcGlyph, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
		::SelectObject(hdc, oldFont);
	}

	void DrawTopBar(HDC hdc)
	{
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);

		if (UseCustomTitleBar())
		{
			RECT rcTitleBar{ 0, 0, rcClient.right, m_nTopBarHeight };
			CBrush titleBarBrush;
			titleBarBrush.CreateSolidBrush(m_titleBarBackgroundColor);
			::FillRect(hdc, &rcTitleBar, titleBarBrush);
		}
		else
		{
			RECT rcMenuBar{ 0, 0, rcClient.right, m_nTopMenuHeight };
			CBrush menuBarBrush;
			menuBarBrush.CreateSolidBrush(m_topBarBackgroundColor);
			::FillRect(hdc, &rcMenuBar, menuBarBrush);
		}

		HFONT hOldBaseFont = reinterpret_cast<HFONT>(::SelectObject(hdc, GetUiFontHandle()));
		::SetBkMode(hdc, TRANSPARENT);

		if (UseCustomTitleBar() && !::IsRectEmpty(&m_topBarTitleRect))
		{
			const int dpiX = (std::max)(GetDeviceCaps(hdc, LOGPIXELSX), 96);
			if (!::IsRectEmpty(&m_topBarTitleBandRect))
			{
				CBrush tabBrush;
				tabBrush.CreateSolidBrush(BlendColors(m_topBarBackgroundColor, RGB(19, 21, 34), 206));
				CPen tabBorderPen;
				tabBorderPen.CreatePen(PS_SOLID, 1, BlendColors(m_topBarBackgroundColor, m_topBarTextColor, 26));
				const HGDIOBJ oldBrush = ::SelectObject(hdc, tabBrush);
				const HGDIOBJ oldPen = ::SelectObject(hdc, tabBorderPen);
				const int tabRadius = (std::max)(MulDiv(13, dpiX, 96), 9);
				::RoundRect(
					hdc,
					m_topBarTitleBandRect.left,
					m_topBarTitleBandRect.top,
					m_topBarTitleBandRect.right,
					m_topBarTitleBandRect.bottom,
					tabRadius,
					tabRadius);
				::SelectObject(hdc, oldPen);
				::SelectObject(hdc, oldBrush);

				const int indicatorSize = (std::max)(MulDiv(8, dpiX, 96), 6);
				const int indicatorX = m_topBarTitleBandRect.right - MulDiv(22, dpiX, 96);
				const int indicatorY = m_topBarTitleBandRect.top + ((m_topBarTitleBandRect.bottom - m_topBarTitleBandRect.top - indicatorSize) / 2);
				RECT rcIndicator{ indicatorX, indicatorY, indicatorX + indicatorSize, indicatorY + indicatorSize };
				CBrush indicatorBrush;
				indicatorBrush.CreateSolidBrush(RGB(150, 150, 160));
				CPen indicatorPen;
				indicatorPen.CreatePen(PS_SOLID, 1, RGB(150, 150, 160));
				const HGDIOBJ oldIndicatorBrush = ::SelectObject(hdc, indicatorBrush);
				const HGDIOBJ oldIndicatorPen = ::SelectObject(hdc, indicatorPen);
				::Ellipse(hdc, rcIndicator.left, rcIndicator.top, rcIndicator.right, rcIndicator.bottom);
				::SelectObject(hdc, oldIndicatorPen);
				::SelectObject(hdc, oldIndicatorBrush);
			}

			RECT rcTitleText = m_topBarTitleRect;
			::SetTextColor(hdc, m_topBarTextColor);
			::DrawTextW(
				hdc,
				m_topBarTitleText.c_str(),
				-1,
				&rcTitleText,
				DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
		}

		if (UseCustomTitleBar())
		{
			for (int buttonIndex = 0; buttonIndex < kTitleBarButtonCount; ++buttonIndex)
			{
				const RECT& rcButton = m_titleBarButtonRects[static_cast<size_t>(buttonIndex)];
				if (::IsRectEmpty(&rcButton))
				{
					continue;
				}

				const COLORREF fillColor = GetTitleBarButtonFillColor(buttonIndex);
				if (fillColor != m_titleBarBackgroundColor)
				{
					CBrush buttonBrush;
					buttonBrush.CreateSolidBrush(fillColor);
					::FillRect(hdc, &rcButton, buttonBrush);
				}

				DrawTitleBarButtonGlyph(hdc, buttonIndex, rcButton);
			}
		}

		for (size_t i = 0; i < m_topMenuButtonRects.size(); ++i)
		{
			RECT rcButton = m_topMenuButtonRects[i];
			const std::wstring caption = (i < m_topMenuCaptions.size()) ? m_topMenuCaptions[i] : L"";

			if (static_cast<int>(i) == m_topMenuPressedIndex)
			{
				CBrush pressedBrush;
				pressedBrush.CreateSolidBrush(m_topBarPressedColor);
				CPen pressedPen;
				pressedPen.CreatePen(PS_SOLID, 1, BlendColors(m_topBarPressedColor, m_topBarDividerColor, 96));
				const HGDIOBJ oldBrush = ::SelectObject(hdc, pressedBrush);
				const HGDIOBJ oldPen = ::SelectObject(hdc, pressedPen);
				const int radius = (std::max)(MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSX), 96), 5);
				::RoundRect(hdc, rcButton.left, rcButton.top, rcButton.right, rcButton.bottom, radius, radius);
				::SelectObject(hdc, oldPen);
				::SelectObject(hdc, oldBrush);
			}
			else if (static_cast<int>(i) == m_topMenuHoveredIndex)
			{
				const COLORREF hoverFill = BlendColors(m_topBarBackgroundColor, m_topBarHoverColor, m_topMenuHoverBlend);
				CBrush hoverBrush;
				hoverBrush.CreateSolidBrush(hoverFill);
				CPen hoverPen;
				hoverPen.CreatePen(PS_SOLID, 1, BlendColors(hoverFill, m_topBarDividerColor, 96));
				const HGDIOBJ oldBrush = ::SelectObject(hdc, hoverBrush);
				const HGDIOBJ oldPen = ::SelectObject(hdc, hoverPen);
				const int radius = (std::max)(MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSX), 96), 5);
				::RoundRect(hdc, rcButton.left, rcButton.top, rcButton.right, rcButton.bottom, radius, radius);
				::SelectObject(hdc, oldPen);
				::SelectObject(hdc, oldBrush);
			}

			::SetTextColor(hdc, m_topBarTextColor);
			::DrawTextW(
				hdc,
				caption.c_str(),
				static_cast<int>(caption.size()),
				&rcButton,
				DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_HIDEPREFIX | DT_END_ELLIPSIS);
		}

		HFONT hOldToolbarFont = nullptr;
		if (!m_toolbarButtonRects.empty())
		{
			hOldToolbarFont = reinterpret_cast<HFONT>(::SelectObject(hdc, GetUiFontHandle()));
		}
		for (size_t i = 0; i < m_toolbarButtonRects.size(); ++i)
		{
			RECT rcButton = m_toolbarButtonRects[i];
			const std::wstring caption = (i < m_toolbarButtonCaptions.size()) ? m_toolbarButtonCaptions[i] : L"";
			if (static_cast<int>(i) == m_toolbarPressedIndex)
			{
				CBrush pressedBrush;
				pressedBrush.CreateSolidBrush(m_toolbarPressedColor);
				::FillRect(hdc, &rcButton, pressedBrush);
			}
			else if (static_cast<int>(i) == m_toolbarHoveredIndex)
			{
				CBrush hoverBrush;
				hoverBrush.CreateSolidBrush(m_toolbarHoverColor);
				::FillRect(hdc, &rcButton, hoverBrush);
			}
			::SetTextColor(hdc, m_topBarTextColor);
			::DrawTextW(hdc, caption.c_str(), static_cast<int>(caption.size()), &rcButton, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_HIDEPREFIX);
		}
		if (hOldToolbarFont != nullptr)
		{
			::SelectObject(hdc, hOldToolbarFont);
		}

		// Visual separator under menu/top bar.
		if (m_nTopBarHeight > 0)
		{
			RECT rcHardDivider{
				0,
				m_nTopBarHeight - 1,
				rcClient.right,
				m_nTopBarHeight
			};
			CBrush hardDividerBrush;
			hardDividerBrush.CreateSolidBrush(m_topBarDividerColor);
			::FillRect(hdc, &rcHardDivider, hardDividerBrush);
		}

		::SelectObject(hdc, hOldBaseFont);
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PAINTSTRUCT ps{};
		HDC hdc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		RECT rcTopBar{ 0, 0, rcClient.right, m_nTopBarHeight };
		RECT rcPaintTop{};
		if (::IntersectRect(&rcPaintTop, &rcTopBar, &ps.rcPaint))
		{
			HDC hdcMem = ::CreateCompatibleDC(hdc);
			if (hdcMem != nullptr)
			{
				HBITMAP hbmMem = ::CreateCompatibleBitmap(hdc, rcTopBar.right, rcTopBar.bottom);
				if (hbmMem != nullptr)
				{
					HGDIOBJ hOldBitmap = ::SelectObject(hdcMem, hbmMem);
					DrawTopBar(hdcMem);
					::BitBlt(
						hdc,
						rcPaintTop.left,
						rcPaintTop.top,
						rcPaintTop.right - rcPaintTop.left,
						rcPaintTop.bottom - rcPaintTop.top,
						hdcMem,
						rcPaintTop.left,
						rcPaintTop.top,
						SRCCOPY);
					::SelectObject(hdcMem, hOldBitmap);
					::DeleteObject(hbmMem);
				}
				else
				{
					DrawTopBar(hdc);
				}
				::DeleteDC(hdcMem);
			}
			else
			{
				DrawTopBar(hdc);
			}
		}
		if (m_isFindPanelVisible)
		{
			const int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
			const int findAreaHeight = GetFindPanelAreaHeightForDpi(dpiY);
			RECT rcFindArea{
				0,
				m_nTopBarHeight,
				rcClient.right,
				m_nTopBarHeight + findAreaHeight
			};
			RECT rcPaintFindArea{};
			if (::IntersectRect(&rcPaintFindArea, &rcFindArea, &ps.rcPaint))
			{
				HBRUSH hAreaBrush = m_viewBackgroundBrush.m_hBrush != nullptr
					? static_cast<HBRUSH>(m_viewBackgroundBrush)
					: static_cast<HBRUSH>(::GetStockObject(DC_BRUSH));
				if (hAreaBrush == static_cast<HBRUSH>(::GetStockObject(DC_BRUSH)))
				{
					::SetDCBrushColor(hdc, m_viewBackgroundColor);
				}
				::FillRect(hdc, &rcPaintFindArea, hAreaBrush);
			}
			RECT rcPaintFindPanel{};
			if (!::IsRectEmpty(&m_findPanelRect) && ::IntersectRect(&rcPaintFindPanel, &m_findPanelRect, &ps.rcPaint))
			{
				DrawFindPanelBackground(hdc);
			}
		}
		::EndPaint(m_hWnd, &ps);
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		const int hoveredTitleButtonIndex = HitTestTitleBarButton(pt);
		const int hoveredMenuIndex = HitTestTopMenuButton(pt);
		const int hoveredToolbarIndex = HitTestToolbarButton(pt);
		if (hoveredTitleButtonIndex != m_titleBarHoveredButtonIndex ||
			hoveredMenuIndex != m_topMenuHoveredIndex ||
			hoveredToolbarIndex != m_toolbarHoveredIndex)
		{
			m_titleBarHoveredButtonIndex = hoveredTitleButtonIndex;
			m_topMenuHoveredIndex = hoveredMenuIndex;
			m_toolbarHoveredIndex = hoveredToolbarIndex;
			EnsureTopBarAnimationTimer();
			InvalidateTopBar();
		}

		TRACKMOUSEEVENT tme{ sizeof(tme) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_titleBarHoveredButtonIndex != -1 || m_topMenuHoveredIndex != -1 || m_toolbarHoveredIndex != -1)
		{
			m_titleBarHoveredButtonIndex = -1;
			m_topMenuHoveredIndex = -1;
			m_toolbarHoveredIndex = -1;
			EnsureTopBarAnimationTimer();
			InvalidateTopBar();
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam == kTopBarAnimationTimerId)
		{
			UpdateTopBarAnimationState();
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		const int titleButtonIndex = HitTestTitleBarButton(pt);
		if (titleButtonIndex >= 0)
		{
			m_titleBarPressedButtonIndex = titleButtonIndex;
			m_titleBarHoveredButtonIndex = titleButtonIndex;
			::SetCapture(m_hWnd);
			EnsureTopBarAnimationTimer();
			InvalidateTopBar();
			return 0;
		}
		if (HitTestTopMenuButton(pt) >= 0)
		{
			return 0;
		}
		if (IsPointInTitleBar(pt))
		{
			POINT ptScreen = pt;
			::ClientToScreen(m_hWnd, &ptScreen);
			::ReleaseCapture();
			::SendMessageW(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(ptScreen.x, ptScreen.y));
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		if (HitTestTopMenuButton(pt) < 0 && IsPointInTitleBar(pt) && HitTestTitleBarButton(pt) < 0)
		{
			::PostMessageW(m_hWnd, WM_SYSCOMMAND, ::IsZoomed(m_hWnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		if (m_titleBarPressedButtonIndex >= 0)
		{
			const int pressedButtonIndex = m_titleBarPressedButtonIndex;
			const int releasedButtonIndex = HitTestTitleBarButton(pt);
			m_titleBarPressedButtonIndex = -1;
			if (::GetCapture() == m_hWnd)
			{
				::ReleaseCapture();
			}
			EnsureTopBarAnimationTimer();
			InvalidateTopBar();
			if (releasedButtonIndex == pressedButtonIndex)
			{
				ExecuteTitleBarButtonCommand(releasedButtonIndex);
				return 0;
			}
		}

		const int toolbarIndex = HitTestToolbarButton(pt);
		if (toolbarIndex >= 0)
		{
			ExecuteToolbarCommand(toolbarIndex);
			return 0;
		}

		const int buttonIndex = HitTestTopMenuButton(pt);
		if (buttonIndex >= 0)
		{
			OpenTopMenuAtIndex(buttonIndex);
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		if (HandleTopBarRightClick(pt))
		{
			bHandled = TRUE;
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		POINT ptScreen{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		if (ptScreen.x == -1 && ptScreen.y == -1)
		{
			RECT rcWindow{};
			::GetWindowRect(m_hWnd, &rcWindow);
			ptScreen.x = rcWindow.left + 24;
			ptScreen.y = rcWindow.top + (m_nTopBarHeight / 2);
		}

		POINT ptClient = ptScreen;
		::ScreenToClient(m_hWnd, &ptClient);
		if (HandleTopBarRightClick(ptClient))
		{
			bHandled = TRUE;
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		POINT ptScreen{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		POINT ptClient = ptScreen;
		::ScreenToClient(m_hWnd, &ptClient);
		if (HandleTopBarRightClick(ptClient))
		{
			bHandled = TRUE;
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnCancelMode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_titleBarPressedButtonIndex != -1)
		{
			m_titleBarPressedButtonIndex = -1;
			if (::GetCapture() == m_hWnd)
			{
				::ReleaseCapture();
			}
			InvalidateTopBar();
		}

		bHandled = FALSE;
		return 0;
	}

	void EnsureStatusBarVisibleInPartsMode()
	{
		if (m_hWndStatusBar == nullptr || !::IsWindow(m_hWndStatusBar))
		{
			return;
		}

		::SendMessageW(m_hWndStatusBar, SB_SIMPLE, FALSE, 0);
		NormalizeStatusBarStyles();
		if (!m_isStatusBarVisible)
		{
			::ShowWindow(m_hWndStatusBar, SW_HIDE);
			return;
		}

		::SetWindowPos(
			m_hWndStatusBar,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		::SendMessageW(m_hWndStatusBar, WM_SIZE, 0, 0);
		::InvalidateRect(m_hWndStatusBar, nullptr, FALSE);
	}

	LRESULT OnEnterMenuLoop(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		EnsureStatusBarVisibleInPartsMode();
		UpdateStatusBar();
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnExitMenuLoop(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		EnsureStatusBarVisibleInPartsMode();
		UpdateStatusBar();
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnMenuSelect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		EnsureStatusBarVisibleInPartsMode();
		UpdateStatusBar();
		bHandled = TRUE;
		return 0;
	}

	void SetViewTextWithoutTracking(const std::string& text)
	{
		m_ignoreEditNotifications = true;
		m_view.SetWindowText(utf8_to_wstring(text).c_str());
		m_view.SetModify(FALSE);
		m_ignoreEditNotifications = false;
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void UpdateStatusBar()
	{
		auto num_lines = SendMessage(m_hWndClient, EM_GETLINECOUNT, 0, 0);
		auto current_line_number = SendMessage(m_hWndClient, EM_LINEFROMCHAR, -1, 0);
		auto index = SendMessage(m_hWndClient, EM_LINEINDEX, -1, 0);
		DWORD dwStart = 0, dwEnd = 0;
		auto column = index;
		SendMessage(m_hWndClient, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
		if (dwStart > 0 && static_cast<unsigned int>(index) <= dwStart)
		{
			column = dwStart - index;
		}

		current_line_number += 1;
		column += 1;
		if (column < 1)
		{
			column = 1;
		}

		const int charsCount = m_view.GetWindowTextLength();
		wchar_t statusPart0[64]{};
		wchar_t statusPart1[64]{};
		const bool isRussianUi = static_cast<WORD>(m_nLanguage) == LANG_RUSSIAN;
		if (isRussianUi)
		{
			swprintf_s(
				statusPart0,
				L"\x0421\x0442\x0440\x043E\x043A\x0430 %d, \x0441\x0442\x043E\x043B\x0431\x0435\x0446 %d",
				static_cast<int>(current_line_number),
				static_cast<int>(column));
			swprintf_s(statusPart1, L"%d \x0441\x0438\x043C\x0432\x043E\x043B\x043E\x0432", charsCount);
		}
		else
		{
			swprintf_s(statusPart0, L"Ln %d, Col %d", static_cast<int>(current_line_number), static_cast<int>(column));
			swprintf_s(statusPart1, L"%d chars", charsCount);
		}
		m_statusBarPartTexts[0] = statusPart0;
		m_statusBarPartTexts[1] = statusPart1;
		m_statusBarPartTexts[2] = isRussianUi
			? L"\x041E\x0431\x044B\x0447\x043D\x044B\x0439 \x0442\x0435\x043A\x0441\x0442"
			: L"Plain text";
		m_statusBarPartTexts[3] = L"100%";
		m_statusBarPartTexts[4] = L"Windows (CRLF)";
		m_statusBarPartTexts[5] = L"UTF-8";
		m_statusBarText = m_statusBarPartTexts[0];
		if (m_hWndStatusBar && m_isStatusBarVisible)
		{
			for (int i = 0; i < kStatusBarParts; ++i)
			{
				SendMessageW(
					m_hWndStatusBar,
					SB_SETTEXTW,
					static_cast<WPARAM>(i | SBT_OWNERDRAW | SBT_NOBORDERS),
					reinterpret_cast<LPARAM>(m_statusBarPartTexts[i].c_str()));
			}
			::InvalidateRect(m_hWndStatusBar, nullptr, FALSE);
		}

		UpdateEditorVerticalScrollbarVisibility();

		/*std::stringstream ss;
		ss << num_lines << _T(" lines, ") << GetText().size() << _T(" characters. Line: ");
		ss << current_line_number << _T(", column: ") << column << _T(".");
		SendMessage(m_hWndStatusBar, WM_SETTEXT, ss.str().size() + 1, (LPARAM)(ss.str().c_str()));*/
	}

	void CheckFontSize()
	{
		// check the correct menu item
		HMENU hMenu = GetMainMenuHandle();
		CMenuHandle menu(hMenu);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_9, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_10, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_12, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_14, MF_UNCHECKED);
		switch (m_nFontSize)
		{
		case 9:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_9, MF_CHECKED);
			break;
		case 12:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_12, MF_CHECKED);
			break;
		case 14:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_14, MF_CHECKED);
			break;
		case 10:
		default:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_10, MF_CHECKED);
		};
	}

	LRESULT OnViewFontSize(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// get selected font size and check the correct menuitem
		int nOldFontSize = m_nFontSize;

		switch (wID)
		{
		case ID_VIEW_FONTSIZE_9:
			m_nFontSize = 9;
			break;
		case ID_VIEW_FONTSIZE_12:
			m_nFontSize = 12;
			break;
		case ID_VIEW_FONTSIZE_14:
			m_nFontSize = 14;
			break;
		case ID_VIEW_FONTSIZE_10:
		default:
			m_nFontSize = 10;
		}
		CheckFontSize();

		CClientDC dc(*this);
		m_fontEdit.DeleteObject();
		m_fontEdit.CreateFont(
			-MulDiv(m_nFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			utf8_to_wstring(m_strFontName).c_str());

		ATLASSERT(m_fontEdit);
		m_view.SetFont(m_fontEdit);

		// set left and right margins
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
		auto margin = MulDiv(20, dpi, 96);
		m_view.SetMargins(margin, margin);

		// Reset text to force realignment to margins without creating fake edit changes.
		const std::string currentText = GetText();
		if (!currentText.empty())
		{
			SetViewTextWithoutTracking(currentText);
		}

		if (m_nFontSize != nOldFontSize)
		{
			m_bTraitsChanged = true;
		}
		return 0;
	}

	void CheckFontTypeFace()
	{
		// check the correct menu item
		HMENU hMenu = GetMainMenuHandle();
		CMenuHandle menu(hMenu);
		menu.CheckMenuItem(ID_FONT_ARIAL, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_COURIER_NEW, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_TAHOMA, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_VERDANA, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_CONSOLAS, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_CASCADIA_CODE, MF_UNCHECKED);
		if (m_strFontName == "Arial")
		{
			menu.CheckMenuItem(ID_FONT_ARIAL, MF_CHECKED);
		}
		else if (m_strFontName == "Courier New")
		{
			menu.CheckMenuItem(ID_FONT_COURIER_NEW, MF_CHECKED);
		}
		else if (m_strFontName == "Tahoma")
		{
			menu.CheckMenuItem(ID_FONT_TAHOMA, MF_CHECKED);
		}
		else if (m_strFontName == "Verdana")
		{
			menu.CheckMenuItem(ID_FONT_VERDANA, MF_CHECKED);
		}
		else if (m_strFontName == "Lucida Console")
		{
			menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_CHECKED);
		}
		else if (m_strFontName == "Consolas")
		{
			menu.CheckMenuItem(ID_FONT_CONSOLAS, MF_CHECKED);
		}
		else if (m_strFontName == "Cascadia Code")
		{
			menu.CheckMenuItem(ID_FONT_CASCADIA_CODE, MF_CHECKED);
		}
		else
		{
			menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_CHECKED);
		}
	}

	LRESULT OnViewFontTypeFace(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// get selected font name and check the correct menu item
		std::string strOldFontName = m_strFontName;
		switch (wID)
		{
		case ID_FONT_ARIAL:
			m_strFontName = NAME_FONT_ARIAL;
			break;
		case ID_FONT_COURIER_NEW:
			m_strFontName = NAME_FONT_COURIER_NEW;
			break;
		case ID_FONT_TAHOMA:
			m_strFontName = NAME_FONT_TAHOMA;
			break;
		case ID_FONT_VERDANA:
			m_strFontName = NAME_FONT_VERDANA;
			break;
		case ID_FONT_LUCIDA_CONSOLE:
			m_strFontName = NAME_FONT_LUCIDA_CONSOLE;
			break;
		case ID_FONT_CASCADIA_CODE:
			m_strFontName = NAME_FONT_CASCADIA_CODE;
			break;
		case ID_FONT_CONSOLAS:
		default:
			wID = ID_FONT_CONSOLAS;
			m_strFontName = NAME_FONT_CONSOLAS;
		}
		CheckFontTypeFace();

		CClientDC dc(*this);
		m_fontEdit.DeleteObject();
		m_fontEdit.CreateFont(
			-MulDiv(m_nFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			utf8_to_wstring(m_strFontName).c_str());

		ATLASSERT(m_fontEdit);
		m_view.SetFont(m_fontEdit);

		// set left and right margins
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
		auto margin = MulDiv(20, dpi, 96);
		m_view.SetMargins(margin, margin);
		
		// Reset text to force realignment to margins without creating fake edit changes.
		const std::string currentText = GetText();
		if (!currentText.empty())
		{
			SetViewTextWithoutTracking(currentText);
		}

		if (strOldFontName != m_strFontName)
		{
			m_bTraitsChanged = true;
		}
		return 0;
	}

	void CheckLanguage(WORD wID)
	{
		// check the correct menu item
		HMENU hMenu = GetMainMenuHandle();
		CMenuHandle menu(hMenu);
		for (const auto& langid : m_languages)
		{
			menu.CheckMenuItem(langid.first, MF_UNCHECKED);
		}
		/*menu.CheckMenuItem(LANG_ENGLISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_GERMAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_FRENCH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_SPANISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_PORTUGUESE, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_ITALIAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_DUTCH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_POLISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_RUSSIAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_SWEDISH, MF_UNCHECKED);*/
		menu.CheckMenuItem(wID, MF_CHECKED);
	}

	void UpdateThemeMenuChecks()
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		CMenuHandle menu(hMenu);
		const ThemeMode activeMode = ParseThemeMode(m_nThemeMode);
		menu.CheckMenuItem(ID_THEME_SYSTEM, MF_BYCOMMAND | (activeMode == ThemeMode::System ? MF_CHECKED : MF_UNCHECKED));
		menu.CheckMenuItem(ID_THEME_LIGHT, MF_BYCOMMAND | (activeMode == ThemeMode::Light ? MF_CHECKED : MF_UNCHECKED));
		menu.CheckMenuItem(ID_THEME_DARK, MF_BYCOMMAND | (activeMode == ThemeMode::Dark ? MF_CHECKED : MF_UNCHECKED));
	}

	LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// does the edit window still contain the default text? make sure to refresh it with the selected language
		bool bTextUnchanged = GetText() == STR(IDS_WELCOME);

		// set UI language
		const auto langIt = m_languages.find(wID);
		if (langIt == m_languages.end())
		{
			return 0;
		}
		DWORD dwLanguagesSet = 1;
		SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, langIt->second.c_str(), &dwLanguagesSet);
		if (bTextUnchanged)
		{
			m_text = STR(IDS_WELCOME);
			SetViewTextWithoutTracking(m_text);
		}

		// refresh all menu items
		UpdateControlItemTexts();

		// refresh status bar
		UpdateStatusBar();

		// redraw the whole window
		Invalidate(FALSE);
		::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);

		// set checkmark to selected language
		CheckLanguage(wID);
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();

		// remember language to save it on exit
		m_nLanguage = wID;
		m_bTraitsChanged = true;

		return 0;
	}

	LRESULT OnChangeTheme(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const ThemeMode oldMode = ParseThemeMode(m_nThemeMode);
		switch (wID)
		{
		case ID_THEME_LIGHT:
			m_nThemeMode = static_cast<int>(ThemeMode::Light);
			break;
		case ID_THEME_DARK:
			m_nThemeMode = static_cast<int>(ThemeMode::Dark);
			break;
		case ID_THEME_SYSTEM:
		default:
			m_nThemeMode = static_cast<int>(ThemeMode::System);
			break;
		}

		if (oldMode != ParseThemeMode(m_nThemeMode))
		{
			m_bTraitsChanged = true;
		}

		UpdateThemeMenuChecks();
		ApplyTheme();
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();
		return 0;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (ParseThemeMode(m_nThemeMode) == ThemeMode::System)
		{
			ApplyTheme();
		}
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		auto* pMinMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
		if (pMinMaxInfo != nullptr)
		{
			const UINT dpi = GetWindowDpiValue(m_hWnd);
			const int minTrackWidth = MulDiv(kMinimumWindowTrackWidth, static_cast<int>((std::max)(dpi, 96U)), 96);
			const int minTrackHeight = MulDiv(kMinimumWindowTrackHeight, static_cast<int>((std::max)(dpi, 96U)), 96);
			pMinMaxInfo->ptMinTrackSize.x =
				static_cast<LONG>((std::max)(static_cast<int>(pMinMaxInfo->ptMinTrackSize.x), minTrackWidth));
			pMinMaxInfo->ptMinTrackSize.y =
				static_cast<LONG>((std::max)(static_cast<int>(pMinMaxInfo->ptMinTrackSize.y), minTrackHeight));
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcCalcSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		if (wParam == FALSE || lParam == 0)
		{
			bHandled = TRUE;
			return 0;
		}

		auto* const pNcCalcSize = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
		if (pNcCalcSize != nullptr && ::IsZoomed(m_hWnd))
		{
			const UINT dpi = GetWindowDpiValue(m_hWnd);
			const int resizeBorderX = GetResizeBorderThicknessXForDpi(dpi);
			const int resizeBorderY = GetResizeBorderThicknessYForDpi(dpi);
			pNcCalcSize->rgrc[0].left += resizeBorderX;
			pNcCalcSize->rgrc[0].right -= resizeBorderX;
			pNcCalcSize->rgrc[0].top += resizeBorderY;
			pNcCalcSize->rgrc[0].bottom -= resizeBorderY;
		}

		bHandled = TRUE;
		return 0;
	}

	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			bHandled = FALSE;
			return 0;
		}

		const POINT ptScreen{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		POINT ptClient = ptScreen;
		::ScreenToClient(m_hWnd, &ptClient);

		const int resizeHit = HitTestWindowResizeEdges(ptClient, true);
		if (resizeHit != HTCLIENT)
		{
			bHandled = TRUE;
			return resizeHit;
		}

		if (HitTestTitleBarButton(ptClient) >= 0)
		{
			bHandled = TRUE;
			return HTCLIENT;
		}
		if (IsPointInMenuBar(ptClient))
		{
			bHandled = TRUE;
			return HTCLIENT;
		}
		if (IsPointInTitleBar(ptClient))
		{
			bHandled = TRUE;
			return HTCAPTION;
		}

		bHandled = TRUE;
		return HTCLIENT;
	}

	LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (!UseCustomTitleBar())
		{
			InvalidateTopBar();
			bHandled = FALSE;
			return 0;
		}

		bHandled = TRUE;
		InvalidateTopBar();
		return ::DefWindowProcW(m_hWnd, WM_NCACTIVATE, wParam, static_cast<LPARAM>(-1));
	}

	LRESULT OnCtlColorEdit(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		HWND hwndChild = reinterpret_cast<HWND>(lParam);
		if (hwndChild == m_view.m_hWnd && m_viewBackgroundBrush.m_hBrush)
		{
			::SetTextColor(hdc, m_viewTextColor);
			::SetBkColor(hdc, m_viewBackgroundColor);
			return reinterpret_cast<LRESULT>(m_viewBackgroundBrush.m_hBrush);
		}
		if ((hwndChild == m_hFindPanelFindEdit || hwndChild == m_hFindPanelReplaceEdit) &&
			m_findPanelInputBrush.m_hBrush)
		{
			::SetTextColor(hdc, m_findPanelButtonTextColor);
			::SetBkColor(hdc, m_findPanelInputBackgroundColor);
			return reinterpret_cast<LRESULT>(m_findPanelInputBrush.m_hBrush);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC hdc = reinterpret_cast<HDC>(wParam);
		HWND hwndChild = reinterpret_cast<HWND>(lParam);
		if (hwndChild == m_hWndStatusBar && m_statusBarBackgroundBrush.m_hBrush)
		{
			::SetTextColor(hdc, m_statusBarTextColor);
			::SetBkColor(hdc, m_statusBarBackgroundColor);
			return reinterpret_cast<LRESULT>(m_statusBarBackgroundBrush.m_hBrush);
		}
		if (hwndChild == m_hFindPanelSearchIcon && m_findPanelInputBrush.m_hBrush)
		{
			::SetTextColor(hdc, m_isDarkThemeApplied ? RGB(180, 180, 180) : RGB(108, 108, 108));
			::SetBkMode(hdc, TRANSPARENT);
			::SetBkColor(hdc, m_findPanelInputBackgroundColor);
			return reinterpret_cast<LRESULT>(m_findPanelInputBrush.m_hBrush);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const auto* pDrawItem = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
		if (pDrawItem == nullptr)
		{
			bHandled = FALSE;
			return 0;
		}
		if (DrawFindPanelButton(pDrawItem))
		{
			bHandled = TRUE;
			return TRUE;
		}
		if (m_hWndStatusBar == nullptr)
		{
			bHandled = FALSE;
			return 0;
		}
		const UINT statusBarCtlId = static_cast<UINT>(::GetDlgCtrlID(m_hWndStatusBar));
		const bool isStatusDraw =
			pDrawItem->hwndItem == m_hWndStatusBar ||
			pDrawItem->CtlID == statusBarCtlId;
		if (!isStatusDraw)
		{
			bHandled = FALSE;
			return 0;
		}
		const RECT rcItem = pDrawItem->rcItem;
		const int width = rcItem.right - rcItem.left;
		const int height = rcItem.bottom - rcItem.top;
		if (width <= 0 || height <= 0)
		{
			bHandled = TRUE;
			return TRUE;
		}

		const int partIndex = static_cast<int>(pDrawItem->itemID);
		const wchar_t* renderText = m_statusBarText.c_str();
		if (partIndex >= 0 && partIndex < kStatusBarParts)
		{
			renderText = m_statusBarPartTexts[partIndex].c_str();
		}

		const auto drawStatusPart = [&](HDC targetHdc, const RECT& rcTarget)
			{
				if (m_statusBarBackgroundBrush.m_hBrush)
				{
					::FillRect(targetHdc, &rcTarget, m_statusBarBackgroundBrush.m_hBrush);
				}
				else
				{
					CBrush statusBrush;
					statusBrush.CreateSolidBrush(m_statusBarBackgroundColor);
					::FillRect(targetHdc, &rcTarget, statusBrush);
				}

				if ((rcTarget.bottom - rcTarget.top) > 0)
				{
					RECT rcHardDivider{
						rcTarget.left,
						rcTarget.top,
						rcTarget.right,
						rcTarget.top + 1
					};
					CBrush hardDividerBrush;
					hardDividerBrush.CreateSolidBrush(m_statusBarDividerColor);
					::FillRect(targetHdc, &rcHardDivider, hardDividerBrush);

					if ((rcTarget.bottom - rcTarget.top) > 1)
					{
						RECT rcSoftDivider{
							rcTarget.left,
							rcTarget.top + 1,
							rcTarget.right,
							rcTarget.top + 2
						};
						CBrush softDividerBrush;
						const BYTE softAlpha = m_isDarkThemeApplied ? 96 : 118;
						softDividerBrush.CreateSolidBrush(BlendColors(m_statusBarBackgroundColor, m_statusBarDividerColor, softAlpha));
						::FillRect(targetHdc, &rcSoftDivider, softDividerBrush);
					}
				}

				if (pDrawItem->itemID > 0)
				{
					CBrush dividerBrush;
					dividerBrush.CreateSolidBrush(m_statusBarDividerColor);
					RECT rcDivider{ rcTarget.left, rcTarget.top + 2, rcTarget.left + 1, rcTarget.bottom - 2 };
					::FillRect(targetHdc, &rcDivider, dividerBrush);
				}

				HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(targetHdc, GetUiFontHandle()));
				::SetBkMode(targetHdc, TRANSPARENT);
				::SetTextColor(targetHdc, m_statusBarTextColor);
				RECT rcText = rcTarget;
				rcText.left += 8;
				::DrawTextW(targetHdc, renderText, -1, &rcText, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
				if (hOldFont != nullptr)
				{
					::SelectObject(targetHdc, hOldFont);
				}
			};

		HDC hdc = pDrawItem->hDC;
		POINT viewportOrigin{};
		::GetViewportOrgEx(hdc, &viewportOrigin);
		const int destX = rcItem.left - viewportOrigin.x;
		const int destY = rcItem.top - viewportOrigin.y;

		HDC hdcMem = ::CreateCompatibleDC(hdc);
		HBITMAP hbmMem = nullptr;
		HGDIOBJ hOldBitmap = nullptr;
		bool didBufferedPaint = false;

		if (hdcMem != nullptr)
		{
			hbmMem = ::CreateCompatibleBitmap(hdc, width, height);
			if (hbmMem != nullptr)
			{
				hOldBitmap = ::SelectObject(hdcMem, hbmMem);
				const RECT rcLocal{ 0, 0, width, height };
				drawStatusPart(hdcMem, rcLocal);
				::BitBlt(hdc, destX, destY, width, height, hdcMem, 0, 0, SRCCOPY);
				didBufferedPaint = true;
			}
		}

		if (!didBufferedPaint)
		{
			drawStatusPart(hdc, rcItem);
		}

		if (hOldBitmap != nullptr)
		{
			::SelectObject(hdcMem, hOldBitmap);
		}
		if (hbmMem != nullptr)
		{
			::DeleteObject(hbmMem);
		}
		if (hdcMem != nullptr)
		{
			::DeleteDC(hdcMem);
		}

		bHandled = TRUE;
		return TRUE;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		UNREFERENCED_PARAMETER(wParam);
		return TRUE;
	}

	const int GetLanguage() const
	{
		return m_nLanguage;
	}

	void SetLanguage(int langid)
	{
		// set UI language
		m_nLanguage = langid;
		if (langid != 0)
		{
			const auto langIt = m_languages.find(static_cast<WORD>(langid));
			if (langIt != m_languages.end())
			{
				DWORD dwLanguagesSet = 1;
				SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, langIt->second.c_str(), &dwLanguagesSet);
			}
		}
	}

		LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			HDROP hDrop = reinterpret_cast<HDROP>(wParam);
			const UINT uFileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
			const int nResult = Utils::MessageBox(*this, WSTR(IDS_ASK_CONVERT_FILES), MB_YESNO | MB_ICONQUESTION);
			if (nResult == IDYES)
			{
				int nConverted = 0;
				std::string encryptPassword;
				for (UINT uIndex = 0; uIndex < uFileCount; ++uIndex)
				{
					const UINT uLength = DragQueryFileW(hDrop, uIndex, nullptr, 0);
					std::wstring fileBuffer(static_cast<size_t>(uLength) + 1, L'\0');
					DragQueryFileW(hDrop, uIndex, fileBuffer.data(), uLength + 1);
					const std::string filename = wstring_to_utf8(fileBuffer.data());

					if (HasExtension(filename, ".txt"))
					{
						std::string newfilename = filename.substr(0, filename.size() - 4);
						newfilename += ".exe";
						std::string text;
						std::string password;
						if (LoadTextFromFile(filename, text, password) && SaveTextToFile(newfilename, text, encryptPassword))
						{
							++nConverted;
						}
					}
					else
					{
						std::array<char, 512> message{};
						sprintf_s(message.data(), message.size(), STR(IDS_CANT_CONVERT).c_str(), filename.c_str());
						Utils::MessageBox(*this, utf8_to_wstring(message.data()), MB_OK | MB_ICONERROR);
					}
				}

				if (nConverted)
				{
					std::array<char, 256> message{};
					sprintf_s(message.data(), message.size(), STR(IDS_CONVERT_DONE).c_str(), nConverted);
					Utils::MessageBox(*this, utf8_to_wstring(message.data()), MB_OK | MB_ICONINFORMATION);
				}
			}

			DragFinish(hDrop);
			return 0;
		}

	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const std::string currentText = GetText();
		if (currentText == m_text && !m_bTraitsChanged)
		{
			return 0;
		}

		if (!currentText.empty() && m_password.empty())
		{
			m_password = GetNewPasswordDlg(*this);
			if (m_password.empty())
			{
				return 0;
			}
		}

		m_text = currentText;
		m_view.SetModify(FALSE);
		UpdateStatusBar();
		return 0;
	}

	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		std::string encryptPassword;

		CFileDialog dlg(FALSE, _T("exe"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, *this);
		if (dlg.DoModal() == IDOK)
		{
			std::string text;
			text = GetText();
			if (SaveTextToFile(wstring_to_utf8(dlg.m_szFileName), text, encryptPassword))
			{
				//m_text = text;
			}
		}

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		const std::string text = GetText();
		const bool textChanged = (m_text != text);
		if (textChanged)
		{
			int nResult = Utils::MessageBox(*this, WSTR(IDS_SAVE_CHANGES), MB_YESNOCANCEL | MB_ICONQUESTION);
			if (nResult == IDCANCEL)
			{
				return FALSE;
			}

			bHandled = FALSE;

			if (nResult == IDNO)
			{
				m_bTraitsChanged = false;
				return FALSE;
			}

			if (text.empty())
			{
				m_text = text;
				return FALSE;
			}

			if (m_password.empty())
			{
				m_password = GetNewPasswordDlg(*this);
				if (m_password.empty())
				{
					bHandled = TRUE;
					return FALSE;
				}
			}
		}

		bHandled = FALSE;
		m_text = text;
		m_view.SetModify(FALSE);
		CaptureCurrentWindowSizeIfRestored();
		SaveWindowSizeToRegistry();

		return FALSE;
	}
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int m_nOldWindowSizeX = m_nWindowSizeX;
		int m_nOldWindowSizeY = m_nWindowSizeY;
		if (wParam == SIZE_RESTORED)
		{
			RECT rc{};
			GetWindowRect(&rc);
			m_nWindowSizeX = rc.right - rc.left;
			m_nWindowSizeY = rc.bottom - rc.top;
			ClampWindowSizeToCurrentWorkArea();
		}
		bHandled = TRUE;
		if (m_nWindowSizeX != m_nOldWindowSizeX || m_nWindowSizeY != m_nOldWindowSizeY)
		{
			m_bTraitsChanged = true;
			UpdateStatusBar();
			if (wParam == SIZE_RESTORED)
			{
				SaveWindowSizeToRegistry();
			}
		}
		if (wParam != SIZE_MINIMIZED)
		{
			const UINT currentSizeType = static_cast<UINT>(wParam);
			if (currentSizeType != m_lastSizeMessageType)
			{
				m_lastSizeMessageType = currentSizeType;
				UpdateCustomChromeFrame();
			}
			LayoutMainControls();
		}
		else
		{
			m_lastSizeMessageType = static_cast<UINT>(wParam);
		}
		return 0;
	}

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		UpdateCustomChromeFrame();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StopTopBarAnimationTimer();
		if (m_pCurrentFindReplaceDialog != nullptr && ::IsWindow(m_pCurrentFindReplaceDialog->m_hWnd))
		{
			DetachFindDialogSubclass(m_pCurrentFindReplaceDialog->m_hWnd);
		}
		DetachChromeResizeSubclassesForFindPanelControls();
		DetachChromeResizeSubclass(m_view.m_hWnd);
		DetachChromeResizeSubclass(m_hWndStatusBar);
		CaptureCurrentWindowSizeIfRestored();
		SaveWindowSizeToRegistry();
		if (m_viewBackgroundBrush.m_hBrush)
		{
			m_viewBackgroundBrush.DeleteObject();
		}
		if (m_frameBackgroundBrush.m_hBrush)
		{
			m_frameBackgroundBrush.DeleteObject();
		}
		if (m_statusBarBackgroundBrush.m_hBrush)
		{
			m_statusBarBackgroundBrush.DeleteObject();
		}
		if (m_findPanelInputBrush.m_hBrush)
		{
			m_findPanelInputBrush.DeleteObject();
		}
		if (m_fontUi.m_hFont)
		{
			m_fontUi.DeleteObject();
		}
		if (m_fontToolbarIcons.m_hFont)
		{
			m_fontToolbarIcons.DeleteObject();
		}
		if (m_fontTitleBarIcons.m_hFont)
		{
			m_fontTitleBarIcons.DeleteObject();
		}
		if (m_fontFindPanelText.m_hFont)
		{
			m_fontFindPanelText.DeleteObject();
		}
		if (m_fontFindPanelIcons.m_hFont)
		{
			m_fontFindPanelIcons.DeleteObject();
		}
		if (m_hMainMenu != nullptr && ::GetMenu(m_hWnd) == nullptr)
		{
			::DestroyMenu(m_hMainMenu);
			m_hMainMenu = nullptr;
		}
		bHandled = FALSE;
		return 0;
	}

		LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			m_dwSearchFlags = FR_DOWN;
			m_pCurrentFindReplaceDialog = nullptr;
			m_findDialogOriginalProc = nullptr;

			DragAcceptFiles(TRUE);

			m_hWndStatusBar = ::CreateWindowExW(
				0,
				STATUSCLASSNAMEW,
				nullptr,
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM | CCS_NODIVIDER,
				0,
				0,
				0,
				0,
				m_hWnd,
				reinterpret_cast<HMENU>(ATL_IDW_STATUS_BAR),
				_Module.GetModuleInstance(),
				nullptr);
			if (m_hWndStatusBar != nullptr)
			{
				NormalizeStatusBarStyles();
				ApplyStatusBarTheme();
				AttachChromeResizeSubclass(m_hWndStatusBar);
			}
			m_hMainMenu = ::LoadMenuW(_Module.GetResourceInstance(), MAKEINTRESOURCEW(IDR_MAINFRAME));
			if (m_hMainMenu != nullptr)
			{
				::SetMenu(m_hWnd, m_hMainMenu);
			}
			else
			{
				m_hMainMenu = ::GetMenu(m_hWnd);
			}

			m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, 0);
			AttachChromeResizeSubclass(m_view.m_hWnd);
			m_view.SetLimitText(0x7ffffffe); // allow a lot of text to be entered
			m_isWordWrapEnabled = ((::GetWindowLongPtrW(m_view, GWL_STYLE) & WS_HSCROLL) == 0);

			CClientDC dc(*this);
			auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
			auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
			if (!m_hasPreparedInitialWindowSize)
			{
				if (m_nWindowSizeX == DEFAULT_WIDTH && m_nWindowSizeY == DEFAULT_HEIGHT)
				{
					m_nWindowSizeX = MulDiv(m_nWindowSizeX, dpi, 96);
					m_nWindowSizeY = MulDiv(m_nWindowSizeY, dpi, 96);
				}
				m_hasLoadedPersistedWindowSize = LoadWindowSizeFromRegistry();
			}
			ClampWindowSizeToCurrentWorkArea();
			m_hasPreparedInitialWindowSize = true;
			m_nTopTitleBarHeight = MulDiv(46, dpi, 96);
			m_nTopMenuHeight = m_nTopTitleBarHeight;
			m_nToolbarHeight = 0;
			m_nTopBarHeight = m_nTopTitleBarHeight;
			const wchar_t* uiFontFace = IsFontFaceAvailable(L"Segoe UI Variable Text") ? L"Segoe UI Variable Text" : L"Segoe UI";
			if (m_fontUi.m_hFont)
			{
				m_fontUi.DeleteObject();
			}
			m_fontUi.CreateFont(
				-MulDiv(9, dpi, 72),
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY,
				DEFAULT_PITCH,
				uiFontFace);
			if (m_fontToolbarIcons.m_hFont)
			{
				m_fontToolbarIcons.DeleteObject();
			}
			m_fontToolbarIcons.CreateFont(
				-MulDiv(11, dpi, 72),
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY,
				DEFAULT_PITCH,
				L"Segoe MDL2 Assets");
			if (m_fontTitleBarIcons.m_hFont)
			{
				m_fontTitleBarIcons.DeleteObject();
			}
			m_fontTitleBarIcons.CreateFont(
				-MulDiv(8, dpi, 72),
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY,
				DEFAULT_PITCH,
				L"Segoe MDL2 Assets");
			if (m_fontFindPanelText.m_hFont)
			{
				m_fontFindPanelText.DeleteObject();
			}
			m_fontFindPanelText.CreateFont(
				-MulDiv(11, dpi, 72),
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY,
				DEFAULT_PITCH,
				uiFontFace);
			if (m_fontFindPanelIcons.m_hFont)
			{
				m_fontFindPanelIcons.DeleteObject();
			}
			m_fontFindPanelIcons.CreateFont(
				-MulDiv(12, dpi, 72),
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY,
				DEFAULT_PITCH,
				L"Segoe MDL2 Assets");
			if (m_hWndStatusBar && m_fontUi.m_hFont != nullptr)
			{
				::SendMessageW(m_hWndStatusBar, WM_SETFONT, reinterpret_cast<WPARAM>(static_cast<HFONT>(m_fontUi)), TRUE);
			}
			EnsureFindPanelControlsCreated();
			ApplyFindPanelFontsAndTexts();
			UpdateFindPanelVisibility();
			m_fontEdit.CreateFont(
				-dpi_factor_font,
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH,
				utf8_to_wstring(m_strFontName).c_str());

			ATLASSERT(m_fontEdit);
			m_view.SetFont(m_fontEdit);

			auto margin = MulDiv(20, dpi, 96);
			m_view.SetMargins(margin, margin);

			if (!m_text.empty())
			{
				SetViewTextWithoutTracking(m_text);
			}

			std::array<wchar_t, MAX_PATH> modulePath{};
			const DWORD modulePathLength = ::GetModuleFileNameW(Utils::GetModuleHandle(), modulePath.data(), static_cast<DWORD>(modulePath.size()));
			if (modulePathLength == 0 || modulePathLength >= modulePath.size())
			{
				return -1;
			}

			std::string title = wstring_to_utf8(modulePath.data());
			const size_t nSlashPos = title.find_last_of('\\');
			if (nSlashPos != std::string::npos)
			{
				title = title.substr(nSlashPos + 1);
			}
			const size_t nDotPos = title.find_last_of('.');
			if (nDotPos != std::string::npos)
			{
				title = title.substr(0, nDotPos);
			}

			const std::string windowTitle = title;

			// register object for message filtering and idle updates
			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(this);
			pLoop->AddIdleHandler(this);

			m_currentBuffer.m_strText = GetText();
			m_view.GetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);
			m_view.SetModify(FALSE);

			UpdateControlItemTexts();

			// get default UI language, set checkmark in menu
			if (m_nLanguage == 0)
			{
				LANGID langid = GetUserDefaultUILanguage();
				m_nLanguage = langid & 0b0000000111111111;
			}
			CheckLanguage(static_cast<WORD>(m_nLanguage));
			ApplyTheme();
			RefreshToolbarLayout();
			RefreshTopMenuLayout();
			if (::GetMenu(m_hWnd) != nullptr)
			{
				::SetMenu(m_hWnd, nullptr);
			}

			UpdateWindowFrameStyle();
			UpdateCustomChromeFrame();
			SetWindowPos(NULL, 0, 0, m_nWindowSizeX, m_nWindowSizeY, SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
			CenterWindow();
			LayoutMainControls();

			m_view.SetFocus();

			m_view.SetSel(m_currentBuffer.m_nStartChar + 2, m_currentBuffer.m_nStartChar + 2);

			SetWindowText(utf8_to_wstring(windowTitle).c_str());
			RefreshToolbarLayout();
			RefreshTopMenuLayout();
			InvalidateTopBar();

			UpdateStatusBar();
			::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);

			return 0;
		}

	void UpdateControlItemTexts()
	{
		// convert menus and menu items from utf-8 to unicode
		ChangeMenuText(0, WSTR(IDS_MENU_FILE));
		ChangeMenuText(1, WSTR(IDS_MENU_EDIT));
		ChangeMenuText(2, WSTR(IDS_MENU_VIEW));
		ChangeMenuText(3, WSTR(IDS_MENU_HELP));
		ChangeSubMenuText(2, 0, WSTR(IDS_MENU_FONT));
		ChangeSubMenuText(2, 1, WSTR(IDS_MENU_FONT_SIZE));
		ChangeSubMenuText(2, 2, WSTR(IDS_MENU_LANGUAGE));
		ChangeMenuItemText(57604, WSTR(IDS_MENUITEM_SAVEAS));
		ChangeMenuItemText(ID_FILE_CHANGEPASSWORD, WSTR(IDS_CHANGE_PASSWORD));
		ChangeMenuItemText(ID_APP_EXIT, WSTR(IDS_MENUITEM_QUIT));
		ChangeMenuItemText(ID_MENU_EDIT, WSTR(IDS_MENU_EDIT));
		ChangeMenuItemText(ID_EDIT_UNDO, WSTR(IDS_MENUITEM_UNDO));
		ChangeMenuItemText(ID_EDIT_CUT, WSTR(IDS_MENUITEM_CUT));
		ChangeMenuItemText(ID_EDIT_COPY, WSTR(IDS_MENUITEM_COPY));
		ChangeMenuItemText(ID_EDIT_PASTE, WSTR(IDS_MENUITEM_PASTE));
		ChangeMenuItemText(ID_EDIT_FIND, WSTR(IDS_MENUITEM_FIND));
		ChangeMenuItemText(ID_EDIT_FINDNEXT, WSTR(IDS_MENUITEM_FINDNEXT));
		ChangeMenuItemText(ID_EDIT_SELECT_ALL, WSTR(IDS_MENUITEM_SELECT_ALL));
		ChangeMenuItemText(ID_MENU_VIEW, WSTR(IDS_MENU_VIEW));
		ChangeMenuItemText(ID_MENU_FONT, WSTR(IDS_MENU_FONT));
		ChangeMenuItemText(ID_MENU_FONT_SIZE, WSTR(IDS_MENU_FONT_SIZE));
		ChangeMenuItemText(ID_MENU_HELP, WSTR(IDS_MENU_HELP));
		ChangeMenuItemText(ID_APP_ABOUT, WSTR(IDS_ABOUT_TITLE));
		ConfigureThemeMenu();
		ConfigureEditSearchPlaceholders();
		UpdateThemeMenuChecks();
		ConfigureEncryptionMenu();
		UpdateEncryptionMenuChecks();
		ApplyFindPanelFontsAndTexts();
		UpdateFindPanelButtonsEnabledState();
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
	}

	void ConfigureEditSearchPlaceholders()
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		HMENU hEditMenu = ::GetSubMenu(hMenu, 1);
		if (hEditMenu == nullptr)
		{
			return;
		}

		::RemoveMenu(hEditMenu, kMenuCmdFindPreviousPlaceholder, MF_BYCOMMAND);
		::RemoveMenu(hEditMenu, kMenuCmdReplacePlaceholder, MF_BYCOMMAND);

		const int itemCount = ::GetMenuItemCount(hEditMenu);
		int findNextPos = -1;
		for (int pos = 0; pos < itemCount; ++pos)
		{
			if (::GetMenuItemID(hEditMenu, pos) == ID_EDIT_FINDNEXT)
			{
				findNextPos = pos;
				break;
			}
		}
		if (findNextPos < 0)
		{
			return;
		}

		const std::wstring findPrevCaption = GetFindPreviousMenuCaption();
		const std::wstring replaceCaption = GetReplaceMenuCaption();

		::InsertMenuW(
			hEditMenu,
			findNextPos + 1,
			MF_BYPOSITION | MF_STRING | MF_DISABLED | MF_GRAYED,
			kMenuCmdFindPreviousPlaceholder,
			findPrevCaption.c_str());
		::InsertMenuW(
			hEditMenu,
			findNextPos + 2,
			MF_BYPOSITION | MF_STRING | MF_DISABLED | MF_GRAYED,
			kMenuCmdReplacePlaceholder,
			replaceCaption.c_str());
	}

	void ConfigureThemeMenu()
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		HMENU hViewMenu = ::GetSubMenu(hMenu, 2);
		if (hViewMenu == nullptr)
		{
			return;
		}

		const UINT themeSystemState = ::GetMenuState(hViewMenu, ID_THEME_SYSTEM, MF_BYCOMMAND);
		if (themeSystemState == static_cast<UINT>(-1))
		{
			HMENU hThemeMenu = ::CreatePopupMenu();
			if (hThemeMenu == nullptr)
			{
				return;
			}

			const std::wstring systemCaption = GetThemeSystemCaption();
			const std::wstring lightCaption = GetThemeLightCaption();
			const std::wstring darkCaption = GetThemeDarkCaption();
			const std::wstring themeCaption = GetThemeMenuCaption();
			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_SYSTEM, systemCaption.c_str());
			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_LIGHT, lightCaption.c_str());
			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_DARK, darkCaption.c_str());
			::AppendMenuW(hViewMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hThemeMenu), themeCaption.c_str());
		}
	}

	void ConfigureEncryptionMenu()
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		const std::array<UINT, 4> menuItemsToRemove{
			ID_STEGANOS_PRIVACYSUITE,
			ID_STEGANOS_ONLINESHIELD,
			ID_STEGANOS_LATESTOFFERS,
			ID_STEGANOS_CHECKFORLOCKNOTEUPDATES
		};
		for (const UINT id : menuItemsToRemove)
		{
			RemoveMenu(hMenu, id, MF_BYCOMMAND);
		}

		for (int topIndex = 0; topIndex < GetMenuItemCount(hMenu); ++topIndex)
		{
			HMENU hSubMenu = GetSubMenu(hMenu, topIndex);
			if (hSubMenu == nullptr)
			{
				continue;
			}

			const bool hasEncryptionItems =
				GetMenuState(hSubMenu, ID_STEGANOS_PASSWORD_MANAGER, MF_BYCOMMAND) != static_cast<UINT>(-1) ||
				GetMenuState(hSubMenu, ID_STEGANOS_SAFE, MF_BYCOMMAND) != static_cast<UINT>(-1);
			if (!hasEncryptionItems)
			{
				continue;
			}

			// Reuse the former "Steganos" submenu as a local encryption settings menu.
			MENUITEMINFOW topMenuInfo{ sizeof(topMenuInfo) };
			topMenuInfo.fMask = MIIM_STRING;
			const std::wstring encryptionMenuCaption = GetEncryptionMenuCaption();
			topMenuInfo.dwTypeData = const_cast<LPWSTR>(encryptionMenuCaption.c_str());
			SetMenuItemInfoW(hMenu, topIndex, TRUE, &topMenuInfo);

			for (int pos = GetMenuItemCount(hSubMenu) - 1; pos >= 0; --pos)
			{
				if ((GetMenuState(hSubMenu, pos, MF_BYPOSITION) & MF_SEPARATOR) != 0)
				{
					RemoveMenu(hSubMenu, pos, MF_BYPOSITION);
				}
			}
			break;
		}

		ChangeMenuItemText(ID_STEGANOS_PASSWORD_MANAGER, GetEncryptionModernCaption());
		ChangeMenuItemText(ID_STEGANOS_SAFE, GetEncryptionCompatibilityCaption());
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();
	}

	void UpdateEncryptionMenuChecks()
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		CMenuHandle menu(hMenu);
		menu.CheckMenuItem(
			ID_STEGANOS_PASSWORD_MANAGER,
			MF_BYCOMMAND | (m_kdfMode == AESLayer::KdfMode::Scrypt ? MF_CHECKED : MF_UNCHECKED));
		menu.CheckMenuItem(
			ID_STEGANOS_SAFE,
			MF_BYCOMMAND | (m_kdfMode == AESLayer::KdfMode::Pbkdf2Sha256 ? MF_CHECKED : MF_UNCHECKED));
	}

	// change the text of the given menu
	void ChangeMenuText(const UINT& menupos, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		MENUITEMINFOW mii{ sizeof(mii) };
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = const_cast<LPWSTR>(strNewText.c_str());
		SetMenuItemInfoW(hMenu, menupos, TRUE, &mii);
	}

	// change the text of the given menu's submenu
	void ChangeSubMenuText(const UINT& menupos, const UINT& submenupos, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}
		HMENU hSubMenu = GetSubMenu(hMenu, menupos);
		if (hSubMenu == nullptr)
		{
			return;
		}

		MENUITEMINFOW mii{ sizeof(mii) };
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = const_cast<LPWSTR>(strNewText.c_str());
		SetMenuItemInfoW(hSubMenu, submenupos, TRUE, &mii);
	}

	// change the text of the given menu item
	void ChangeMenuItemText(const UINT& id, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr)
		{
			return;
		}

		MENUITEMINFOW mii{ sizeof(mii) };
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = const_cast<LPWSTR>(strNewText.c_str());
		SetMenuItemInfoW(hMenu, id, FALSE, &mii);
	}

	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.Copy();
		return 0;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.Cut();
		return 0;
	}

	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const HWND hFocus = ::GetFocus();
		if (hFocus == m_hFindPanelFindEdit || hFocus == m_hFindPanelReplaceEdit)
		{
			::SendMessageW(hFocus, WM_PASTE, 0, 0);
			return 0;
		}
		if (m_pCurrentFindReplaceDialog &&
			::IsWindow(m_pCurrentFindReplaceDialog->m_hWnd) &&
			::IsWindowVisible(m_pCurrentFindReplaceDialog->m_hWnd))
		{
			m_pCurrentFindReplaceDialog->SendMessageToDescendants(WM_PASTE, 0, 0);
		}
		else
		{
			m_view.Paste();
		}
		return 0;
	}

	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_listUndo.empty())
		{
			UndoBuffer undo = m_listUndo.back();
			m_listUndo.pop_back();
			m_currentBuffer = undo;
			SetViewTextWithoutTracking(m_currentBuffer.m_strText);
			m_view.SetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);
		}
		return 0;
	}

	std::wstring GetControlText(HWND hWndControl) const
	{
		if (hWndControl == nullptr || !::IsWindow(hWndControl))
		{
			return std::wstring{};
		}

		const int textLength = ::GetWindowTextLengthW(hWndControl);
		if (textLength <= 0)
		{
			return std::wstring{};
		}

		std::wstring text(static_cast<size_t>(textLength) + 1, L'\0');
		::GetWindowTextW(hWndControl, text.data(), textLength + 1);
		if (!text.empty() && text.back() == L'\0')
		{
			text.pop_back();
		}
		return text;
	}

	static std::wstring ToLowerWide(std::wstring value)
	{
		std::transform(
			value.begin(),
			value.end(),
			value.begin(),
			[](const wchar_t ch)
			{
				return static_cast<wchar_t>(std::towlower(ch));
			});
		return value;
	}

	void SyncSearchTextFromFindPanel()
	{
		if (m_hFindPanelFindEdit == nullptr)
		{
			return;
		}
		m_strSearchString = wstring_to_utf8(GetControlText(m_hFindPanelFindEdit));
		UpdateFindPanelButtonsEnabledState();
	}

	void SetFindPanelVisible(const bool visible)
	{
		if (visible && m_pCurrentFindReplaceDialog != nullptr && ::IsWindow(m_pCurrentFindReplaceDialog->m_hWnd))
		{
			m_pCurrentFindReplaceDialog->ShowWindow(SW_HIDE);
		}
		if (!visible)
		{
			m_isReplacePanelVisible = false;
		}
		m_isFindPanelVisible = visible;
		UpdateFindPanelVisibility();
		LayoutMainControls();
		Invalidate(FALSE);
	}

	void FocusFindPanelEdit(const bool selectAll)
	{
		if (m_hFindPanelFindEdit == nullptr || !::IsWindow(m_hFindPanelFindEdit))
		{
			return;
		}
		::SetFocus(m_hFindPanelFindEdit);
		if (selectAll)
		{
			::SendMessageW(m_hFindPanelFindEdit, EM_SETSEL, 0, -1);
		}
	}

	void ShowFindPanelOptionsMenu()
	{
		if (m_hFindPanelOptionsButton == nullptr || !::IsWindow(m_hFindPanelOptionsButton))
		{
			return;
		}

		RECT rcButton{};
		::GetWindowRect(m_hFindPanelOptionsButton, &rcButton);
		CMenu menu;
		if (!menu.CreatePopupMenu())
		{
			return;
		}

		const std::wstring matchCaseCaption = GetFindPanelMatchCaseCaption();
		const std::wstring wrapCaption = GetFindPanelWrapCaption();
		menu.AppendMenuW(
			MF_STRING | ((m_dwSearchFlags & FR_MATCHCASE) ? MF_CHECKED : MF_UNCHECKED),
			kFindPanelOptionMatchCaseId,
			matchCaseCaption.c_str());
		menu.AppendMenuW(
			MF_STRING | (m_isFindWrapEnabled ? MF_CHECKED : MF_UNCHECKED),
			kFindPanelOptionWrapId,
			wrapCaption.c_str());

		ApplyPopupMenuTheme();
		const UINT selectedCmd = menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
			rcButton.left,
			rcButton.bottom,
			m_hWnd);
		if (selectedCmd == kFindPanelOptionMatchCaseId)
		{
			m_dwSearchFlags ^= FR_MATCHCASE;
		}
		else if (selectedCmd == kFindPanelOptionWrapId)
		{
			m_isFindWrapEnabled = !m_isFindWrapEnabled;
		}
	}

	std::wstring GetSelectedEditorText() const
	{
		int selStart = 0;
		int selEnd = 0;
		m_view.GetSel(selStart, selEnd);
		if (selEnd <= selStart)
		{
			return std::wstring{};
		}

		std::wstring selected(static_cast<size_t>(selEnd - selStart) + 1, L'\0');
		::SendMessageW(m_view, EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(selected.data()));
		if (!selected.empty() && selected.back() == L'\0')
		{
			selected.pop_back();
		}
		return selected;
	}

	bool IsSelectionEqualToSearch(const std::wstring& searchText) const
	{
		if (searchText.empty())
		{
			return false;
		}

		std::wstring selected = GetSelectedEditorText();
		if (selected.empty())
		{
			return false;
		}
		if (m_dwSearchFlags & FR_MATCHCASE)
		{
			return selected == searchText;
		}
		return ToLowerWide(selected) == ToLowerWide(searchText);
	}

	bool ReplaceCurrentSelection()
	{
		SyncSearchTextFromFindPanel();
		const std::wstring findText = utf8_to_wstring(m_strSearchString);
		if (findText.empty())
		{
			return false;
		}

		if (!IsSelectionEqualToSearch(findText))
		{
			m_dwSearchFlags |= FR_DOWN;
			FindNext();
			if (!IsSelectionEqualToSearch(findText))
			{
				return false;
			}
		}

		const std::wstring replaceText = GetControlText(m_hFindPanelReplaceEdit);
		m_view.ReplaceSel(replaceText.c_str(), TRUE);
		return true;
	}

	void ReplaceAllSelections()
	{
		SyncSearchTextFromFindPanel();
		const std::wstring findText = utf8_to_wstring(m_strSearchString);
		const std::wstring replaceText = GetControlText(m_hFindPanelReplaceEdit);
		if (findText.empty())
		{
			return;
		}

		std::wstring document = utf8_to_wstring(GetText());
		std::wstring lookupDocument = document;
		std::wstring lookupFind = findText;
		std::wstring lookupReplace = replaceText;
		if (!(m_dwSearchFlags & FR_MATCHCASE))
		{
			lookupDocument = ToLowerWide(lookupDocument);
			lookupFind = ToLowerWide(lookupFind);
			lookupReplace = ToLowerWide(lookupReplace);
		}

		size_t pos = 0;
		size_t replacedCount = 0;
		while (true)
		{
			const size_t hit = lookupDocument.find(lookupFind, pos);
			if (hit == std::wstring::npos)
			{
				break;
			}
			document.replace(hit, findText.size(), replaceText);
			lookupDocument.replace(hit, lookupFind.size(), lookupReplace);
			pos = hit + replaceText.size();
			++replacedCount;
		}

		if (replacedCount > 0)
		{
			m_view.SetWindowTextW(document.c_str());
			m_view.SetSel(0, 0);
			UpdateStatusBar();
		}
	}

	LRESULT OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetFindPanelVisible(true);
		ApplyFindPanelFontsAndTexts();
		const std::wstring initialText = utf8_to_wstring(m_strSearchString);
		if (m_hFindPanelFindEdit != nullptr && !initialText.empty() && ::GetWindowTextLengthW(m_hFindPanelFindEdit) == 0)
		{
			::SetWindowTextW(m_hFindPanelFindEdit, initialText.c_str());
		}
		FocusFindPanelEdit(true);
		return 0;
	}

	void FindNext()
	{
		if (m_isFindPanelVisible)
		{
			SyncSearchTextFromFindPanel();
		}
		int nBegin = 0;
		int nEnd = 0;
		std::string text = GetText();
		std::string searchtext = m_strSearchString;
		if (searchtext.empty())
		{
			FocusFindPanelEdit(false);
			return;
		}
		if (!(m_dwSearchFlags & FR_MATCHCASE))
		{
			text = str_tolower(text);
			searchtext = str_tolower(searchtext);
		}
		const std::wstring wtext = utf8_to_wstring(text);
		const std::wstring wsearchtext = utf8_to_wstring(searchtext);
		m_view.GetSel(nBegin, nEnd);

		size_t nIndex = std::wstring::npos;
		if (!(m_dwSearchFlags & FR_DOWN))
		{
			const size_t lookupPos = nBegin > 0 ? static_cast<size_t>(nBegin - 1) : std::wstring::npos;
			nIndex = wtext.rfind(wsearchtext, lookupPos);
			if (nIndex == std::wstring::npos && m_isFindWrapEnabled)
			{
				nIndex = wtext.rfind(wsearchtext);
			}
		}
		else
		{
			nIndex = wtext.find(wsearchtext, static_cast<size_t>(nEnd));
			if (nIndex == std::wstring::npos && m_isFindWrapEnabled)
			{
				nIndex = wtext.find(wsearchtext, 0);
			}
		}
		if (nIndex != std::wstring::npos)
		{
			m_view.SetSel(static_cast<int>(nIndex), static_cast<int>(nIndex + wsearchtext.size()));
		}
		else
		{
			char szErrorMessage[MAX_PATH] = "";
			sprintf_s(szErrorMessage, MAX_PATH, STR(IDS_FIND_NOT_FOUND).c_str(), m_strSearchString.c_str());
			Utils::MessageBox(*this, utf8_to_wstring(szErrorMessage).c_str(), MB_OK | MB_ICONWARNING);
			FocusFindPanelEdit(false);
		}
	}

	LRESULT OnEditFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_isFindPanelVisible)
		{
			BOOL bHandled = FALSE;
			OnEditFind(0, 0, NULL, bHandled);
		}
		m_dwSearchFlags |= FR_DOWN;
		FindNext();
		return 0;
	}

	LRESULT OnFindPanelFindTextChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SyncSearchTextFromFindPanel();
		return 0;
	}

	LRESULT OnFindPanelFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_dwSearchFlags |= FR_DOWN;
		FindNext();
		return 0;
	}

	LRESULT OnFindPanelFindPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_dwSearchFlags &= ~FR_DOWN;
		FindNext();
		return 0;
	}

	LRESULT OnFindPanelToggleReplace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_isReplacePanelVisible = !m_isReplacePanelVisible;
		ApplyFindPanelFontsAndTexts();
		LayoutMainControls();
		if (m_isReplacePanelVisible && m_hFindPanelReplaceEdit != nullptr)
		{
			::SetFocus(m_hFindPanelReplaceEdit);
		}
		else
		{
			FocusFindPanelEdit(false);
		}
		return 0;
	}

	LRESULT OnFindPanelOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowFindPanelOptionsMenu();
		return 0;
	}

	LRESULT OnFindPanelClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetFindPanelVisible(false);
		m_view.SetFocus();
		return 0;
	}

	LRESULT OnFindPanelReplaceOne(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (ReplaceCurrentSelection())
		{
			m_dwSearchFlags |= FR_DOWN;
			FindNext();
		}
		return 0;
	}

	LRESULT OnFindPanelReplaceAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ReplaceAllSelections();
		return 0;
	}

	LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.SetSelAll();
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFindMsgString(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CFindReplaceDialog* pDlgFindReplace = CFindReplaceDialog::GetNotifier(lParam);
		ATLASSERT(m_pCurrentFindReplaceDialog == pDlgFindReplace);

		m_dwSearchFlags = (pDlgFindReplace->m_fr.Flags) & (FR_WHOLEWORD|FR_MATCHCASE|FR_DOWN);
		m_strSearchString = wstring_to_utf8(pDlgFindReplace->GetFindString());

		if (pDlgFindReplace->IsTerminating())
		{
			DetachFindDialogSubclass(pDlgFindReplace->m_hWnd);
			m_pCurrentFindReplaceDialog = nullptr;
		}
		else if (pDlgFindReplace->FindNext())
		{
			FindNext();
		}

		return 0;
	}

	LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		if (hWndCtl != m_view.m_hWnd || m_ignoreEditNotifications)
		{
			return 0;
		}

		if (m_listUndo.size() >= 1000)
		{
			m_listUndo.pop_front();
		}
		m_listUndo.push_back(m_currentBuffer);
		m_currentBuffer.m_strText = GetText();
		m_view.GetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);

		UpdateStatusBar();

		return 0;
	}

		LRESULT OnFileChangePassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!m_password.empty())
			{
				std::string strOldPassword = GetPasswordDlg(*this);
				if (strOldPassword.empty())
				{
					return 0;
				}
				if (!Utils::ConstantTimeEquals(strOldPassword, m_password))
				{
					Utils::MessageBox(*this, WSTR(IDS_INVALID_PASSWORD), MB_OK | MB_ICONERROR);
					return 0;
				}
			}

		std::string strNewPassword = GetNewPasswordDlg(*this);
		if (strNewPassword.empty())
		{
			return 0;
		}

		m_password = strNewPassword;
		Utils::MessageBox(*this, WSTR(IDS_PASSWORD_CHANGED), MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnSetEncryptionMode(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const AESLayer::KdfMode oldMode = m_kdfMode;
		m_kdfMode = (wID == ID_STEGANOS_SAFE)
			? AESLayer::KdfMode::Pbkdf2Sha256
			: AESLayer::KdfMode::Scrypt;

		if (m_kdfMode != oldMode)
		{
			m_bTraitsChanged = true;
		}
		UpdateEncryptionMenuChecks();
		return 0;
	}
};

