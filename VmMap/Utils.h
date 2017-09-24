#pragma once

#include <string>

#include <Tchar.h>
#include <Windows.h>

using CString = std::basic_string<TCHAR>;

template<typename IntegerType>
CString IntToString( IntegerType i )
{
#ifdef UNICODE 
    return std::to_wstring( i );
#else
    return std::to_string( i );
#endif
}

CString GetErrorText( DWORD errorCode );
void ShowLastError();