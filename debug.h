#pragma once

#if defined(DEBUG) | defined(_DEBUG)

#include <Windows.h>
#include <sstream>

static HRESULT _dbghr;

static void _DebugMessage(HRESULT hr, const char* file, int line)
{
	WCHAR* msgBuffer = nullptr;

	DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	DWORD language = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	DWORD msgLength = FormatMessage(formatFlags, 0, hr, language, reinterpret_cast<LPWSTR>(&msgBuffer), 0, nullptr);

	if (msgLength == 0)
		msgBuffer = const_cast<LPWSTR>(L"Unknown error");

	std::wstringstream sstr;
	sstr << msgBuffer << '\n' << file << ':' << line;

	MessageBox(nullptr, sstr.str().c_str(), L"Assertion failed!", MB_ICONSTOP | MB_OK);

	if (msgLength > 0)
		LocalFree(msgBuffer);
}

#define GFX_ASSERT(hr) if (FAILED(_dbghr = (hr))) _DebugMessage(_dbghr, __FILE__, __LINE__)

#else

#define GFX_ASSERT(hr) hr

#endif
