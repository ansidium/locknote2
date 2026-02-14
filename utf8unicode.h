#ifndef _UTF8UNICODE
#define _UTF8UNICODE

#pragma once

#include <algorithm>
#include <string>

#include <windows.h>

std::string wchar_to_utf8(LPCWSTR lpString, bool bIncludeZero = false)
{
	if (lpString == nullptr)
	{
		return {};
	}

	const int requiredChars = WideCharToMultiByte(
		CP_UTF8,
		0,
		lpString,
		-1,
		nullptr,
		0,
		nullptr,
		nullptr);
	if (requiredChars <= 0)
	{
		return {};
	}

	std::string result(static_cast<size_t>(requiredChars), '\0');
	const int writtenChars = WideCharToMultiByte(
		CP_UTF8,
		0,
		lpString,
		-1,
		result.data(),
		requiredChars,
		nullptr,
		nullptr);
	if (writtenChars <= 0)
	{
		return {};
	}

	if (!bIncludeZero && !result.empty() && result.back() == '\0')
	{
		result.pop_back();
	}
	return result;
}

std::string wstring_to_utf8(const std::wstring& str, bool bIncludeZero = false)
{
	return wchar_to_utf8(str.c_str(), bIncludeZero);
}

std::wstring utf8_to_wstring(LPCSTR lpString)
{
	if (lpString == nullptr)
	{
		return {};
	}

	const int requiredChars = MultiByteToWideChar(
		CP_UTF8,
		0,
		lpString,
		-1,
		nullptr,
		0);
	if (requiredChars <= 0)
	{
		return {};
	}

	std::wstring result(static_cast<size_t>(requiredChars), L'\0');
	const int writtenChars = MultiByteToWideChar(
		CP_UTF8,
		0,
		lpString,
		-1,
		result.data(),
		requiredChars);
	if (writtenChars <= 0)
	{
		return {};
	}

	if (!result.empty() && result.back() == L'\0')
	{
		result.pop_back();
	}
	return result;
}

std::wstring utf8_to_wstring(const std::string& s)
{
	return utf8_to_wstring(s.c_str());
}

// buffer must be deleted after use
LPWSTR utf8_to_wchar(LPCSTR lpString)
{
	const std::wstring wide = utf8_to_wstring(lpString);
	LPWSTR buffer = new WCHAR[wide.size() + 1];
	std::fill(buffer, buffer + wide.size() + 1, L'\0');
	if (!wide.empty())
	{
		std::copy(wide.begin(), wide.end(), buffer);
	}
	return buffer;
}

#endif // _UTF8UNICODE
