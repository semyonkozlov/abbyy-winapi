#include <locale>
#include <codecvt>

#include "Utils.h"

std::string GetErrorMessage( DWORD errorCode )
{
    wchar_t errorMessage[256];
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), errorMessage, 255, nullptr );

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes( errorMessage );
}