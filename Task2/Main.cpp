#include <iostream>

#include <gtest/gtest.h>
#include <Windows.h>

#include "HeapManager.h"

TEST( HeapManagerTest, AllocFree )
{
    std::cout << "All right" << std::endl;
}

int main( int argc, char** argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}