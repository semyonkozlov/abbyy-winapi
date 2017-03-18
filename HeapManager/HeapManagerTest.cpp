#include <chrono>

#include <gtest/gtest.h>
#include <Windows.h>

#include "HeapManager.h"

class HeapManagerTest : public testing::Test {
protected:
    class CClassWithCustomHeap;
    class CClassWithDefaultHeap;

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

    static CHeapManager customHeap;
    static HANDLE defaultHeap;

    static const int heapInitSize = 4'000;
    static const int heapMaxSize = 1'000'000'000;
    static const int allocatedClassSize = 3;

    SYSTEM_INFO systemInfo;
};

CHeapManager HeapManagerTest::customHeap;
HANDLE HeapManagerTest::defaultHeap = nullptr;

class HeapManagerTest::CClassWithDefaultHeap {
public:
    void* operator new( std::size_t size )
    {
        return HeapAlloc( defaultHeap, 0, size );
    }

    void operator delete( void* mem )
    {
        HeapFree( defaultHeap, 0, mem );
    }

private:
    BYTE arr[allocatedClassSize];
}; 

class HeapManagerTest::CClassWithCustomHeap {
public:
    void* operator new( std::size_t size )
    {
        return customHeap.Alloc( size );
    }

    void operator delete( void* mem )
    {
        customHeap.Free( mem );
    }

private:
    BYTE arr[allocatedClassSize];
};


template<typename T>
auto AllocFreeTime( int numSeries, int numPushBacksInSerie ) 
{
    auto start = std::chrono::steady_clock::now();

    std::vector<T> vector;
    for( int i = 0; i < numSeries; ++i ) {
        for( int j = 0; j < numPushBacksInSerie; ++j ) {
            vector.emplace_back();
        }
        vector.shrink_to_fit();
    }

    auto end = std::chrono::steady_clock::now();
    return end - start;
}

TEST_F( HeapManagerTest, AllocFree )
{
    const int numSeries = 1'000;
    const int numPushBacksInSerie = 10'000;
    auto defaultHeapTime = AllocFreeTime<CClassWithDefaultHeap>( numSeries, numPushBacksInSerie );
    auto customHeapTime = AllocFreeTime<CClassWithCustomHeap>( numSeries, numPushBacksInSerie );

    std::cout << defaultHeapTime.count() << ' ' << customHeapTime.count() << std::endl;

    EXPECT_LE( customHeapTime, 10 * defaultHeapTime );
    EXPECT_EQ( customHeap.CommittedMemorySize(), systemInfo.dwPageSize );
}