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

inline const UINT UWM_FINDMSGSTRING = CFindReplaceDialog::GetFindReplaceMsg();

enum class ThemeMode : int
{
	System = 0,
	Light = 1,
	Dark = 2
};

inline constexpr UINT ID_TOOLBAR_SETTINGS = WM_APP + 101;

class CSettingsWindow;

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
	COLORREF m_viewTextColor{ ::GetSysColor(COLOR_WINDOWTEXT) };
	COLORREF m_viewBackgroundColor{ ::GetSysColor(COLOR_WINDOW) };
	COLORREF m_frameBackgroundColor{ ::GetSysColor(COLOR_3DFACE) };
	COLORREF m_topBarBackgroundColor{ RGB(25, 35, 49) };
	COLORREF m_topBarTextColor{ RGB(230, 230, 230) };
	COLORREF m_topBarHoverColor{ RGB(54, 65, 82) };
	COLORREF m_topBarPressedColor{ RGB(69, 82, 101) };
	COLORREF m_topBarDividerColor{ RGB(43, 53, 67) };
	COLORREF m_toolbarBackgroundColor{ RGB(25, 35, 49) };
	COLORREF m_toolbarHoverColor{ RGB(58, 70, 87) };
	COLORREF m_toolbarPressedColor{ RGB(74, 86, 105) };
	COLORREF m_statusBarBackgroundColor{ RGB(35, 28, 24) };
	COLORREF m_statusBarDividerColor{ RGB(65, 56, 49) };
	COLORREF m_statusBarTextColor{ RGB(222, 222, 222) };
	CBrush m_viewBackgroundBrush;
	CBrush m_frameBackgroundBrush;
	CBrush m_statusBarBackgroundBrush;
	HMENU m_hMainMenu{ nullptr };
	int m_nTopBarHeight{ 42 };
	int m_nTopMenuHeight{ 42 };
	int m_nToolbarHeight{ 0 };
	std::vector<int> m_topMenuPopupPositions;
	std::vector<RECT> m_topMenuButtonRects;
	std::vector<std::wstring> m_topMenuCaptions;
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
	CSettingsWindow* m_settingsWindow{ nullptr };

	CMainFrame()
	{
	}

	virtual ~CMainFrame() = default;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
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
		RefreshSettingsWindow();
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
		UpdateStatusBar();
		m_view.Invalidate();
		RefreshSettingsWindow();
	}

	bool IsFormattingEnabled() const
	{
		return m_isFormattingEnabled;
	}

	void SetFormattingEnabled(const bool enabled)
	{
		m_isFormattingEnabled = enabled;
		RefreshSettingsWindow();
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
			return L"Русский";
		case LANG_GERMAN:
			return L"Deutsch";
		case LANG_FRENCH:
			return L"Français";
		case LANG_SPANISH:
			return L"Español";
		case LANG_ITALIAN:
			return L"Italiano";
		case LANG_PORTUGUESE:
			return L"Português";
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

	void OpenSettingsWindow();
	void CloseSettingsWindow();
	void RefreshSettingsWindow();

	void OnSettingsWindowClosed()
	{
		m_settingsWindow = nullptr;
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
			// Status bar is fully owner-drawn; disable visual style edges/dividers.
			setWindowTheme(m_hWndStatusBar, L"", L"");
		}

		::FreeLibrary(hUxTheme);
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
			constexpr int AppModeAllowDark = 1;
			setPreferredAppMode(m_isDarkThemeApplied ? AppModeAllowDark : AppModeDefault);
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
			m_viewBackgroundColor = RGB(36, 36, 36);
			m_frameBackgroundColor = RGB(32, 32, 32);
			m_topBarBackgroundColor = RGB(25, 35, 49);
			m_topBarTextColor = RGB(230, 233, 238);
			m_topBarHoverColor = RGB(54, 65, 82);
			m_topBarPressedColor = RGB(69, 82, 101);
			m_topBarDividerColor = RGB(43, 53, 67);
			m_toolbarBackgroundColor = RGB(25, 35, 49);
			m_toolbarHoverColor = RGB(58, 70, 87);
			m_toolbarPressedColor = RGB(74, 86, 105);
			m_statusBarBackgroundColor = RGB(35, 28, 24);
			m_statusBarDividerColor = RGB(65, 56, 49);
			m_statusBarTextColor = RGB(222, 222, 222);
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

		if (m_hWndStatusBar)
		{
			::SendMessageW(m_hWndStatusBar, SB_SETBKCOLOR, 0, static_cast<LPARAM>(m_statusBarBackgroundColor));
		}

		ApplyExplorerTheme(m_hWnd, m_isDarkThemeApplied);
		ApplyExplorerTheme(m_view.m_hWnd, m_isDarkThemeApplied);
		ApplyStatusBarTheme();

		ApplyTitleBarTheme();
		ApplyPopupMenuTheme();

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
		RefreshSettingsWindow();
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

	void RefreshTopMenuLayout()
	{
		m_topMenuPopupPositions.clear();
		m_topMenuButtonRects.clear();
		m_topMenuCaptions.clear();
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
		const int leftPadding = MulDiv(8, dpiX, 96);
		const int sectionSpacing = MulDiv(10, dpiX, 96);
		const int itemPaddingX = MulDiv(12, dpiX, 96);
		const int itemSpacing = MulDiv(6, dpiX, 96);
		const int topInset = MulDiv(6, dpiX, 96);
		const int bottomInset = MulDiv(6, dpiX, 96);

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(dc, GetUiFontHandle()));
		const int titlePadding = MulDiv(10, dpiX, 96);
		const int titleIconSize = MulDiv(16, dpiX, 96);
		const int titleTextSpacing = MulDiv(8, dpiX, 96);
		const int titleMinWidth = MulDiv(180, dpiX, 96);
		const int titleMaxTextWidth = MulDiv(260, dpiX, 96);

		m_topBarTitleText = GetTopBarTitleText();
		SIZE titleTextSize{};
		::GetTextExtentPoint32W(
			dc,
			m_topBarTitleText.c_str(),
			static_cast<int>(m_topBarTitleText.size()),
			&titleTextSize);
		const int titleWidth = (std::max)(
			titleMinWidth,
			(titlePadding * 2) + titleIconSize + titleTextSpacing + (std::min)(static_cast<int>(titleTextSize.cx), titleMaxTextWidth));
		const int titleRight = (std::min)(static_cast<int>(rcClient.right) - leftPadding, leftPadding + titleWidth);
		m_topBarTitleRect = {
			leftPadding,
			topInset,
			titleRight,
			m_nTopBarHeight - bottomInset
		};

		int x = m_topBarTitleRect.right + sectionSpacing;
		int maxRight = static_cast<int>(rcClient.right) - leftPadding;
		if (!m_toolbarButtonRects.empty())
		{
			maxRight = m_toolbarButtonRects.front().left - sectionSpacing;
		}
		if (maxRight < x + MulDiv(110, dpiX, 96))
		{
			maxRight = (std::min)(
				static_cast<int>(rcClient.right) - leftPadding,
				x + MulDiv(110, dpiX, 96));
		}

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
		tryAddPrimaryItem(3, L"?");

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
				entries.push_back(TopMenuEntry{ pos, L"Encryption" });
				break;
			}
		}

		for (const TopMenuEntry& entry : entries)
		{
			const std::wstring& caption = entry.caption;

			SIZE textSize{};
			::GetTextExtentPoint32W(dc, caption.c_str(), static_cast<int>(caption.size()), &textSize);

			RECT buttonRect{
				x,
				topInset,
				x + textSize.cx + (itemPaddingX * 2),
				m_nTopBarHeight - bottomInset
			};
			if (buttonRect.right > maxRight && !m_topMenuButtonRects.empty())
			{
				break;
			}
			if (buttonRect.right > rcClient.right - leftPadding)
			{
				break;
			}
			m_topMenuPopupPositions.push_back(entry.position);
			m_topMenuButtonRects.push_back(buttonRect);
			m_topMenuCaptions.push_back(std::move(caption));
			x = buttonRect.right + itemSpacing;
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
		if (!m_hWnd || m_nTopBarHeight <= 0)
		{
			return;
		}

		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);

		CClientDC dc(*this);
		const int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
		const int minLeftPadding = MulDiv(300, dpiX, 96);
		const int rightPadding = MulDiv(12, dpiX, 96);
		const int itemPaddingX = MulDiv(10, dpiX, 96);
		const int itemSpacing = MulDiv(6, dpiX, 96);
		const int topInset = MulDiv(6, dpiX, 96);
		const int bottomInset = m_nTopBarHeight - MulDiv(6, dpiX, 96);

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(dc, GetToolbarIconFontHandle()));
		int x = rcClient.right - rightPadding;

		struct ToolbarEntry
		{
			UINT commandId;
			std::wstring caption;
		};
		const std::vector<ToolbarEntry> entries{
			{ ID_EDIT_FIND, L"\xE721" },
			{ ID_TOOLBAR_SETTINGS, L"\xE713" }
		};

		std::vector<UINT> commandIds;
		std::vector<RECT> buttonRects;
		std::vector<std::wstring> captions;

		for (auto it = entries.rbegin(); it != entries.rend(); ++it)
		{
			const auto& entry = *it;
			if (entry.caption.empty())
			{
				continue;
			}

			SIZE textSize{};
			::GetTextExtentPoint32W(dc, entry.caption.c_str(), static_cast<int>(entry.caption.size()), &textSize);
			const int buttonWidth = textSize.cx + (itemPaddingX * 2);
			const int left = x - buttonWidth;
			if (left < minLeftPadding)
			{
				break;
			}
			RECT buttonRect{
				left,
				topInset,
				x,
				bottomInset
			};
			commandIds.push_back(entry.commandId);
			buttonRects.push_back(buttonRect);
			captions.push_back(entry.caption);
			x = left - itemSpacing;
		}
		std::reverse(commandIds.begin(), commandIds.end());
		std::reverse(buttonRects.begin(), buttonRects.end());
		std::reverse(captions.begin(), captions.end());
		m_toolbarCommandIds = std::move(commandIds);
		m_toolbarButtonRects = std::move(buttonRects);
		m_toolbarButtonCaptions = std::move(captions);

		::SelectObject(dc, hOldFont);
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

		int statusHeight = 0;
		if (m_hWndStatusBar)
		{
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

		const int editTop = m_nTopBarHeight;
		const int editHeight = (rcClient.bottom - rcClient.top) - editTop - statusHeight;
		::MoveWindow(m_view, 0, editTop, rcClient.right - rcClient.left, (std::max)(0, editHeight), TRUE);
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
		InvalidateTopBar();
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

	void OpenTopMenuAtIndex(const int buttonIndex)
	{
		const HMENU hMenu = GetMainMenuHandle();
		if (hMenu == nullptr || buttonIndex < 0 || buttonIndex >= static_cast<int>(m_topMenuPopupPositions.size()))
		{
			return;
		}

		const int menuPos = m_topMenuPopupPositions[buttonIndex];
		HMENU hSubMenu = ::GetSubMenu(hMenu, menuPos);
		if (hSubMenu == nullptr)
		{
			return;
		}

		POINT popupOrigin{
			m_topMenuButtonRects[buttonIndex].left,
			m_topMenuButtonRects[buttonIndex].bottom
		};
		::ClientToScreen(m_hWnd, &popupOrigin);
		m_topMenuPressedIndex = buttonIndex;
		InvalidateTopBar();
		ApplyPopupMenuTheme();

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

		if (commandId != 0)
		{
			::PostMessageW(m_hWnd, WM_COMMAND, MAKEWPARAM(commandId, 0), 0);
		}
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
		if (commandId == ID_TOOLBAR_SETTINGS)
		{
			OpenSettingsWindow();
		}
		else if (commandId != 0)
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
		const int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		CBrush dividerBrush;
		dividerBrush.CreateSolidBrush(m_topBarDividerColor);

		if (!::IsRectEmpty(&m_topBarTitleRect))
		{
			const int iconSize = MulDiv(16, dpiX, 96);
			const int iconLeft = m_topBarTitleRect.left + MulDiv(4, dpiX, 96);
			const int iconTop = m_topBarTitleRect.top + (m_topBarTitleRect.bottom - m_topBarTitleRect.top - iconSize) / 2;
			HICON hIcon = reinterpret_cast<HICON>(::SendMessageW(m_hWnd, WM_GETICON, ICON_SMALL2, 0));
			if (hIcon == nullptr)
			{
				hIcon = reinterpret_cast<HICON>(::SendMessageW(m_hWnd, WM_GETICON, ICON_SMALL, 0));
			}
			if (hIcon == nullptr)
			{
				hIcon = reinterpret_cast<HICON>(::GetClassLongPtrW(m_hWnd, GCLP_HICONSM));
			}
			if (hIcon != nullptr)
			{
				::DrawIconEx(hdc, iconLeft, iconTop, hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
			}

			RECT rcTitleText = m_topBarTitleRect;
			rcTitleText.left = iconLeft + iconSize + MulDiv(8, dpiX, 96);
			rcTitleText.right -= MulDiv(4, dpiX, 96);
			::SetTextColor(hdc, m_topBarTextColor);
			::DrawTextW(hdc, m_topBarTitleText.c_str(), -1, &rcTitleText, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);

			if (!m_topMenuButtonRects.empty())
			{
				RECT rcTitleDivider{
					m_topBarTitleRect.right + MulDiv(4, dpiX, 96),
					m_topBarTitleRect.top + MulDiv(8, dpiY, 96),
					m_topBarTitleRect.right + MulDiv(5, dpiX, 96),
					m_topBarTitleRect.bottom - MulDiv(8, dpiY, 96)
				};
				::FillRect(hdc, &rcTitleDivider, dividerBrush);
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
				::FillRect(hdc, &rcButton, pressedBrush);
			}
			else if (static_cast<int>(i) == m_topMenuHoveredIndex)
			{
				CBrush hoverBrush;
				hoverBrush.CreateSolidBrush(m_topBarHoverColor);
				::FillRect(hdc, &rcButton, hoverBrush);
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

		if (!m_toolbarButtonRects.empty() && (!m_topMenuButtonRects.empty() || !::IsRectEmpty(&m_topBarTitleRect)))
		{
			const int dividerLeft = m_toolbarButtonRects.front().left - MulDiv(6, dpiX, 96);
			RECT rcGroupDivider{
				dividerLeft,
				MulDiv(9, dpiX, 96),
				dividerLeft + 1,
				m_nTopBarHeight - MulDiv(9, dpiX, 96)
			};
			::FillRect(hdc, &rcGroupDivider, dividerBrush);
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
			swprintf_s(statusPart0, L"Строка %d, столбец %d", static_cast<int>(current_line_number), static_cast<int>(column));
			swprintf_s(statusPart1, L"%d символов", charsCount);
		}
		else
		{
			swprintf_s(statusPart0, L"Ln %d, Col %d", static_cast<int>(current_line_number), static_cast<int>(column));
			swprintf_s(statusPart1, L"%d chars", charsCount);
		}
		m_statusBarPartTexts[0] = statusPart0;
		m_statusBarPartTexts[1] = statusPart1;
		m_statusBarPartTexts[2] = isRussianUi ? L"Обычный текст" : L"Plain text";
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
		RefreshSettingsWindow();
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
		RefreshSettingsWindow();
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
		RefreshSettingsWindow();

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
		RefreshSettingsWindow();
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
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		const auto* pDrawItem = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
		if (pDrawItem == nullptr || m_hWndStatusBar == nullptr)
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
					rcStatus.top > 0 ? rcStatus.top - 1 : 0,
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
		CloseSettingsWindow();
		m_text = text;
		m_view.SetModify(FALSE);

		return FALSE;
	}
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int m_nOldWindowSizeX = m_nWindowSizeX;
		int m_nOldWindowSizeY = m_nWindowSizeY;
		RECT rc;
		GetWindowRect(&rc);
		m_nWindowSizeX = rc.right - rc.left;
		m_nWindowSizeY = rc.bottom - rc.top;
		bHandled = TRUE;
		if (m_nWindowSizeX != m_nOldWindowSizeX || m_nWindowSizeY != m_nOldWindowSizeY)
		{
			m_bTraitsChanged = true;
			UpdateStatusBar();
		}
		if (wParam != SIZE_MINIMIZED)
		{
			LayoutMainControls();
		}
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CloseSettingsWindow();
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
		if (m_fontUi.m_hFont)
		{
			m_fontUi.DeleteObject();
		}
		if (m_fontToolbarIcons.m_hFont)
		{
			m_fontToolbarIcons.DeleteObject();
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

			DragAcceptFiles(TRUE);

			CreateSimpleStatusBar(); //m_hWndStatusBar
			if (m_hWndStatusBar)
			{
				const LONG_PTR style = ::GetWindowLongPtrW(m_hWndStatusBar, GWL_STYLE);
				::SetWindowLongPtrW(
					m_hWndStatusBar,
					GWL_STYLE,
					(style & ~SBARS_SIZEGRIP & ~WS_BORDER) | CCS_NODIVIDER);
				::SetWindowPos(
					m_hWndStatusBar,
					nullptr,
					0,
					0,
					0,
					0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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
			if (m_hWndStatusBar && m_fontUi.m_hFont != nullptr)
			{
				::SendMessageW(m_hWndStatusBar, WM_SETFONT, reinterpret_cast<WPARAM>(static_cast<HFONT>(m_fontUi)), TRUE);
			}
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
		UpdateThemeMenuChecks();
		ConfigureEncryptionMenu();
		UpdateEncryptionMenuChecks();
		RefreshToolbarLayout();
		RefreshTopMenuLayout();
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

			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_SYSTEM, L"System");
			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_LIGHT, L"Light");
			::AppendMenuW(hThemeMenu, MF_STRING, ID_THEME_DARK, L"Dark");
			::AppendMenuW(hViewMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hThemeMenu), L"Theme");
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
			topMenuInfo.dwTypeData = const_cast<LPWSTR>(L"Encryption");
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

		ChangeMenuItemText(ID_STEGANOS_PASSWORD_MANAGER, L"Modern encryption (scrypt)");
		ChangeMenuItemText(ID_STEGANOS_SAFE, L"Compatibility encryption (PBKDF2-SHA256)");
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
		if (m_pCurrentFindReplaceDialog)
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

	LRESULT OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_pCurrentFindReplaceDialog)
		{
			m_pCurrentFindReplaceDialog->ShowWindow(SW_SHOW);
			m_pCurrentFindReplaceDialog->SetFocus();
			return 0;
		}
		CFindReplaceDialog* pDlgFindReplace = new CFindReplaceDialog();
		pDlgFindReplace->Create(TRUE, utf8_to_wstring(m_strSearchString).c_str(), _T(""), m_dwSearchFlags, *this);
		pDlgFindReplace->ShowWindow(SW_SHOW);
		m_pCurrentFindReplaceDialog = pDlgFindReplace;
		return 0;
	}	

	void FindNext()
	{
		int nBegin = 0;
		int nEnd = 0;
		std::string text = GetText();
		std::string searchtext = m_strSearchString;
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
			if (nIndex == std::wstring::npos)
			{
				nIndex = wtext.rfind(wsearchtext);
			}
		}
		else
		{
			nIndex = wtext.find(wsearchtext, static_cast<size_t>(nEnd));
			if (nIndex == std::wstring::npos)
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
			if (m_pCurrentFindReplaceDialog)
			{
				m_pCurrentFindReplaceDialog->SetFocus();
			}
		}
	}

	LRESULT OnEditFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_pCurrentFindReplaceDialog)
		{
			BOOL bHandled = FALSE;
			OnEditFind(0, 0, NULL, bHandled);
		}
		else
		{
			FindNext();
		}
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
			m_pCurrentFindReplaceDialog = nullptr;
		}
		else if (pDlgFindReplace->FindNext())
		{
			FindNext();
		}

		return 0;
	}

	LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_ignoreEditNotifications)
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

class CSettingsWindow : public CWindowImpl<CSettingsWindow>
{
public:
	DECLARE_WND_CLASS(L"LockNote2SettingsWindow")

	explicit CSettingsWindow(CMainFrame* owner)
		: m_owner(owner)
	{
	}

	BEGIN_MSG_MAP(CSettingsWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

private:
	enum CardId : int
	{
		CardTheme = 0,
		CardFont,
		CardWrap,
		CardFormatting,
		CardStatusBar,
		CardLanguage,
		CardCount
	};

	CMainFrame* m_owner{ nullptr };
	CFont m_fontBase;
	CFont m_fontTitle;
	std::array<RECT, CardCount> m_cardRects{};
	RECT m_infoRect{};
	RECT m_aboutButtonRect{};
	int m_hoveredCard{ -1 };
	bool m_hoveredAboutButton{ false };

	int Scale(const int value) const
	{
		if (!m_hWnd)
		{
			return value;
		}
		HDC hdc = ::GetDC(m_hWnd);
		const int dpiX = hdc ? GetDeviceCaps(hdc, LOGPIXELSX) : 96;
		if (hdc)
		{
			::ReleaseDC(m_hWnd, hdc);
		}
		return MulDiv(value, dpiX, 96);
	}

	bool IsRussianUi() const
	{
		return m_owner != nullptr && m_owner->IsRussianUi();
	}

	std::wstring ThemeLabel() const
	{
		if (m_owner == nullptr)
		{
			return L"";
		}
		switch (CMainFrame::ParseThemeMode(m_owner->GetThemeMode()))
		{
		case ThemeMode::Dark:
			return IsRussianUi() ? L"Темная" : L"Dark";
		case ThemeMode::Light:
			return IsRussianUi() ? L"Светлая" : L"Light";
		case ThemeMode::System:
		default:
			return IsRussianUi() ? L"Системная" : L"System";
		}
	}

	std::wstring OnOffLabel(const bool enabled) const
	{
		return enabled
			? (IsRussianUi() ? L"Вкл." : L"On")
			: (IsRussianUi() ? L"Выкл." : L"Off");
	}

	void RecalculateLayout()
	{
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		const int margin = Scale(28);
		const int top = Scale(120);
		const int gap = Scale(12);
		const int cardHeight = Scale(72);
		const int minInfoWidth = Scale(330);
		const int preferredLeftWidth = Scale(520);

		int leftWidth = preferredLeftWidth;
		const int availableWidth = (rcClient.right - rcClient.left) - (margin * 3) - minInfoWidth;
		if (availableWidth < preferredLeftWidth)
		{
			leftWidth = (std::max)(Scale(300), availableWidth);
		}

		int y = top;
		for (int i = 0; i < CardCount; ++i)
		{
			m_cardRects[i] = {
				margin,
				y,
				margin + leftWidth,
				y + cardHeight
			};
			y += cardHeight + gap;
		}

		m_infoRect = {
			margin + leftWidth + margin,
			top,
			rcClient.right - margin,
			rcClient.bottom - margin
		};

		m_aboutButtonRect = {
			m_infoRect.left,
			m_infoRect.top + Scale(210),
			(std::min)(m_infoRect.right, m_infoRect.left + Scale(190)),
			m_infoRect.top + Scale(248)
		};
	}

	void DrawCard(
		HDC hdc,
		const RECT& rcCard,
		const std::wstring& title,
		const std::wstring& value,
		const bool hovered) const
	{
		CBrush cardBrush;
		cardBrush.CreateSolidBrush(hovered ? RGB(62, 62, 66) : RGB(49, 49, 52));
		::FillRect(hdc, &rcCard, cardBrush);
		::FrameRect(hdc, &rcCard, static_cast<HBRUSH>(::GetStockObject(DC_BRUSH)));

		RECT rcTitle = rcCard;
		rcTitle.left += Scale(16);
		rcTitle.top += Scale(10);
		rcTitle.right -= Scale(12);
		rcTitle.bottom = rcTitle.top + Scale(24);

		RECT rcValue = rcCard;
		rcValue.left += Scale(16);
		rcValue.top += Scale(34);
		rcValue.right -= Scale(12);
		rcValue.bottom -= Scale(8);

		::SetTextColor(hdc, RGB(240, 240, 240));
		::DrawTextW(hdc, title.c_str(), -1, &rcTitle, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
		::SetTextColor(hdc, RGB(180, 180, 180));
		::DrawTextW(hdc, value.c_str(), -1, &rcValue, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	}

	void DrawSettings(HDC hdc)
	{
		if (m_owner == nullptr)
		{
			return;
		}

		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		CBrush backgroundBrush;
		backgroundBrush.CreateSolidBrush(RGB(32, 32, 34));
		::FillRect(hdc, &rcClient, backgroundBrush);

		::SetBkMode(hdc, TRANSPARENT);
		::SetDCBrushColor(hdc, RGB(76, 76, 82));

		HFONT hOldFont = reinterpret_cast<HFONT>(::SelectObject(hdc, static_cast<HFONT>(m_fontTitle)));
		RECT rcTitle{
			Scale(28),
			Scale(22),
			rcClient.right - Scale(28),
			Scale(94)
		};
		::SetTextColor(hdc, RGB(245, 245, 245));
		const wchar_t* settingsTitle = IsRussianUi() ? L"Настройки" : L"Settings";
		::DrawTextW(hdc, settingsTitle, -1, &rcTitle, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		::SelectObject(hdc, static_cast<HFONT>(m_fontBase));
		RECT rcSectionLeft{
			Scale(28),
			Scale(94),
			m_cardRects[CardTheme].right,
			Scale(120)
		};
		::SetTextColor(hdc, RGB(214, 214, 214));
		const wchar_t* appearanceTitle = IsRussianUi() ? L"Параметры" : L"Preferences";
		::DrawTextW(hdc, appearanceTitle, -1, &rcSectionLeft, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		DrawCard(
			hdc,
			m_cardRects[CardTheme],
			IsRussianUi() ? L"Тема приложения" : L"Application theme",
			ThemeLabel(),
			m_hoveredCard == CardTheme);
		DrawCard(
			hdc,
			m_cardRects[CardFont],
			IsRussianUi() ? L"Шрифт" : L"Font",
			utf8_to_wstring(m_owner->m_strFontName) + L", " + std::to_wstring(m_owner->m_nFontSize),
			m_hoveredCard == CardFont);
		DrawCard(
			hdc,
			m_cardRects[CardWrap],
			IsRussianUi() ? L"Перенос по словам" : L"Word wrap",
			OnOffLabel(m_owner->IsWordWrapEnabled()),
			m_hoveredCard == CardWrap);
		DrawCard(
			hdc,
			m_cardRects[CardFormatting],
			IsRussianUi() ? L"Форматирование текста" : L"Text formatting",
			OnOffLabel(m_owner->IsFormattingEnabled()),
			m_hoveredCard == CardFormatting);
		DrawCard(
			hdc,
			m_cardRects[CardStatusBar],
			IsRussianUi() ? L"Строка состояния" : L"Status bar",
			OnOffLabel(m_owner->IsStatusBarVisible()),
			m_hoveredCard == CardStatusBar);
		DrawCard(
			hdc,
			m_cardRects[CardLanguage],
			IsRussianUi() ? L"Язык интерфейса" : L"Interface language",
			m_owner->GetLanguageDisplayName(),
			m_hoveredCard == CardLanguage);

		RECT rcInfoTitle = m_infoRect;
		rcInfoTitle.bottom = rcInfoTitle.top + Scale(36);
		::SetTextColor(hdc, RGB(240, 240, 240));
		const wchar_t* infoTitle = IsRussianUi() ? L"Информация о приложении" : L"App information";
		::DrawTextW(hdc, infoTitle, -1, &rcInfoTitle, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		RECT rcInfoBody = m_infoRect;
		rcInfoBody.top += Scale(40);
		rcInfoBody.bottom = rcInfoBody.top + Scale(160);
		::SetTextColor(hdc, RGB(195, 195, 195));
		const wchar_t* infoBody = IsRussianUi()
			? L"LockNote2\nСамомодифицирующийся шифрованный блокнот.\n\nКарточки слева переключают доступные параметры без потери текущей функциональности."
			: L"LockNote2\nSelf-modifying encrypted notepad.\n\nUse cards on the left to switch available settings while keeping existing LockNote2 behavior.";
		::DrawTextW(hdc, infoBody, -1, &rcInfoBody, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

		CBrush aboutBrush;
		aboutBrush.CreateSolidBrush(m_hoveredAboutButton ? RGB(70, 70, 76) : RGB(57, 57, 61));
		::FillRect(hdc, &m_aboutButtonRect, aboutBrush);
		::SetTextColor(hdc, RGB(238, 238, 238));
		RECT rcAboutText = m_aboutButtonRect;
		const wchar_t* aboutLabel = IsRussianUi() ? L"О приложении" : L"About app";
		::DrawTextW(hdc, aboutLabel, -1, &rcAboutText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);

		if (hOldFont != nullptr)
		{
			::SelectObject(hdc, hOldFont);
		}
	}

	int HitTestCard(const POINT ptClient) const
	{
		for (int i = 0; i < CardCount; ++i)
		{
			if (::PtInRect(&m_cardRects[i], ptClient))
			{
				return i;
			}
		}
		return -1;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CClientDC dc(m_hWnd);
		const int dpiY = GetDeviceCaps(dc, LOGPIXELSY);
		m_fontBase.CreateFont(
			-MulDiv(10, dpiY, 72),
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
		m_fontTitle.CreateFont(
			-MulDiv(28, dpiY, 72),
			0,
			0,
			0,
			FW_SEMIBOLD,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH,
			L"Segoe UI");
		RecalculateLayout();
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RecalculateLayout();
		Invalidate(FALSE);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps{};
		HDC hdc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient{};
		::GetClientRect(m_hWnd, &rcClient);
		const int width = rcClient.right - rcClient.left;
		const int height = rcClient.bottom - rcClient.top;
		if (width > 0 && height > 0)
		{
			HDC hdcMem = ::CreateCompatibleDC(hdc);
			HBITMAP hbmMem = ::CreateCompatibleBitmap(hdc, width, height);
			if (hdcMem != nullptr && hbmMem != nullptr)
			{
				HGDIOBJ hOldBitmap = ::SelectObject(hdcMem, hbmMem);
				DrawSettings(hdcMem);
				::BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
				::SelectObject(hdcMem, hOldBitmap);
			}
			else
			{
				DrawSettings(hdc);
			}
			if (hbmMem != nullptr)
			{
				::DeleteObject(hbmMem);
			}
			if (hdcMem != nullptr)
			{
				::DeleteDC(hdcMem);
			}
		}
		::EndPaint(m_hWnd, &ps);
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		const int hoveredCard = HitTestCard(pt);
		const bool hoveredAbout = ::PtInRect(&m_aboutButtonRect, pt) != FALSE;
		if (hoveredCard != m_hoveredCard || hoveredAbout != m_hoveredAboutButton)
		{
			m_hoveredCard = hoveredCard;
			m_hoveredAboutButton = hoveredAbout;
			Invalidate(FALSE);
		}

		TRACKMOUSEEVENT tme{ sizeof(tme) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);
		return 0;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_hoveredCard != -1 || m_hoveredAboutButton)
		{
			m_hoveredCard = -1;
			m_hoveredAboutButton = false;
			Invalidate(FALSE);
		}
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (m_owner == nullptr)
		{
			return 0;
		}

		const POINT pt{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};

		if (::PtInRect(&m_aboutButtonRect, pt))
		{
			::PostMessageW(m_owner->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_APP_ABOUT, 0), 0);
			return 0;
		}

		switch (HitTestCard(pt))
		{
		case CardTheme:
		{
			const ThemeMode current = CMainFrame::ParseThemeMode(m_owner->GetThemeMode());
			const ThemeMode next = current == ThemeMode::System
				? ThemeMode::Light
				: (current == ThemeMode::Light ? ThemeMode::Dark : ThemeMode::System);
			m_owner->ApplyThemeSetting(next);
			break;
		}
		case CardFont:
		{
			const std::array<std::string, 7> fonts{
				NAME_FONT_CONSOLAS,
				NAME_FONT_CASCADIA_CODE,
				NAME_FONT_LUCIDA_CONSOLE,
				NAME_FONT_COURIER_NEW,
				NAME_FONT_TAHOMA,
				NAME_FONT_VERDANA,
				NAME_FONT_ARIAL
			};
			auto it = std::find(fonts.begin(), fonts.end(), m_owner->m_strFontName);
			if (it == fonts.end() || ++it == fonts.end())
			{
				m_owner->ApplyFontFaceByName(fonts.front());
			}
			else
			{
				m_owner->ApplyFontFaceByName(*it);
			}
			break;
		}
		case CardWrap:
			m_owner->SetWordWrapEnabled(!m_owner->IsWordWrapEnabled());
			break;
		case CardFormatting:
			m_owner->SetFormattingEnabled(!m_owner->IsFormattingEnabled());
			break;
		case CardStatusBar:
			m_owner->SetStatusBarVisible(!m_owner->IsStatusBarVisible());
			break;
		case CardLanguage:
			m_owner->ApplyLanguageById(m_owner->IsRussianUi() ? LANG_ENGLISH : LANG_RUSSIAN);
			break;
		default:
			break;
		}

		Invalidate(FALSE);
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DestroyWindow();
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_fontBase.m_hFont)
		{
			m_fontBase.DeleteObject();
		}
		if (m_fontTitle.m_hFont)
		{
			m_fontTitle.DeleteObject();
		}
		if (m_owner != nullptr)
		{
			m_owner->OnSettingsWindowClosed();
		}
		delete this;
		return 0;
	}
};

inline void CMainFrame::OpenSettingsWindow()
{
	if (m_settingsWindow != nullptr && ::IsWindow(m_settingsWindow->m_hWnd))
	{
		m_settingsWindow->ShowWindow(SW_SHOW);
		::SetForegroundWindow(m_settingsWindow->m_hWnd);
		return;
	}

	CSettingsWindow* settingsWindow = new CSettingsWindow(this);
	if (settingsWindow == nullptr)
	{
		return;
	}

	const HWND hSettings = settingsWindow->Create(
		m_hWnd,
		CWindow::rcDefault,
		IsRussianUi() ? L"Настройки - LockNote2" : L"Settings - LockNote2",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	if (hSettings == nullptr)
	{
		delete settingsWindow;
		return;
	}

	CClientDC dc(*this);
	const int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
	const int width = MulDiv(1160, dpiX, 96);
	const int height = MulDiv(760, dpiX, 96);

	RECT rcMain{};
	::GetWindowRect(m_hWnd, &rcMain);
	::SetWindowPos(
		hSettings,
		nullptr,
		rcMain.left + MulDiv(24, dpiX, 96),
		rcMain.top + MulDiv(24, dpiX, 96),
		width,
		height,
		SWP_NOZORDER | SWP_SHOWWINDOW);

	m_settingsWindow = settingsWindow;
}

inline void CMainFrame::CloseSettingsWindow()
{
	if (m_settingsWindow != nullptr && ::IsWindow(m_settingsWindow->m_hWnd))
	{
		m_settingsWindow->DestroyWindow();
	}
}

inline void CMainFrame::RefreshSettingsWindow()
{
	if (m_settingsWindow != nullptr && ::IsWindow(m_settingsWindow->m_hWnd))
	{
		::SetWindowTextW(
			m_settingsWindow->m_hWnd,
			IsRussianUi() ? L"Настройки - LockNote2" : L"Settings - LockNote2");
		::InvalidateRect(m_settingsWindow->m_hWnd, nullptr, FALSE);
	}
}
