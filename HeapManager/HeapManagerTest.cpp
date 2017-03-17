#include <gtest/gtest.h>

#include "HeapManager.h"

const int HeapInitSize = 1'000;
const int HeapMaxSize = 1'000'000;
const int NumAllocs = 50;

TEST(HeapManagerTest, AllocFree)
{
    CHeapManager heapManager( HeapInitSize, HeapMaxSize );
    for( int i = 0; i < NumAllocs; ++i ) {
        void* p = heapManager.Alloc( std::rand() % HeapMaxSize );
        heapManager.Free( p );
    }
}