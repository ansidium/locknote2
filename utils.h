// Steganos LockNote - self-modifying encrypted notepad
// Copyright (C) 2006-2010 Steganos GmbH
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

#include "utf8unicode.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <cstdio>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "cryptopp/misc.h"

std::string GetPasswordDlg(HWND hWnd = nullptr);
std::string GetNewPasswordDlg(HWND hWnd = nullptr);

typedef struct wintraits_t
{
	int m_nWindowSizeX;
	int m_nWindowSizeY;
	int m_nFontSize;
	int m_nLangId;
	int m_nKdfMode;
	std::string m_strFontName;
} LOCKNOTEWINTRAITS, *LPLOCKNOTEWINTRAITS;

namespace Utils
{
	using namespace CryptoPP;

	inline HMODULE GetModuleHandle(void)
	{
		return ::GetModuleHandle(nullptr);
	}

	inline std::string STR(UINT nResourceID)
	{
		std::array<char, 16384> buffer{};
		LoadStringA(GetModuleHandle(), nResourceID, buffer.data(), static_cast<int>(buffer.size()));
		return buffer.data();
	}

	inline std::wstring WSTR(UINT nResourceID)
	{
		std::array<char, 16384> buffer{};
		LoadStringA(GetModuleHandle(), nResourceID, buffer.data(), static_cast<int>(buffer.size()));
		return utf8_to_wstring(buffer.data());
	}

	inline int MessageBox(HWND hWnd, const std::wstring& text, UINT uType)
	{
		return ::MessageBox(hWnd, text.c_str(), WSTR(IDR_MAINFRAME).c_str(), uType);
	}

	inline std::string Quote(const std::string& strText)
	{
		return std::string("\"") + strText + std::string("\"");
	}

	// lower a std::string. from https://en.cppreference.com/w/cpp/string/byte/tolower
	inline std::string str_tolower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); }
		);
		return s;
	}

	inline bool HasExtension(const std::string& path, const std::string& extension)
	{
		if (path.size() < extension.size())
		{
			return false;
		}

		const std::string pathExtension = str_tolower(path.substr(path.size() - extension.size()));
		return pathExtension == str_tolower(extension);
	}

	inline bool TryParseInt(const std::string& value, int& out)
	{
		if (value.empty())
		{
			return false;
		}

		out = 0;
		const char* begin = value.data();
		const char* end = value.data() + value.size();
		const auto [parsedUntil, ec] = std::from_chars(begin, end, out);
		return ec == std::errc{} && parsedUntil == end;
	}

		inline AESLayer::KdfMode ParseKdfModeValue(const int value)
		{
			return value == static_cast<int>(AESLayer::KdfMode::Pbkdf2Sha256)
				? AESLayer::KdfMode::Pbkdf2Sha256
				: AESLayer::KdfMode::Scrypt;
		}

		inline bool ConstantTimeEquals(const std::string_view lhs, const std::string_view rhs)
		{
			if (lhs.size() != rhs.size())
			{
				return false;
			}
			return VerifyBufsEqual(
				reinterpret_cast<const byte*>(lhs.data()),
				reinterpret_cast<const byte*>(rhs.data()),
				lhs.size());
		}

	inline bool UpdateResource(
		const std::string& strExePath,
		const std::string& strResourceName,
		const std::string& strResourceSection,
		const std::vector<unsigned char>& arrayBuffer)
	{
		if (arrayBuffer.size() > static_cast<size_t>((std::numeric_limits<DWORD>::max)()))
		{
			return false;
		}

		const std::wstring exePath = utf8_to_wstring(strExePath);
		const std::wstring resourceName = utf8_to_wstring(strResourceName);
		const std::wstring resourceSection = utf8_to_wstring(strResourceSection);
		if (exePath.empty() || resourceName.empty() || resourceSection.empty())
		{
			return false;
		}

		bool bResult = false;
		HANDLE hFile = ::BeginUpdateResourceW(exePath.c_str(), FALSE);
		if (hFile)
		{
			void* resourceData = arrayBuffer.empty() ? nullptr : const_cast<unsigned char*>(arrayBuffer.data());
			bResult = ::UpdateResourceW(
				hFile,
				resourceSection.c_str(),
				resourceName.c_str(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				resourceData,
				static_cast<DWORD>(arrayBuffer.size())) ? true : false;

			if (!::EndUpdateResourceW(hFile, bResult ? FALSE : TRUE))
			{
				bResult = false;
			}
		}
		return bResult;
	}

	inline bool UpdateResource(const std::string& strExePath, const std::string& strResourceName, const std::string& strResourceSection, const std::string& strText)
	{
		std::vector<unsigned char> arrayBuffer;
		size_t dwSize = (strText.size() + 1) * sizeof(char);
		arrayBuffer.resize(dwSize);
		std::memcpy(arrayBuffer.data(), strText.c_str(), dwSize);
		return UpdateResource(strExePath, strResourceName, strResourceSection, arrayBuffer);
	}

	inline bool LoadResource(const std::string& strResourceName, const std::string& strResourceSection, std::vector<unsigned char>& arrayBuffer, HMODULE hModule = GetModuleHandle())
	{
		bool bResult = false;
		HRSRC hResInfo = ::FindResourceA(hModule, strResourceName.c_str(), strResourceSection.c_str());
		if (hResInfo)
		{
			DWORD dwSize = ::SizeofResource(hModule, hResInfo);
			HGLOBAL hRes = ::LoadResource(hModule, hResInfo);
			if (hRes && dwSize)
			{
				void* pData = ::LockResource(hRes);
				if (pData)
				{
					arrayBuffer.resize(dwSize, 0);
					std::memcpy(arrayBuffer.data(), pData, dwSize);
					bResult = true;
				}
			}
		}
		return bResult;
	}

	inline bool LoadResource(const std::string& strResourceName, const std::string& strResourceSection, std::string& strText, HMODULE hModule = GetModuleHandle())
	{
		bool bResult = false;
		std::vector<unsigned char> arrayBuffer;
		bResult = LoadResource(strResourceName, strResourceSection, arrayBuffer, hModule);
		if (bResult)
		{
			if (arrayBuffer.empty())
			{
				strText.clear();
				return true;
			}

			arrayBuffer.back() = '\0';
			strText = reinterpret_cast<char*>(arrayBuffer.data());
		}
		return bResult;
	}

	inline bool EncryptString(
		const std::string& strText,
		const std::string& strPassword,
		std::string& strEncryptedData,
		const AESLayer::KdfMode kdfMode = AESLayer::KdfMode::Scrypt)
	{
		AutoSeededRandomPool rng;
		AESLayer aes;
		std::vector<byte> cipher(aes.MaxCiphertextLen(static_cast<unsigned int>(strText.length())), 0);
		AESLayer::EncryptionOptions options;
		options.m_kdfMode = kdfMode;
		const unsigned int cipherLen = aes.Encrypt(rng, strPassword, cipher.data(), strText, options);

		strEncryptedData.clear();
		HexEncoder hex(new StringSink(strEncryptedData));
		hex.Put(cipher.data(), cipherLen);
		hex.MessageEnd();
		return true;
	}

	inline bool DecryptString(const std::string& strEncryptedData, const std::string& strPassword, std::string& strText)
	{
		strText.clear();
		if ((strEncryptedData.size() % 2) != 0)
		{
			return false;
		}

		try
		{
			AESLayer aes;
			const size_t cipherTextLength = strEncryptedData.size() / 2;
			std::vector<byte> cipher(cipherTextLength, 0);

			HexDecoder hex(new ArraySink(cipher.data(), cipher.size()));
			hex.Put(reinterpret_cast<const byte*>(strEncryptedData.data()), strEncryptedData.size());
			hex.MessageEnd();

			ConstByteArrayParameter cbar(static_cast<const byte*>(cipher.data()), cipherTextLength);
			std::vector<byte> plainText(cipherTextLength, 0);
			const DecodingResult result = aes.Decrypt(strPassword, plainText.data(), cbar);
			if (!result.isValidCoding)
			{
				return false;
			}

			strText.assign(reinterpret_cast<const char*>(plainText.data()), result.messageLength);
			SecureWipeBuffer(plainText.data(), plainText.size());
			return true;
		}
		catch (const Exception&)
		{
			return false;
		}
	}

	inline bool WriteWinTraitsResources(const std::string& path, const LOCKNOTEWINTRAITS& wintraits)
	{
		bool result = true;
		result &= UpdateResource(path, "SIZEX", "INFORMATION", std::to_string(wintraits.m_nWindowSizeX));
		result &= UpdateResource(path, "SIZEY", "INFORMATION", std::to_string(wintraits.m_nWindowSizeY));
		result &= UpdateResource(path, "FONTSIZE", "INFORMATION", std::to_string(wintraits.m_nFontSize));
		result &= UpdateResource(path, "TYPEFACE", "INFORMATION", wintraits.m_strFontName);
		result &= UpdateResource(path, "KDFMODE", "INFORMATION", std::to_string(wintraits.m_nKdfMode));
		if (wintraits.m_nLangId != 0)
		{
			result &= UpdateResource(path, "LANGID", "INFORMATION", std::to_string(wintraits.m_nLangId));
		}
		return result;
	}

	inline bool LoadTextFromFile(const std::string& path, std::string& text, std::string& password)
	{
		text.clear();
		password.clear();

		if (!HasExtension(path, ".txt"))
		{
			return false;
		}

		const std::wstring widePath = utf8_to_wstring(path);
		if (widePath.empty())
		{
			return false;
		}

		FILE* f = nullptr;
		const errno_t err = _wfopen_s(&f, widePath.c_str(), L"rb");
		if (err != 0 || f == nullptr)
		{
			return false;
		}

		if (_fseeki64(f, 0, SEEK_END) != 0)
		{
			fclose(f);
			return false;
		}

		const __int64 fileSize = _ftelli64(f);
		if (fileSize < 0 || fileSize > static_cast<__int64>((std::numeric_limits<size_t>::max)()) || _fseeki64(f, 0, SEEK_SET) != 0)
		{
			fclose(f);
			return false;
		}

		const size_t fileSizeAsSizeT = static_cast<size_t>(fileSize);
		text.resize(fileSizeAsSizeT, '\0');
		if (fileSize > 0)
		{
			const size_t readCount = fread(text.data(), sizeof(char), fileSizeAsSizeT, f);
			if (readCount != fileSizeAsSizeT)
			{
				fclose(f);
				return false;
			}
		}

		fclose(f);
		return true;
	}

	inline bool SaveTextToFile(const std::string& path, const std::string& text, std::string& password, HWND hWnd = 0, LPLOCKNOTEWINTRAITS wintraits = nullptr)
	{
		if (HasExtension(path, ".txt"))
		{
			const std::wstring widePath = utf8_to_wstring(path);
			if (widePath.empty())
			{
				return false;
			}

			FILE* f = nullptr;
			const errno_t err = _wfopen_s(&f, widePath.c_str(), L"wb");
			if (err != 0 || f == nullptr)
			{
				return false;
			}

			const size_t written = fwrite(text.data(), sizeof(char), text.size(), f);
			fclose(f);
			return written == text.size();
		}

		if (password.empty())
		{
			password = GetNewPasswordDlg();
			if (password.empty())
			{
				return false;
			}
		}

		std::string data = "";
		if (!text.empty())
		{
			AESLayer::KdfMode kdfMode = AESLayer::KdfMode::Scrypt;
			if (wintraits != nullptr)
			{
				kdfMode = ParseKdfModeValue(wintraits->m_nKdfMode);
			}
			Utils::EncryptString(text, password, data, kdfMode);
		}
		else
		{
			Utils::MessageBox(hWnd, WSTR(IDS_TEXT_IS_ENCRYPTED), MB_OK | MB_ICONINFORMATION);
		}

		std::array<wchar_t, MAX_PATH> modulePath{};
		const DWORD modulePathLength = ::GetModuleFileNameW(Utils::GetModuleHandle(), modulePath.data(), static_cast<DWORD>(modulePath.size()));
		if (modulePathLength == 0 || modulePathLength >= modulePath.size())
		{
			return false;
		}

		const std::wstring targetPath = utf8_to_wstring(path);
		if (targetPath.empty() || !::CopyFileW(modulePath.data(), targetPath.c_str(), FALSE))
		{
			return false;
		}

		bool bResult = Utils::UpdateResource(path, "CONTENT", "PAYLOAD", data);

		if (wintraits)
		{
			bResult &= WriteWinTraitsResources(path, *wintraits);
		}

		return bResult;
	}
}

using namespace Utils;
