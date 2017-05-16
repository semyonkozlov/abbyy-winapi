#include <Windows.h>

#include "TextFilter.h"

int main( int argc, char** argv )
{   
    if( argc < 4 ) {
        return EXIT_FAILURE;
    }

    CTextFilter textFilter( argv[1] );

    textFilter.Filter( argv[2], argv[3] );

    return EXIT_SUCCESS;
} 