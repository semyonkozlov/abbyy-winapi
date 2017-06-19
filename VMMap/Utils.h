#pragma once

#include <string>

#include <Tchar.h>
#include <Windows.h>

using CString = std::basic_string<TCHAR>;

CString GetErrorString( DWORD errorCode );