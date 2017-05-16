#pragma once

#include <iostream>

#include <Windows.h>

#include "Utils.h"

std::string GetLastErrorMessage()
{
    int errorCode = GetLastError();
    char errorMessage[256];
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), errorMessage, 255, nullptr );

    return errorMessage;
}

std::string AddId( const char* str, int id )
{
    return std::string( str ) + std::to_string( id );
}