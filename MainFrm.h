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
#include <atldlgs.h>
//#include <windows.h>
#include "utils.h"

inline constexpr const char* DEFAULT_FONT_NAME = NAME_FONT_CONSOLAS;
constexpr unsigned int DEFAULT_FONT_SIZE = 12;
constexpr unsigned int DEFAULT_WIDTH = 650;
constexpr unsigned int DEFAULT_HEIGHT = 500;

inline const UINT UWM_FINDMSGSTRING = CFindReplaceDialog::GetFindReplaceMsg();

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
	std::map<WORD, std::wstring> m_languages{ {LANG_ENGLISH, L"en_US"}, { LANG_GERMAN, L"de_DE" }, { LANG_FRENCH, L"fr_FR" }, { LANG_SPANISH, L"es_ES" }, { LANG_PORTUGUESE, L"pt_PT" }, { LANG_ITALIAN, L"it_IT" }, { LANG_DUTCH, L"nl_NL" }, { LANG_SWEDISH, L"sv_SE" }, { LANG_POLISH, L"pl_PL" }, { LANG_RUSSIAN, L"ru_RU" } };

	std::string m_strFontName{ DEFAULT_FONT_NAME };
	AESLayer::KdfMode m_kdfMode{ AESLayer::KdfMode::Scrypt };
	bool m_bTraitsChanged{ false };
	bool m_ignoreEditNotifications{ false };

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

		std::array<char, 512> statusText{};
		sprintf_s(
			statusText.data(),
			statusText.size(),
			STR(IDS_STATUSBAR_STATS).c_str(),
			static_cast<int>(num_lines),
			static_cast<int>(GetText().size()),
			static_cast<int>(current_line_number),
			static_cast<int>(column));
		const std::wstring wideStatusText = utf8_to_wstring(statusText.data());
		SendMessage(m_hWndStatusBar, WM_SETTEXT, wideStatusText.size() + 1, reinterpret_cast<LPARAM>(wideStatusText.c_str()));

		/*std::stringstream ss;
		ss << num_lines << _T(" lines, ") << GetText().size() << _T(" characters. Line: ");
		ss << current_line_number << _T(", column: ") << column << _T(".");
		SendMessage(m_hWndStatusBar, WM_SETTEXT, ss.str().size() + 1, (LPARAM)(ss.str().c_str()));*/
	}

	void CheckFontSize()
	{
		// check the correct menu item
		HMENU hMenu = this->GetMenu();
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
		HMENU hMenu = this->GetMenu();
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
		HMENU hMenu = this->GetMenu();
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
		Invalidate();
		::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		// menu bar needs special treatment
		DrawMenuBar();

		// set checkmark to selected language
		CheckLanguage(wID);

		// remember language to save it on exit
		m_nLanguage = wID;
		m_bTraitsChanged = true;

		return 0;
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
		bHandled = FALSE;
		if (m_nWindowSizeX != m_nOldWindowSizeX || m_nWindowSizeY != m_nOldWindowSizeY)
		{
			m_bTraitsChanged = true;
			UpdateStatusBar();
		}
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

		LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			m_dwSearchFlags = FR_DOWN;
			m_pCurrentFindReplaceDialog = nullptr;

			DragAcceptFiles(TRUE);

			CreateSimpleStatusBar(); //m_hWndStatusBar

			m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_WINDOWEDGE);
			m_view.SetLimitText(0x7ffffffe); // allow a lot of text to be entered

			CClientDC dc(*this);
			auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
			auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
			if (m_nWindowSizeX == DEFAULT_WIDTH && m_nWindowSizeY == DEFAULT_HEIGHT)
			{
				// resize window depending on dpi scale factor
				m_nWindowSizeX = MulDiv(m_nWindowSizeX, dpi, 96);
				m_nWindowSizeY = MulDiv(m_nWindowSizeY, dpi, 96);
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

			SetWindowPos(NULL, 0, 0, m_nWindowSizeX, m_nWindowSizeY, SWP_NOZORDER | SWP_NOMOVE);
			CenterWindow();

			m_view.SetFocus();

			m_view.SetSel(m_currentBuffer.m_nStartChar + 2, m_currentBuffer.m_nStartChar + 2);

			SetWindowText(utf8_to_wstring(windowTitle).c_str());

			UpdateStatusBar();

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
		ConfigureEncryptionMenu();
		UpdateEncryptionMenuChecks();
	}

	void ConfigureEncryptionMenu()
	{
		HMENU hMenu = GetMenu();
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
		DrawMenuBar();
	}

	void UpdateEncryptionMenuChecks()
	{
		HMENU hMenu = GetMenu();
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
		HMENU hMenu = GetMenu();
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
		HMENU hMenu = GetMenu();
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
		HMENU hMenu = GetMenu();
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
