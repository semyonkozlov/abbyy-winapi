#include <chrono>
#include <random>

#include <gtest/gtest.h>
#include <Windows.h>

#include "HeapManager.h"

class HeapManagerTest : public testing::Test {
protected:
    HeapManagerTest() 
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

    static const int heapInitSize = 4'000;
    static const int heapMaxSize = 1'000'000'000;

    CHeapManager customHeap;
    HANDLE defaultHeap;

    SYSTEM_INFO systemInfo;
};

TEST_F( HeapManagerTest, MemoryManagementTime )
{
    const int numSeries = 100;
    const int numAllocsInSerie = 1'000;
    const int minBlockSize = 16;
    const int maxBlockSize = 500'000;
    const int deallocFrequency = 3;

    std::random_device randomDevice;
    std::default_random_engine engine( randomDevice() );
    std::uniform_int_distribution<int> randomInt( minBlockSize, maxBlockSize );

    std::vector<void*> allocatedMemory;

    auto start = std::chrono::steady_clock::now();
    for( int i = 0; i < numSeries; ++i ) {
        for( int j = 0; j < numAllocsInSerie; ++j ) {
            allocatedMemory.push_back( HeapAlloc( defaultHeap, 0, randomInt( engine ) ) );
        }
        for( auto& memory : allocatedMemory ) {
            if( randomInt( engine ) % deallocFrequency == 0 && memory != nullptr ) {
                HeapFree( defaultHeap, 0, memory );
                memory = nullptr;
            }
        }
    }
    for( auto& memory : allocatedMemory ) {
        if( memory != nullptr ) {
            HeapFree( defaultHeap, 0, memory );
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto defaultHeapTime = end - start;

    allocatedMemory.clear();

    start = std::chrono::steady_clock::now();
    for( int i = 0; i < numSeries; ++i ) {
        for( int j = 0; j < numAllocsInSerie; ++j ) {
            allocatedMemory.push_back( customHeap.Alloc( randomInt( engine ) ) );
        }
        for( auto& memory : allocatedMemory ) {
            if( randomInt( engine ) % deallocFrequency == 0 && memory != nullptr ) {
                customHeap.Free( memory );
                memory = nullptr;
            }
        }
    }
    for( auto& memory : allocatedMemory ) {
        if( memory != nullptr ) {
            customHeap.Free( memory );
        }
    }
    end = std::chrono::steady_clock::now();
    auto customHeapTime = end - start;

#ifdef _DEBUG
    std::cout << double( customHeapTime.count() ) / double( defaultHeapTime.count() ) << std::endl;
#endif 

    EXPECT_LE( customHeapTime, 10 * defaultHeapTime );
}

TEST_F( HeapManagerTest, DISABLED_MemoryUsage )
{

}

TEST_F( HeapManagerTest, DISABLED_Overflow )
{
  
}