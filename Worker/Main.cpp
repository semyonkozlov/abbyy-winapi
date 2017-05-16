#include <string>

#include <Windows.h>

#include "Worker.h"

int main( int argc, char** argv ) 
{
    if ( argc < 3 ) {
        return EXIT_FAILURE;
    }

    CWorker worker( argv[1], std::stoi( argv[2] ) );
    worker.Work();

    return EXIT_SUCCESS;
}