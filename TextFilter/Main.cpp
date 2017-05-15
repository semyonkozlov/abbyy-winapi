#include <iostream>

#define CONC( str, id ) (str#id)

int wmain()
{   
    std::cout << CONC( "kakakak", 6 ) << std::endl;
    return EXIT_SUCCESS;
} 