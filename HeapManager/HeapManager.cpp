#pragma once

#include <exception>
#include <algorithm>

#include "HeapManager.h"
#include "Utils.h"

const CHeapManager::CSystemInfo CHeapManager::systemInfo;
const int CHeapManager::smallBlocksSizeLimit = systemInfo.dwPageSize;
const int CHeapManager::mediumBlocksSizeLimit = 32 * systemInfo.dwPageSize;

CHeapManager::CHeapManager( int initSize, int maxSize ) 
{
    Create( initSize, maxSize );
}

CHeapManager::~CHeapManager()
{
    if( heapBegin != nullptr ) {
        Destroy();
    }
}

void CHeapManager::Create( int initSize, int maxSize )
{
    if( maxSize < initSize ) {
        throw std::invalid_argument( "Initial size cannot be bigger than maximum size of the heap" );
    }
    
    initHeapSize = granularRound( systemInfo.dwPageSize, initSize );
    maxHeapSize = granularRound( systemInfo.dwAllocationGranularity, maxSize );

    numReservedPages = maxHeapSize / systemInfo.dwPageSize;
    numCommittedPages = initHeapSize / systemInfo.dwPageSize;
    
    numAllocationsPerPage.resize( numReservedPages, 0 );
    isCommitted.resize( numReservedPages, false );
    collectCounter = 0;

    heapBegin = static_cast<BYTE*>( VirtualAlloc( nullptr, maxHeapSize, MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE ) );
    if( maxHeapSize != 0 && heapBegin == nullptr ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }

    if( VirtualAlloc( heapBegin, initHeapSize, MEM_COMMIT, PAGE_READWRITE ) != heapBegin ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }
    std::fill( std::begin( isCommitted ), std::begin( isCommitted ) + initHeapSize / systemInfo.dwPageSize, true );

    markMemoryFree( heapBegin, maxHeapSize );
}

void CHeapManager::Destroy()
{
    if( VirtualFree( heapBegin, 0, MEM_RELEASE ) == 0 ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }

    heapBegin = nullptr;
}

void* CHeapManager::Alloc( int size )
{
    int allocationSize = granularRound( minBlockSize, size + sizeof( int ) );

    BYTE* memory = reserveMemory( allocationSize );
    if( memory == nullptr ) {
        throw std::bad_alloc();
    }

    int firstPageContainsAlloc = pageIndex( memory );
    int lastPageContainsAlloc = pageIndex( memory + allocationSize ); 
    int i = firstPageContainsAlloc;
    do {
        if( !isCommitted[i] && i >= initHeapSize / systemInfo.dwPageSize ) 
        {
            VirtualAlloc( pageAddress( i ), systemInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE );
            ++numCommittedPages;
            isCommitted[i] = true;
        }
        ++numAllocationsPerPage[i];
        ++i;
    } while( i <= lastPageContainsAlloc );
    *reinterpret_cast<int*>( memory ) = allocationSize;

    return memory + sizeof( int );
}

void CHeapManager::Free( void* mem )
{
    BYTE* allocatedMemory = static_cast<BYTE*>( mem ) - sizeof( int );
    int allocatedSize = *reinterpret_cast<int*>( allocatedMemory );

    int firstPageContainsAlloc = pageIndex( allocatedMemory );
    int lastPageContainsAlloc = pageIndex( allocatedMemory + allocatedSize );
    int i = firstPageContainsAlloc;
    do {
        --numAllocationsPerPage[i];
        if( numAllocationsPerPage[i] < 0 ) {
            throw std::logic_error( "Too few allocs per page" );
        }
        ++i;
    } while( i <= lastPageContainsAlloc );

    releaseMemory( allocatedMemory, allocatedSize );

    ++collectCounter;
    if( collectCounter >= numDeallocationsBeforeCollect ) {
        decommitUnusedMemory();
        collectCounter = 0;
    }
}

int CHeapManager::Size( void* mem ) const
{
    return *(static_cast<int*>( mem ) - 1);
}

int CHeapManager::CommittedMemorySize() const noexcept
{
    return numCommittedPages * systemInfo.dwPageSize;
}

void CHeapManager::Optimize()
{
    decommitUnusedMemory();
}

int CHeapManager::granularRound( int granulaSize, int value ) noexcept
{
    int residue = value % granulaSize;
    return residue > 0 ? value - residue + granulaSize : value;
}

BYTE* CHeapManager::reserveMemory( int size )
{
    BYTE* memory = nullptr;

    if( size < smallBlocksSizeLimit ) {
        memory = reserveMemory( smallFreeBlocks, size );
    }
    if( memory == nullptr && size < mediumBlocksSizeLimit ) {
        memory = reserveMemory( mediumFreeBlocks, size );
    }
    if( memory == nullptr ) {
        memory = reserveMemory( bigFreeBlocks, size );
    }

    return memory;
}

BYTE* CHeapManager::reserveMemory( std::map<BYTE*, int>& memorySet, int size ) 
{
    BYTE* memory = nullptr;
    int memorySize = 0;
    
    for( auto it = std::cbegin( memorySet ); it != std::cend( memorySet ); ++it ) {
        if( it->second >= size ) {
            memory = it->first;
            memorySize = it->second;

            memorySet.erase( it );

            if( size != memorySize ) {
                markMemoryFree( memory + size + 1, memorySize - size );
            }
            break;
        }
    }

    return memory;
}

inline int CHeapManager::pageIndex( BYTE* address ) const noexcept
{
    return std::distance( heapBegin, address ) / systemInfo.dwPageSize; 
}

inline BYTE* CHeapManager::pageAddress( int pageIndex ) const noexcept
{
    return heapBegin + pageIndex * systemInfo.dwPageSize;
}

void CHeapManager::releaseMemory( BYTE* memory, int size )
{
    std::tie( memory, size ) = uniteMemory( smallFreeBlocks, memory, size );
    std::tie( memory, size ) = uniteMemory( mediumFreeBlocks, memory, size );
    std::tie( memory, size ) = uniteMemory( bigFreeBlocks, memory, size );
    
    markMemoryFree( memory, size );
}

std::pair<BYTE*, int> CHeapManager::uniteMemory( std::map<BYTE*, int>& memorySet, BYTE* memory, int size )
{
    BYTE* attachedMemory = nullptr;
    int attachedMemorySize = 0;

    auto it = memorySet.upper_bound( memory );
    if( it != std::end( memorySet ) ) {
        attachedMemory = it->first;
        attachedMemorySize = it->second;

        if( memory + size + 1 == attachedMemory ) {
            memorySet.erase( it );
            size += attachedMemorySize; 
        }
    }

    it = memorySet.lower_bound( memory );
    if( it != std::end( memorySet ) && memory != it->first ) {
        attachedMemory = it->first;
        attachedMemorySize = it->second;
       
        if( attachedMemory + attachedMemorySize + 1 == memory ) {
            memorySet.erase( it );
            memory = attachedMemory;
            size += attachedMemorySize;
        }
    }

    return { memory, size };
}

void CHeapManager::markMemoryFree( BYTE* memory, int size )
{
    if( size < smallBlocksSizeLimit ) {
        smallFreeBlocks.emplace( memory, size );
    } else if( size < mediumBlocksSizeLimit ) {
        mediumFreeBlocks.emplace( memory, size );
    } else {
        bigFreeBlocks.emplace( memory, size );
    }
}

void CHeapManager::decommitUnusedMemory()
{
    for ( int i = initHeapSize / systemInfo.dwPageSize; i < numReservedPages; ++i) {
        if( numAllocationsPerPage[i] == 0 && isCommitted[i] ) {
            VirtualFree( pageAddress( i ), systemInfo.dwPageSize, MEM_DECOMMIT );
            isCommitted[i] = false;
            --numCommittedPages;
        }
    }
}