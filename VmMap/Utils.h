#pragma once

#include <string>

#include <Tchar.h>
#include <Windows.h>

using CString = std::basic_string<TCHAR>;
using CStringStream = std::basic_stringstream<TCHAR>;

CString GetErrorText( DWORD errorCode );
void ShowLastError();