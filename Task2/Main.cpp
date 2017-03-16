#include <iostream>

#include <Windows.h>

#include "HeapManager.h"

int main( int argc, char* argv[] )
{
    CHeapManager heapManager( 100, 10'000'000 );
    char* str = (char*)heapManager.Alloc( 400'000 );
    std::strcpy(str, "allocator");
    std::cout << str << std::endl;

    char* str2 = (char*)heapManager.Alloc( 10'000 );
    std::strcpy( str2, "str2" );
    std::cout << str2 << std::endl;

    heapManager.Free( str );
    heapManager.Free( str2 );
    return 0;
}