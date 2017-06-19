#include <iostream>

#include <Tchar.h>
#include <Windows.h>

#include "Utils.h"

int WINAPI _tWinMain( 
    HINSTANCE instance, HINSTANCE prevInstance,
    LPTSTR cmdLine, int cmdShow )
{
    auto p = VirtualAlloc( nullptr, 10000000000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE );
    MessageBox( nullptr, GetErrorString( GetLastError() ).c_str(), TEXT( "Message" ), MB_ICONWARNING );
}