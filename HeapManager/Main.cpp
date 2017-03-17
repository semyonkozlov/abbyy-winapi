#include <gtest/gtest.h>

const int HeapInitSize = 1'000;
const int HeapMaxSize = 1'000'000;
const int NumAllocs = 50;

int main( int argc, char** argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}