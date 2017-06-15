#include <string>
#include <fstream>
#include <iterator>

#include <Windows.h>

#include "WordsCount.h"

#pragma comment( lib, "C:/Users/Semyon/Documents/Visual Studio 2017/Projects/AbbyyWinapi/x64/Debug/WordsCountDll.dll" )

int main( int argc, char* argv[] )
{
    int argCount;
    LPWSTR* argVector = CommandLineToArgvW( GetCommandLine(), &argCount );
    if( argCount > 1 ) {
        std::wifstream file( argVector[1] );
        std::wstring fileContent( std::istreambuf_iterator<wchar_t>( file ), {} );
        int numWords = WordsCount( fileContent.c_str() );
        MessageBox( NULL, std::to_wstring( numWords ).c_str(), L"WordsCount", MB_DEFAULT_DESKTOP_ONLY );
    }

    return EXIT_SUCCESS;
}
