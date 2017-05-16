#include <Windows.h>

#include "Worker.h"

int main( int argc, char** argv ) 
{
    if ( argc < 2 ) {
        return EXIT_FAILURE;
    }

    CWorker worker( argv[1] );
    worker.Work();

    return EXIT_SUCCESS;
}