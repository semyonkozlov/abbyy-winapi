#pragma once

#include <Tchar.h>
#include <Windows.h>

#include <string>

using CString = std::basic_string<TCHAR>;

CString GetErrorText( DWORD errorCode );

void ShowLastError();