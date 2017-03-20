#include <chrono>
#include <list>
#include <random>

#include <gtest/gtest.h>
#include <Windows.h>

#include "HeapManager.h"

class HeapManagerTest : public testing::Test {
protected:
    template<typename T>
    class IAllocator;

    template<typename T>
    class CDefaultHeapAllocator;

    template<typename T>
    class CCustomHeapAllocator;

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
    static const int allocatedClassSize = 8;

    SYSTEM_INFO systemInfo;
};

CHeapManager HeapManagerTest::customHeap;
HANDLE HeapManagerTest::defaultHeap = nullptr;

template<typename T>
class HeapManagerTest::IAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    IAllocator() noexcept = default;
    IAllocator( const IAllocator& ) noexcept = default;
    virtual ~IAllocator() noexcept = default;

    pointer address( reference value ) const
    {
        return &value;
    }
    const_pointer address( const_reference value ) const
    {
        return &value;
    }

    size_type max_size() const noexcept
    {
        return heapMaxSize / sizeof( T );
    }

    void construct( pointer p, const_reference value )
    {
        new (reinterpret_cast<void*>( p )) T( value );
    }

    void destroy( pointer p )
    {
        p->~T();
    }

    virtual pointer allocate( size_type num, const void* = nullptr ) = 0;
    virtual void deallocate( pointer p, size_type num ) = 0;
};

template<typename T>
class HeapManagerTest::CDefaultHeapAllocator : public IAllocator<T> {
public:
    using IAllocator<T>::value_type;
    using IAllocator<T>::pointer;

    CDefaultHeapAllocator() noexcept = default;

    template<typename U>
    struct rebind {
        using other = CDefaultHeapAllocator<U>;
    };

    template<typename U>
    explicit CDefaultHeapAllocator( const CDefaultHeapAllocator<U>& ) noexcept
    {
    }

    pointer allocate( size_type num, const void* = nullptr ) override
    {
        std::cout << num << std::endl;
        return static_cast<pointer>( HeapAlloc( defaultHeap, 0, num * sizeof( T ) ) );
    }

    void deallocate( pointer p, size_type num ) override
    {
        HeapFree( defaultHeap, 0, reinterpret_cast<void*>( p ) );
    }
};

template<typename T>
class HeapManagerTest::CCustomHeapAllocator : public IAllocator<T> {
public:
    using IAllocator<T>::value_type;
    using IAllocator<T>::pointer;

    CCustomHeapAllocator() noexcept = default;

    template<typename U>
    struct rebind {
        using other = CCustomHeapAllocator<U>;
    };

    template<typename U>
    explicit CCustomHeapAllocator( const CCustomHeapAllocator<U>& ) noexcept
    {
    }

    pointer allocate( size_type num, const void* = nullptr ) override
    {
        return static_cast<pointer>( customHeap.Alloc( num * sizeof( T ) ) );
    }

    void deallocate( pointer p, size_type num ) override 
    {
        customHeap.Free( p );
    }
};

template<template<typename> class Allocator, typename T1, typename T2>
bool operator==( const Allocator<T1>&, const Allocator<T2>& ) noexcept
{
    return true;
}

template<template<typename> class Allocator, typename T1, typename T2>
bool operator!=( const Allocator<T1>&, const Allocator<T2>& ) noexcept
{
    return false;
}


template<typename T, template<typename> class Allocator>
auto TestTime( int numSeries, int numEmplacesInSerie ) 
{
    auto start = std::chrono::steady_clock::now();

    // various memory usage cases
    std::vector<T, Allocator<T>> vector; 
    std::list<T, Allocator<T>> list;
    for( int i = 0; i < numSeries; ++i ) {
        for( int j = 0; j < numEmplacesInSerie; ++j ) {
            vector.emplace_back();
            //list.emplace_back();
        }
 
        for( int j = 0; j < numEmplacesInSerie / 3; ++j ) {
            vector.pop_back();
            list.erase( std::begin( list ) );
        }
    }

    auto end = std::chrono::steady_clock::now();
    return end - start;
}

TEST_F( HeapManagerTest, MemoryManagementTime )
{
    const int numSeries = 10;
    const int numEmplacesInSerie = 10;

    auto defaultHeapTime = TestTime<BYTE, CDefaultHeapAllocator>( numSeries, numEmplacesInSerie );
    // auto customHeapTime = TestTime<int, CCustomHeapAllocator>( numSeries, numEmplacesInSerie );

#ifdef _DEBUG
    //std::cout << customHeapTime.count() << ' ' << defaultHeapTime.count() << std::endl;
#endif 

    //EXPECT_LE( customHeapTime, 10 * defaultHeapTime );
    EXPECT_EQ( customHeap.CommittedMemorySize(), systemInfo.dwPageSize );
}

TEST_F( HeapManagerTest, DISABLED_MemoryUsage )
{
    const int numEmplaces = 1'000;

    std::vector<int, CCustomHeapAllocator<int>> vector;
    for( int i = 0; i < numEmplaces; ++i ) {
        EXPECT_LE( customHeap.CommittedMemorySize(), 
            vector.capacity() * sizeof( int ) + systemInfo.dwPageSize );
        vector.emplace_back();
        std::cout << customHeap.CommittedMemorySize() << std::endl;
    }

    std::vector<int, CCustomHeapAllocator<int>>().swap( vector ); // clear vector buffer
    EXPECT_NEAR( customHeap.CommittedMemorySize(), heapInitSize, systemInfo.dwPageSize );
}

TEST_F( HeapManagerTest, DISABLED_Overflow )
{
    EXPECT_THROW( (std::vector<BYTE, CCustomHeapAllocator<BYTE>>( heapMaxSize + 2 * systemInfo.dwAllocationGranularity )), 
        std::bad_alloc );
}