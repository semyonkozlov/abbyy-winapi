#include "Utils.h"

CString GetErrorText( DWORD errorCode )
{
    const int errorTextLength = 256;
    TCHAR errorText[errorTextLength];

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        errorCode,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        errorText,
        errorTextLength,
        nullptr );

    return { errorText };
}

void ShowLastError()
{
    CString errorText = GetErrorText( GetLastError() );
    MessageBox( nullptr, errorText.c_str(), nullptr, MB_OK );
}