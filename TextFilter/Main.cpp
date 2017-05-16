#include <Windows.h>

#include "TextFilter.h"

int main( int argc, char** argv )
{   
    if( argc < 4 ) {
        return EXIT_FAILURE;
    }

    CTextFilter textFilter( argv[1] );

    HANDLE inputFile = CreateFile(
        argv[2],
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr );

    HANDLE outputFile = CreateFile(
        argv[3],
        FILE_APPEND_DATA,
        0,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr );

    textFilter.Filter( inputFile, outputFile );

    return EXIT_SUCCESS;
} 