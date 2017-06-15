#include <string>
#include <fstream>
#include <iterator>
#include <functional>

#include <Windows.h>

const wchar_t* LibPath = L"WordsCountDll.dll";

int main( int argc, char* argv[] )
{
    HMODULE wordsCountDll = LoadLibrary( LibPath );
    auto wordsCount = reinterpret_cast<int (*)( const wchar_t* )>( GetProcAddress( wordsCountDll, "WordsCount" ) ); 

    int argCount;
    LPWSTR* argVector = CommandLineToArgvW( GetCommandLine(), &argCount );
    if( argCount > 1 ) {
        std::wifstream file( argVector[1] );
        std::wstring fileContent( std::istreambuf_iterator<wchar_t>( file ), {} );
        int numWords = wordsCount( fileContent.c_str() );
        MessageBox( NULL, std::to_wstring( numWords ).c_str(), L"WordsCount", MB_DEFAULT_DESKTOP_ONLY );
    }

    FreeLibrary( wordsCountDll );
    return EXIT_SUCCESS;
}