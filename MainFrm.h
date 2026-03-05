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
	static constexpr int kStoredWindowMinWidth = 320;
	static constexpr int kStoredWindowMinHeight = 240;
	COLORREF m_viewTextColor{ ::GetSysColor(COLOR_WINDOWTEXT) };
	COLORREF m_viewBackgroundColor{ ::GetSysColor(COLOR_WINDOW) };
	COLORREF m_frameBackgroundColor{ ::GetSysColor(COLOR_3DFACE) };
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
	int m_nTopBarHeight{ 42 };
	int m_nTopMenuHeight{ 42 };
	int m_nToolbarHeight{ 0 };
	std::vector<int> m_topMenuPopupPositions;
	std::vector<RECT> m_topMenuButtonRects;
	std::vector<std::wstring> m_topMenuCaptions;
	std::vector<int> m_topMenuOverflowPopupPositions;
	std::vector<std::wstring> m_topMenuOverflowCaptions;
	std::vector<UINT> m_toolbarCommandIds;
	std::vector<RECT> m_toolbarButtonRects;
	std::vector<std::wstring> m_toolbarButtonCaptions;
	RECT m_topBarTitleRect{};
	std::wstring m_topBarTitleText;
	int m_topMenuHoveredIndex{ -1 };
	int m_topMenuPressedIndex{ -1 };
	int m_toolbarHoveredIndex{ -1 };
	int m_toolbarPressedIndex{ -1 };
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
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColorEdit)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
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
		UpdateStatusBar();
		m_view.Invalidate();
	}

	void UpdateEditorVerticalScrollbarVisibility() const
	{
		if (!m_view.m_hWnd || !::IsWindow(m_view.m_hWnd))
		{
			return;
		}

		RECT rcClient{};
		::GetClientRect(m_view.m_hWnd, &rcClient);
		const int clientHeight = rcClient.bottom - rcClient.top;
		if (clientHeight <= 0)
		{
			::ShowScrollBar(m_view.m_hWnd, SB_VERT, FALSE);
			return;
		}

		HDC hdc = ::GetDC(m_view.m_hWnd);
		if (hdc == nullptr)
		{
			return;
		}

		const HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(m_view.m_hWnd, WM_GETFONT, 0, 0));
		HFONT hOldFont = nullptr;
		if (hFont != nullptr)
		{
			hOldFont = reinterpret_cast<HFONT>(::SelectObject(hdc, hFont));
		}

		TEXTMETRICW tm{};
		::GetTextMetricsW(hdc, &tm);
		if (hOldFont != nullptr)
		{
			::SelectObject(hdc, hOldFont);
		}
		::ReleaseDC(m_view.m_hWnd, hdc);

		const int lineHeight = (std::max)(1, static_cast<int>(tm.tmHeight + tm.tmExternalLeading));
		const int visibleLines = (std::max)(1, clientHeight / lineHeight);
		const int lineCount = static_cast<int>(::SendMessageW(m_view.m_hWnd, EM_GETLINECOUNT, 0, 0));
		const BOOL showScrollbar = (lineCount > visibleLines) ? TRUE : FALSE;
		::ShowScrollBar(m_view.m_hWnd, SB_VERT, showScrollbar);
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
			return L"Р СѓСЃСЃРєРёР№";
		case LANG_GERMAN:
			return L"Deutsch";
		case LANG_FRENCH:
			return L"FranГ§ais";
		case LANG_SPANISH:
			return L"EspaГ±ol";
		case LANG_ITALIAN:
			return L"Italiano";
		case LANG_PORTUGUESE:
			return L"PortuguГЄs";
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
			dwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
			dwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &useDark, sizeof(useDark));
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
			dwmSetWindowAttribute(hWndTarget, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
			dwmSetWindowAttribute(hWndTarget, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &useDark, sizeof(useDark));
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
			m_topBarBackgroundColor = RGB(32, 32, 32);
			m_topBarTextColor = RGB(232, 232, 232);
			m_topBarHoverColor = RGB(57, 57, 57);
			m_topBarPressedColor = RGB(72, 72, 72);
			m_topBarDividerColor = RGB(62, 62, 62);
			m_toolbarBackgroundColor = RGB(32, 32, 32);
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
			m_frameBackgroundColor = RGB(246, 246, 246);
			m_topBarBackgroundColor = RGB(245, 247, 250);
			m_topBarTextColor = RGB(26, 26, 26);
			m_topBarHoverColor = RGB(225, 231, 239);
			m_topBarPressedColor = RGB(212, 220, 231);
			m_topBarDividerColor = RGB(207, 214, 224);
			m_toolbarBackgroundColor = RGB(245, 247, 250);
			m_toolbarHoverColor = RGB(221, 228, 238);
			m_toolbarPressedColor = RGB(207, 216, 228);
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

	std::wstring GetTopBarTitleText() const
	{
		if (!m_hWnd)
		{
			return L"LockNote";
		}

		const int textLength = ::GetWindowTextLengthW(m_hWnd);
		if (textLength <= 0)
		{
			return L"LockNote";
		}

		std::wstring windowText(static_cast<size_t>(textLength) + 1, L'\0');
		::GetWindowTextW(m_hWnd, windowText.data(), textLength + 1);
		if (!windowText.empty() && windowText.back() == L'\0')
		{
			windowText.pop_back();
		}

		const size_t separatorPos = windowText.find(L" - ");
		if (separatorPos != std::wstring::npos && separatorPos > 0)
		{
			windowText = windowText.substr(0, separatorPos);
		}
		if (windowText.empty())
		{
			windowText = L"LockNote";
		}
		return windowText;
	}

	static COLORREF BlendColors(const COLORREF base, const COLORREF accent, const BYTE accentAlpha)
	{
		const BYTE baseAlpha = static_cast<BYTE>(255 - accentAlpha);
		const int r = (GetRValue(base) * baseAlpha + GetRValue(accent) * accentAlpha) / 255;
		const int g = (GetGValue(base) * baseAlpha + GetGValue(accent) * accentAlpha) / 255;
		const int b = (GetBValue(base) * baseAlpha + GetBValue(accent) * accentAlpha) / 255;
		return RGB(r, g, b);
	}

	static int GetCaptionButtonWidthForDpi(const int dpiX)
	{
		int captionButtonWidth = MulDiv(::GetSystemMetrics(SM_CXSIZE), dpiX, 96);
		HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
		if (hUser32 != nullptr)
		{
			using GetSystemMetricsForDpiFn = int(WINAPI*)(int, UINT);
			const auto getSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiFn>(
				::GetProcAddress(hUser32, "GetSystemMetricsForDpi"));
			if (getSystemMetricsForDpi != nullptr)
			{
				captionButtonWidth = getSystemMetricsForDpi(SM_CXSIZE, static_cast<UINT>((std::max)(dpiX, 96)));
			}
		}
		return captionButtonWidth;
	}

	static LPCWSTR GetUiStateRegistryPath()
	{
		return L"Software\\Steganos\\LockNote2";
	}

	void LoadWindowSizeFromRegistry()
	{
		CRegKey key;
		if (key.Open(HKEY_CURRENT_USER, GetUiStateRegistryPath(), KEY_READ) != ERROR_SUCCESS)
		{
			return;
		}

		DWORD width = 0;
		DWORD height = 0;
		if (key.QueryDWORDValue(L"WindowWidth", width) != ERROR_SUCCESS ||
			key.QueryDWORDValue(L"WindowHeight", height) != ERROR_SUCCESS)
		{
			return;
		}

		if (width < static_cast<DWORD>(kStoredWindowMinWidth) ||
			height < static_cast<DWORD>(kStoredWindowMinHeight))
		{
			return;
		}

		m_nWindowSizeX = static_cast<int>(width);
		m_nWindowSizeY = static_cast<int>(height);
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

		const RECT rcWork = monitorInfo.rcWork;
		const int workWidth = (std::max)(1, static_cast<int>(rcWork.right - rcWork.left));
		const int workHeight = (std::max)(1, static_cast<int>(rcWork.bottom - rcWork.top));
		const int systemMinWidth = ::GetSystemMetrics(SM_CXMINTRACK);
		const int systemMinHeight = ::GetSystemMetrics(SM_CYMINTRACK);
		const int minWidth = (std::min)((std::max)(kStoredWindowMinWidth, systemMinWidth), workWidth);
		const int minHeight = (std::min)((std::max)(kStoredWindowMinHeight, systemMinHeight), workHeight);

		m_nWindowSizeX = (std::clamp)(m_nWindowSizeX, minWidth, workWidth);
		m_nWindowSizeY = (std::clamp)(m_nWindowSizeY, minHeight, workHeight);
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
		m_topMenuHoveredIndex = -1;
		m_topMenuPressedIndex = -1;
		::SetRectEmpty(&m_topBarTitleRect);
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
		const int leftPadding = MulDiv(6, dpiX, 96);
		const int rightPadding = MulDiv(10, dpiX, 96);
		const int itemPaddingX = MulDiv(14, dpiX, 96);
		const int itemSpacing = MulDiv(3, dpiX, 96);
		const int topInset = MulDiv(7, dpiX, 96);
		const int bottomInset = MulDiv(7, dpiX, 96);

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(dc, GetUiFontHandle()));

		struct TopMenuEntry
		{
			int position;
			std::wstring caption;
		};
		std::vector<TopMenuEntry> entries;

		const auto tryAddPrimaryItem = [&](const int pos, const std::wstring& caption)
		{
			if (::GetSubMenu(hMenu, pos) != nullptr)
			{
				entries.push_back(TopMenuEntry{ pos, caption });
			}
		};

		tryAddPrimaryItem(0, WSTR(IDS_MENU_FILE));
		tryAddPrimaryItem(1, WSTR(IDS_MENU_EDIT));
		tryAddPrimaryItem(2, WSTR(IDS_MENU_VIEW));
		tryAddPrimaryItem(3, WSTR(IDS_MENU_HELP));

		const int itemCount = ::GetMenuItemCount(hMenu);
		for (int pos = 0; pos < itemCount; ++pos)
		{
			HMENU hSubMenu = ::GetSubMenu(hMenu, pos);
			if (hSubMenu == nullptr)
			{
				continue;
			}

			const auto existingEntry = std::find_if(
				entries.begin(),
				entries.end(),
				[pos](const TopMenuEntry& entry)
				{
					return entry.position == pos;
				});
			if (existingEntry != entries.end())
			{
				continue;
			}

			const bool hasEncryptionItems =
				::GetMenuState(hSubMenu, ID_STEGANOS_PASSWORD_MANAGER, MF_BYCOMMAND) != static_cast<UINT>(-1) ||
				::GetMenuState(hSubMenu, ID_STEGANOS_SAFE, MF_BYCOMMAND) != static_cast<UINT>(-1);
			if (hasEncryptionItems)
			{
				entries.push_back(TopMenuEntry{ pos, GetEncryptionMenuCaption() });
				break;
			}
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
			const int buttonWidth = (std::max)(textWidth + (itemPaddingX * 2), MulDiv(44, dpiX, 96));
			measuredEntries.push_back(MeasuredMenuEntry{ entry.position, caption, buttonWidth });
		}

		const std::wstring overflowCaption = L">>";
		SIZE overflowSize{};
		::GetTextExtentPoint32W(dc, overflowCaption.c_str(), static_cast<int>(overflowCaption.size()), &overflowSize);
		const int overflowTextWidth = static_cast<int>(overflowSize.cx);
		const int overflowWidth = (std::max)(overflowTextWidth + (itemPaddingX * 2), MulDiv(44, dpiX, 96));

		const int captionButtonWidth = GetCaptionButtonWidthForDpi(dpiX);
		const int reservedWindowButtonsWidth = (captionButtonWidth * 3) + MulDiv(4, dpiX, 96);

		int menuRight = static_cast<int>(rcClient.right) - rightPadding - reservedWindowButtonsWidth;
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
					m_nTopBarHeight - bottomInset
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
					m_nTopBarHeight - bottomInset
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
		UpdateFindPanelVisibility();
		if (!m_isFindPanelVisible)
		{
			::SetRectEmpty(&m_findPanelRect);
			::SetRectEmpty(&m_findPanelFindEditFrameRect);
			::SetRectEmpty(&m_findPanelReplaceEditFrameRect);
			::SetRectEmpty(&m_findPanelSearchIconRect);
			if (!::IsRectEmpty(&previousPanelRect))
			{
				::InvalidateRect(m_hWnd, &previousPanelRect, TRUE);
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
		RECT invalidateRect{};
		if (::IsRectEmpty(&previousPanelRect))
		{
			invalidateRect = m_findPanelRect;
		}
		else
		{
			::UnionRect(&invalidateRect, &previousPanelRect, &m_findPanelRect);
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
		CClientDC dc(*this);
		const int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
		const int dpiY = GetDeviceCaps(dc, LOGPIXELSY);

		int statusHeight = 0;
		if (m_hWndStatusBar)
		{
			NormalizeStatusBarStyles();
			::ShowWindow(m_hWndStatusBar, m_isStatusBarVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
			if (m_isStatusBarVisible)
			{
				::SendMessage(m_hWndStatusBar, WM_SIZE, 0, 0);
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
				RECT rcStatus{};
				::GetWindowRect(m_hWndStatusBar, &rcStatus);
				::MapWindowPoints(nullptr, m_hWnd, reinterpret_cast<LPPOINT>(&rcStatus), 2);
				statusHeight = rcStatus.bottom - rcStatus.top;
			}
		}

		const int findPanelHeight = GetFindPanelAreaHeightForDpi(dpiY);
		LayoutFindPanelControls(rcClient, dpiX, dpiY);
		const int editTop = m_nTopBarHeight + findPanelHeight;
		const int editHeight = (rcClient.bottom - rcClient.top) - editTop - statusHeight;
		::MoveWindow(m_view, 0, editTop, rcClient.right - rcClient.left, (std::max)(0, editHeight), TRUE);
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();
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
		const bool isHot = (pDrawItem->itemState & ODS_HOTLIGHT) != 0;
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
		if (ptClient.y < 0 || ptClient.y >= m_nTopMenuHeight)
		{
			return -1;
		}

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

	void DrawTopBar(HDC hdc)
	{
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		RECT rcTopBar{ 0, 0, rcClient.right, m_nTopBarHeight };
		CBrush topBarBrush;
		topBarBrush.CreateSolidBrush(m_topBarBackgroundColor);
		::FillRect(hdc, &rcTopBar, topBarBrush);

		HFONT hOldBaseFont = reinterpret_cast<HFONT>(::SelectObject(hdc, GetUiFontHandle()));
		::SetBkMode(hdc, TRANSPARENT);
		const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		const int menuHighlightRadius = (std::max)(MulDiv(7, dpiX, 96), 4);
		const int menuHighlightInsetY = (std::max)(MulDiv(1, dpiX, 96), 1);

		const auto drawRoundedMenuHighlight = [&](const RECT& rcButton, const COLORREF fillColor)
		{
			RECT rcHighlight = rcButton;
			rcHighlight.top += menuHighlightInsetY;
			rcHighlight.bottom -= menuHighlightInsetY;
			if ((rcHighlight.right - rcHighlight.left) < 2 || (rcHighlight.bottom - rcHighlight.top) < 2)
			{
				return;
			}

			HBRUSH hFillBrush = ::CreateSolidBrush(fillColor);
			HPEN hBorderPen = ::CreatePen(
				PS_SOLID,
				1,
				BlendColors(fillColor, m_topBarDividerColor, m_isDarkThemeApplied ? 120 : 96));
			if (hFillBrush == nullptr)
			{
				if (hBorderPen != nullptr)
				{
					::DeleteObject(hBorderPen);
				}
				return;
			}

			HGDIOBJ hOldBrush = ::SelectObject(hdc, hFillBrush);
			HGDIOBJ hOldPen = ::SelectObject(hdc, (hBorderPen != nullptr) ? hBorderPen : ::GetStockObject(NULL_PEN));
			::RoundRect(
				hdc,
				rcHighlight.left,
				rcHighlight.top,
				rcHighlight.right,
				rcHighlight.bottom,
				menuHighlightRadius,
				menuHighlightRadius);

			if (hOldPen != nullptr)
			{
				::SelectObject(hdc, hOldPen);
			}
			if (hOldBrush != nullptr)
			{
				::SelectObject(hdc, hOldBrush);
			}
			::DeleteObject(hFillBrush);
			if (hBorderPen != nullptr)
			{
				::DeleteObject(hBorderPen);
			}
		};

		if (!::IsRectEmpty(&m_topBarTitleRect))
		{
			RECT rcTitleText = m_topBarTitleRect;
			rcTitleText.left += MulDiv(10, dpiX, 96);
			rcTitleText.right -= MulDiv(8, dpiX, 96);
			::SetTextColor(hdc, m_topBarTextColor);
			::DrawTextW(hdc, m_topBarTitleText.c_str(), -1, &rcTitleText, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
		}

		for (size_t i = 0; i < m_topMenuButtonRects.size(); ++i)
		{
			RECT rcButton = m_topMenuButtonRects[i];
			const std::wstring caption = (i < m_topMenuCaptions.size()) ? m_topMenuCaptions[i] : L"";

			if (static_cast<int>(i) == m_topMenuPressedIndex)
			{
				drawRoundedMenuHighlight(rcButton, m_topBarPressedColor);
			}
			else if (static_cast<int>(i) == m_topMenuHoveredIndex)
			{
				drawRoundedMenuHighlight(rcButton, m_topBarHoverColor);
			}

			::SetTextColor(hdc, m_topBarTextColor);
			RECT rcText = rcButton;
			::DrawTextW(hdc, caption.c_str(), static_cast<int>(caption.size()), &rcText, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_HIDEPREFIX);
		}

		HFONT hOldToolbarFont = nullptr;
		if (!m_toolbarButtonRects.empty())
		{
			hOldToolbarFont = reinterpret_cast<HFONT>(::SelectObject(hdc, GetToolbarIconFontHandle()));
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
			if (m_nTopBarHeight > 1)
			{
				RECT rcSoftDivider{
					0,
					m_nTopBarHeight - 2,
					rcClient.right,
					m_nTopBarHeight - 1
				};
				CBrush softDividerBrush;
				const BYTE softAlpha = m_isDarkThemeApplied ? 96 : 118;
				softDividerBrush.CreateSolidBrush(BlendColors(m_topBarBackgroundColor, m_topBarDividerColor, softAlpha));
				::FillRect(hdc, &rcSoftDivider, softDividerBrush);
			}

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
		if (m_isFindPanelVisible && !::IsRectEmpty(&m_findPanelRect))
		{
			RECT rcPaintFind{};
			if (::IntersectRect(&rcPaintFind, &m_findPanelRect, &ps.rcPaint))
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
		const int hoveredMenuIndex = HitTestTopMenuButton(pt);
		const int hoveredToolbarIndex = HitTestToolbarButton(pt);
		if (hoveredMenuIndex != m_topMenuHoveredIndex || hoveredToolbarIndex != m_toolbarHoveredIndex)
		{
			m_topMenuHoveredIndex = hoveredMenuIndex;
			m_toolbarHoveredIndex = hoveredToolbarIndex;
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
		if (m_topMenuHoveredIndex != -1 || m_toolbarHoveredIndex != -1)
		{
			m_topMenuHoveredIndex = -1;
			m_toolbarHoveredIndex = -1;
			InvalidateTopBar();
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		if (pt.y >= 0 && pt.y < m_nTopBarHeight)
		{
			const bool isTopMenuHit = HitTestTopMenuButton(pt) >= 0;
			const bool isToolbarHit = HitTestToolbarButton(pt) >= 0;
			if (!isTopMenuHit && !isToolbarHit)
			{
				::ReleaseCapture();
				::SendMessageW(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
				return 0;
			}
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
		if (!m_frameBackgroundBrush.m_hBrush)
		{
			return FALSE;
		}

		HDC hdc = reinterpret_cast<HDC>(wParam);
		RECT rc{};
		::GetClientRect(m_hWnd, &rc);
		if (m_nTopBarHeight < rc.bottom)
		{
			RECT rcBody = rc;
			rcBody.top = m_nTopBarHeight;
			::FillRect(hdc, &rcBody, m_frameBackgroundBrush.m_hBrush);
			if (m_hWndStatusBar && m_isStatusBarVisible && m_statusBarBackgroundBrush.m_hBrush)
			{
				RECT rcStatus{};
				::GetWindowRect(m_hWndStatusBar, &rcStatus);
				::MapWindowPoints(nullptr, m_hWnd, reinterpret_cast<LPPOINT>(&rcStatus), 2);
				RECT rcStatusFill{
					0,
					rcStatus.top > 0 ? rcStatus.top : 0,
					rc.right,
					rc.bottom
				};
				::FillRect(hdc, &rcStatusFill, m_statusBarBackgroundBrush.m_hBrush);
			}
		}
		else
		{
			::FillRect(hdc, &rc, m_frameBackgroundBrush.m_hBrush);
		}
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
			LayoutMainControls();
		}
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_pCurrentFindReplaceDialog != nullptr && ::IsWindow(m_pCurrentFindReplaceDialog->m_hWnd))
		{
			DetachFindDialogSubclass(m_pCurrentFindReplaceDialog->m_hWnd);
		}
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
			m_view.SetLimitText(0x7ffffffe); // allow a lot of text to be entered
			m_isWordWrapEnabled = ((::GetWindowLongPtrW(m_view, GWL_STYLE) & WS_HSCROLL) == 0);

			CClientDC dc(*this);
			auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
			auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
			if (m_nWindowSizeX == DEFAULT_WIDTH && m_nWindowSizeY == DEFAULT_HEIGHT)
			{
				// resize window depending on dpi scale factor
				m_nWindowSizeX = MulDiv(m_nWindowSizeX, dpi, 96);
				m_nWindowSizeY = MulDiv(m_nWindowSizeY, dpi, 96);
			}
			LoadWindowSizeFromRegistry();
			ClampWindowSizeToCurrentWorkArea();
			m_nTopMenuHeight = MulDiv(42, dpi, 96);
			m_nToolbarHeight = 0;
			m_nTopBarHeight = m_nTopMenuHeight;
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
				L"Segoe UI");
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
				L"Segoe UI");
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

			const std::string windowTitle = title + " - " + STR(IDR_MAINFRAME);

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

			SetWindowPos(NULL, 0, 0, m_nWindowSizeX, m_nWindowSizeY, SWP_NOZORDER | SWP_NOMOVE);
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

