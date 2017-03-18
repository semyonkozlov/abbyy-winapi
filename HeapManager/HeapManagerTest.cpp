#include <gtest/gtest.h>
#include <Windows.h>

#include "HeapManager.h"

class HeapManagerTest : public testing::Test {
protected:
    HeapManagerTest() : customHeap(), defaultHeap( nullptr )
    {
        GetSystemInfo( &systemInfo );
    }

    void SetUp() override
    {
        customHeap.Create( heapInitSize, heapMaxSize );
        defaultHeap = HeapCreate( HEAP_NO_SERIALIZE, heapInitSize, heapMaxSize );
    }
   
    void TearDown() override
    {
        HeapDestroy( defaultHeap );
        customHeap.Destroy();
    }
    
    CHeapManager customHeap;
    HANDLE defaultHeap;

    SYSTEM_INFO systemInfo;

    static const int heapInitSize = 1'000;
    static const int heapMaxSize = 1'000'000;
};

TEST_F( HeapManagerTest, AllocFree )
{
    const int numAllocs = 5'000;

    CHeapManager heapManager( heapInitSize, heapMaxSize );

    for( int i = 0; i < numAllocs; ++i ) {
        void* p = heapManager.Alloc( std::rand() % heapMaxSize );
        heapManager.Free( p );
    }

    EXPECT_EQ( heapManager.Size(), systemInfo.dwPageSize );
}

