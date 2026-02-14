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

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <process.h>
#include <array>

#include <atlfile.h>

#include "resource.h"

#include "locknoteView.h"
#include "AboutDlg.h"
#include "PasswordDlg.h"
#include "MainFrm.h"

#include "aeslayer.cpp" // prevents having to include precompiled header in aeslayer.cpp

CAppModule _Module;

namespace
{
	bool StageWritebackFromMainFrame(const CMainFrame& wndMain, std::string password)
	{
		std::string encryptedData;
		if (!wndMain.m_text.empty())
		{
			Utils::EncryptString(
				wndMain.m_text,
				password,
				encryptedData,
				Utils::ParseKdfModeValue(wndMain.GetKdfMode()));
		}
		else
		{
			Utils::MessageBox(nullptr, WSTR(IDS_TEXT_IS_ENCRYPTED), MB_OK | MB_ICONINFORMATION);
		}

		std::array<char, MAX_PATH> modulePath{};
		std::array<char, MAX_PATH> tempPath{};
		std::array<char, MAX_PATH> fileName{};

		::GetModuleFileNameA(Utils::GetModuleHandle(), modulePath.data(), static_cast<DWORD>(modulePath.size()));
		::GetTempPathA(static_cast<DWORD>(tempPath.size()), tempPath.data());
		::GetTempFileNameA(tempPath.data(), "STG", 0, fileName.data());
		if (!::CopyFileA(modulePath.data(), fileName.data(), FALSE))
		{
			return false;
		}

		bool writeResult = Utils::UpdateResource(fileName.data(), "CONTENT", "PAYLOAD", encryptedData);

		LOCKNOTEWINTRAITS traits{};
		traits.m_nWindowSizeX = wndMain.m_nWindowSizeX;
		traits.m_nWindowSizeY = wndMain.m_nWindowSizeY;
		traits.m_nFontSize = wndMain.m_nFontSize;
		traits.m_nLangId = wndMain.GetLanguage();
		traits.m_nKdfMode = wndMain.GetKdfMode();
		traits.m_strFontName = wndMain.m_strFontName;
		writeResult &= Utils::WriteWinTraitsResources(fileName.data(), traits);

		if (!writeResult)
		{
			return false;
		}

		_tspawnl(
			_P_NOWAIT,
			utf8_to_wstring(fileName.data()).c_str(),
			utf8_to_wstring(Utils::Quote(fileName.data())).c_str(),
			_T("-writeback"),
			utf8_to_wstring(Utils::Quote(modulePath.data())).c_str(),
			nullptr);
		return true;
	}
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	TCHAR szModulePath[MAX_PATH] = {'\0'};
	::GetModuleFileName(Utils::GetModuleHandle(), szModulePath, MAX_PATH);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	if (__argc == 3)
	{
#ifdef _UNICODE
		TCHAR* lpszCommand = __wargv[1];
		TCHAR* lpszPath = __wargv[2];
#else
		char* lpszCommand = __argv[1];
		char* lpszPath = __argv[2];
#endif
		if (!_tcscmp(lpszCommand, _T("-writeback")))
		{
			while (!::CopyFile(szModulePath, lpszPath, FALSE))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			_tspawnl(_P_NOWAIT, lpszPath, utf8_to_wstring(Utils::Quote(wstring_to_utf8(lpszPath))).c_str(), _T("-erase"), utf8_to_wstring(Utils::Quote(wstring_to_utf8(szModulePath))).c_str(), NULL);

			return 0;
		}
		else if (!_tcscmp(lpszCommand, _T("-erase")))
		{
			while (PathFileExists(lpszPath) && !::DeleteFile(lpszPath))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			return 0;
		}
	}

	if (__argc > 1)
	{
		int nResult = Utils::MessageBox(NULL, WSTR(IDS_ASK_CONVERT_FILES), MB_YESNO | MB_ICONQUESTION);
		if (nResult == IDYES)
		{
			int nConverted = 0;
			std::string encryptPassword;
			for (int nIndex = 1; nIndex < __argc; nIndex++)
			{
				const std::string filename = __argv[nIndex];
				if (HasExtension(filename, ".txt"))
				{
					std::string newfilename = filename.substr(0, filename.size() - 4);
					newfilename += ".exe";
					std::string text;
					std::string password;
					if (LoadTextFromFile(filename, text, password))
					{
						if (SaveTextToFile(newfilename, text, encryptPassword))
						{
							nConverted += 1;
						}
					}
				}
				else
				{
					std::array<wchar_t, 512> message{};
					const std::wstring wideFilename = utf8_to_wstring(filename);
					swprintf_s(message.data(), message.size(), WSTR(IDS_CANT_CONVERT).c_str(), wideFilename.c_str());
					Utils::MessageBox(NULL, message.data(), MB_OK | MB_ICONERROR);
				}
			}

			if (nConverted)
			{
				std::array<wchar_t, 256> message{};
				swprintf_s(message.data(), message.size(), WSTR(IDS_CONVERT_DONE).c_str(), nConverted);
				Utils::MessageBox(NULL, message.data(), MB_OK | MB_ICONINFORMATION);
			}
		}

		return 0;
	}

	CHAR szFileMappingName[MAX_PATH];
	strcpy_s(szFileMappingName, MAX_PATH, wstring_to_utf8(szModulePath).c_str());
	CHAR* szChar = szFileMappingName;
	while (*szChar)
	{
		if ((*szChar == '\\')||(*szChar == ':'))
		{
			*szChar = '_';
		}
		szChar++;
	};

	CAtlFileMapping<HWND> fmSingleInstanceHWND;
	BOOL bAlreadyExisted = FALSE;
	fmSingleInstanceHWND.MapSharedMem(sizeof(HWND), utf8_to_wstring(szFileMappingName).c_str(), &bAlreadyExisted);
	if (bAlreadyExisted)
	{
		HWND hWndMain = *fmSingleInstanceHWND;
		::SetForegroundWindow(hWndMain);
		return 0;
	}

	std::string text;
	std::string data;
	std::string password;
	Utils::LoadResource("CONTENT", "PAYLOAD", data);
	if (!data.empty())
	{
		password = GetPasswordDlg();
		if (password.empty())
		{
			return -1;
		}
		if (!Utils::DecryptString(data, password, text))
		{
			MessageBox(NULL, WSTR(IDS_INVALID_PASSWORD), MB_OK | MB_ICONERROR);
			return -1;
		}		
	}
	else
	{
		text = STR(IDS_WELCOME);
	}

	CMainFrame wndMain;

	wndMain.m_password = password;
	wndMain.m_text = text;

	// get window sizes from resource
	std::string strSizeX;
	std::string strSizeY;
	Utils::LoadResource("SIZEX", "INFORMATION", strSizeX);
	Utils::LoadResource("SIZEY", "INFORMATION", strSizeY);
	int parsedValue = 0;
	if (Utils::TryParseInt(strSizeX, parsedValue))
	{
		wndMain.m_nWindowSizeX = parsedValue;
	}
	if (Utils::TryParseInt(strSizeY, parsedValue))
	{
		wndMain.m_nWindowSizeY = parsedValue;
	}

	// get font size
	std::string fontsize;
	Utils::LoadResource("FONTSIZE", "INFORMATION", fontsize);
	if (Utils::TryParseInt(fontsize, parsedValue))
	{
		wndMain.m_nFontSize = parsedValue;
	}
	else
	{
		wndMain.m_nFontSize = DEFAULT_FONT_SIZE;
	}

	// get font set
	std::string fontface;
	Utils::LoadResource("TYPEFACE", "INFORMATION", fontface);
	if (!fontface.empty())
	{
		// font typeface format: "Lucida Console" (example)
		wndMain.m_strFontName = fontface;
	}
	else
	{
		wndMain.m_strFontName = DEFAULT_FONT_NAME;
	}
	
	// get language id
	std::string language;
	Utils::LoadResource("LANGID", "INFORMATION", language);
	if (Utils::TryParseInt(language, parsedValue))
	{
		wndMain.SetLanguage(parsedValue);
	}
	else
	{
		wndMain.SetLanguage(0);
	}

	std::string kdfMode;
	Utils::LoadResource("KDFMODE", "INFORMATION", kdfMode);
	if (Utils::TryParseInt(kdfMode, parsedValue))
	{
		wndMain.SetKdfMode(parsedValue);
	}
	else
	{
		wndMain.SetKdfMode(static_cast<int>(AESLayer::KdfMode::Scrypt));
	}

	// create window
	if (wndMain.CreateEx() == nullptr)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	wndMain.CheckFontSize();
	wndMain.CheckFontTypeFace();

	*fmSingleInstanceHWND = wndMain.m_hWnd;

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	if (((wndMain.m_text != text) || (wndMain.m_password != password) || wndMain.m_bTraitsChanged) && (wndMain.m_password.size()))
	{
		password = wndMain.m_password;
		if (!StageWritebackFromMainFrame(wndMain, password))
		{
			Utils::MessageBox(nullptr, L"Saving changes failed.", MB_OK | MB_ICONERROR);
		}
	}

	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
