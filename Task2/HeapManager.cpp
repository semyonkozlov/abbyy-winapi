#pragma once

#include <exception>
#include <locale>
#include <codecvt>

#include "HeapManager.h"

std::string GetErrorMessage( DWORD errorCode )
{
    wchar_t errorMessage[256];
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), errorMessage, 255, NULL );

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes( errorMessage );
}

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

    GetSystemInfo( &systemInfo );

    smallBlocksSizeLimit = systemInfo.dwPageSize;
    mediumBlocksSizeLimit =  32 * systemInfo.dwPageSize;
    
    maxHeapSize = granularRound( systemInfo.dwAllocationGranularity, maxSize );
    numReservedPages = maxHeapSize / systemInfo.dwPageSize;
    numBlocksPerPage.resize( numReservedPages, 0 );

    heapBegin = (BYTE*)VirtualAlloc( NULL, maxHeapSize, MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE ); 
    if( maxHeapSize != 0 && heapBegin == NULL ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }

    initHeapSize = granularRound( systemInfo.dwPageSize, initSize );
    if( VirtualAlloc( heapBegin, initHeapSize, MEM_COMMIT, PAGE_READWRITE ) != heapBegin ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }

    logFreeBlock( heapBegin, maxHeapSize );

}

void CHeapManager::Destroy()
{
#ifdef DEBUG
    // TODO print out allocated areas
#endif
    
    if( VirtualFree( heapBegin, 0, MEM_RELEASE ) == 0 ) {
        throw std::runtime_error( GetErrorMessage( GetLastError() ) );
    }

    heapBegin = nullptr;
}

void* CHeapManager::Alloc( int size )
{
    int blockSize = granularRound( minBlockSize, size + sizeof( int ) );
    BYTE* freeMemory = searchFreeBlock( blockSize );
    if( freeMemory == NULL ) {
        throw std::bad_alloc(); // TODO redo mb
    }

    int firstPageContainsBlock = getPageIndex( freeMemory );
    int lastPageContainsBlock = getPageIndex( freeMemory + blockSize ); 
    int pageIndex = firstPageContainsBlock;
    do {
        if( numBlocksPerPage[pageIndex] == 0 && pageIndex > initHeapSize / systemInfo.dwPageSize ) {
            VirtualAlloc( getPageAddress(pageIndex), systemInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE );
        }
        ++numBlocksPerPage[pageIndex];
        ++pageIndex;
    } while( pageIndex <= lastPageContainsBlock );
    *(int*)freeMemory = size;

    return freeMemory + sizeof( int );
}

void CHeapManager::Free( void* mem )
{
    BYTE* allocatedMemory = (BYTE*)mem - sizeof( int );
    int blockSize = *(int*)allocatedMemory;

    int firstPageContainsAlloc = getPageIndex( allocatedMemory );
    int lastPageContainsAlloc = getPageIndex( allocatedMemory + blockSize );
    int pageIndex = firstPageContainsAlloc;
    do {
        --numBlocksPerPage[pageIndex];
        if( numBlocksPerPage[pageIndex] < 0 ) {
            throw std::logic_error( "Too few allocs per page" );
        }
        if( numBlocksPerPage[pageIndex] == 0 && pageIndex > initHeapSize / systemInfo.dwPageSize ) {
            VirtualFree( (void*)getPageAddress( pageIndex ), systemInfo.dwPageSize, MEM_DECOMMIT );
        }
        ++pageIndex;
    } while( pageIndex <= lastPageContainsAlloc );

    retrieveFreeBlock( allocatedMemory, blockSize );
}

inline int CHeapManager::granularRound( int granulaSize, int value )
{
    int residue = value % granulaSize;
    return residue > 0 ? value - residue + granulaSize : value;
}

BYTE* CHeapManager::searchFreeBlock( int size ) // TODO check
{
    BYTE* blockAddress = NULL;
    int blockSize;
    if( size < smallBlocksSizeLimit ) {
        for( auto address : smallFreeBlocks ) {
            blockSize = *(int*)(address);
            if( blockSize >= size ) {
                blockAddress = address; 
                smallFreeBlocks.erase( address );
                break;
            }
        }
    } 

    if( size < mediumBlocksSizeLimit && blockAddress == NULL ) {
        for( const auto& addressSizePair : mediumFreeBlocks ) {
            blockSize = addressSizePair.second; // TODO optimize mb
            if( blockSize >= size ) {
                blockAddress = addressSizePair.first;
                mediumFreeBlocks.erase( blockAddress );
                break;
            }
        }
    } 

    if( blockAddress == NULL ) {
        for( const auto& addressSizePair : bigFreeBlocks ) {
            blockSize = addressSizePair.second;
            if( blockSize >= size ) {
                blockAddress = addressSizePair.first;
                bigFreeBlocks.erase( blockAddress );
                break;
            }
        }
    }

    if( blockAddress != NULL && blockSize > size) {
        logFreeBlock( blockAddress + size + 1, blockSize - size );
    }

    return blockAddress;

}

inline int CHeapManager::getPageIndex( BYTE* address )
{
    return std::distance( heapBegin, address ) / systemInfo.dwPageSize; 
}

inline BYTE* CHeapManager::getPageAddress( int pageIndex )
{
    return heapBegin + pageIndex * systemInfo.dwPageSize;
}

void CHeapManager::logFreeBlock( BYTE* address, int size )
{
    if( size < smallBlocksSizeLimit ) {
        smallFreeBlocks.emplace( address );
        *(int*)address = size;
    } else if( size < mediumBlocksSizeLimit ) {
        mediumFreeBlocks.emplace( address, size );
    } else {
        bigFreeBlocks.emplace( address, size );
    }
}

void CHeapManager::retrieveFreeBlock( BYTE* address, int size ) // TODO refactor, check
{
    BYTE* attachedBlockAddress;
    int attachedBlockSize;

    // check out small blocks
    auto smallBlocksIt = smallFreeBlocks.upper_bound( address );
    if( smallBlocksIt != std::end( smallFreeBlocks ) ) {
        attachedBlockAddress = *smallBlocksIt;
        attachedBlockSize = *(int*)attachedBlockAddress;
        if( address + size + 1 == attachedBlockAddress ) {
            smallFreeBlocks.erase( smallBlocksIt );
            smallFreeBlocks.erase( address );
            logFreeBlock( address, size + attachedBlockSize );
        }
    }

    smallBlocksIt = smallFreeBlocks.lower_bound( address );
    if( smallBlocksIt != std::end( smallFreeBlocks ) ) {
        attachedBlockAddress = *smallBlocksIt;
        attachedBlockSize = *(int*)attachedBlockAddress;
        if( attachedBlockAddress + attachedBlockSize + 1 == address ) {
            smallFreeBlocks.erase( smallBlocksIt );
            smallFreeBlocks.erase( address );
            logFreeBlock( attachedBlockAddress, size + attachedBlockSize );
        }
    }

    // check out medium blocks
    auto mediumBlocksIt = mediumFreeBlocks.upper_bound( address );
    if( mediumBlocksIt != std::end( mediumFreeBlocks ) ) {
        attachedBlockAddress = mediumBlocksIt->first;
        attachedBlockSize = mediumBlocksIt->second;
        if( address + size + 1 == attachedBlockAddress ) {
            mediumFreeBlocks.erase( mediumBlocksIt );
            mediumFreeBlocks.erase( address );
            logFreeBlock( address, size + attachedBlockSize );
        }
    }

    mediumBlocksIt = mediumFreeBlocks.lower_bound( address );
    if( mediumBlocksIt != std::end( mediumFreeBlocks ) ) {
        attachedBlockAddress = mediumBlocksIt->first;
        attachedBlockSize = mediumBlocksIt->second;
        if( attachedBlockAddress + attachedBlockSize + 1 == address ) {
            mediumFreeBlocks.erase( mediumBlocksIt );
            mediumFreeBlocks.erase( address );
            logFreeBlock( attachedBlockAddress, size + attachedBlockSize );
        }
    }

    // check out big blocks
    auto bigBlocksIt = bigFreeBlocks.upper_bound( address );
    if( bigBlocksIt != std::end( bigFreeBlocks ) ) {
        attachedBlockAddress = bigBlocksIt->first;
        attachedBlockSize = bigBlocksIt->second;
        if( address + size + 1 == attachedBlockAddress ) {
            bigFreeBlocks.erase( bigBlocksIt );
            bigFreeBlocks.erase( address );
            logFreeBlock( address, size + attachedBlockSize );
        }
    }

    bigBlocksIt = bigFreeBlocks.lower_bound( address );
    if( bigBlocksIt != std::end( bigFreeBlocks ) ) {
        attachedBlockAddress = bigBlocksIt->first;
        attachedBlockSize = bigBlocksIt->second;
        if( attachedBlockAddress + attachedBlockSize + 1 == address ) {
            bigFreeBlocks.erase( bigBlocksIt );
            bigFreeBlocks.erase( address );
            logFreeBlock( attachedBlockAddress, size + attachedBlockSize );
        }
    }
}

