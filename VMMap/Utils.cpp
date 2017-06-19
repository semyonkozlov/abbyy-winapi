#include "Utils.h"

CString GetErrorString( DWORD errorCode )
{
    const int messageStringSize = 256;
    TCHAR errorMessage[messageStringSize];

    FormatMessage( 
        FORMAT_MESSAGE_FROM_SYSTEM, 
        nullptr,
        errorCode,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
        errorMessage, 
        messageStringSize,
        nullptr );

    return { errorMessage };
}
