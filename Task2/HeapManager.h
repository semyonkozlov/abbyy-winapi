#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include <Windows.h>

std::string GetErrorMessage( DWORD errorCode );

class CHeapManager {
public:
    CHeapManager() = default;
    CHeapManager( int initSize, int maxSize );
    ~CHeapManager();

    CHeapManager( const CHeapManager& ) = delete;
    CHeapManager& operator=( const CHeapManager& ) = delete;

    CHeapManager( CHeapManager&& ) = default;
    CHeapManager& operator=( CHeapManager&& ) = default;

    void Create( int initSize, int maxSize );
    void Destroy();

    void* Alloc( int size );
    void Free( void* mem);

private:
    int maxHeapSize;
    int initHeapSize;

    BYTE* heapBegin;
    int numReservedPages;

    std::vector<int> numBlocksPerPage; 

    std::set<BYTE*> smallFreeBlocks;
    std::map<BYTE*, int> mediumFreeBlocks;
    std::map<BYTE*, int> bigFreeBlocks;

    const int minBlockSize = sizeof( int );
    int smallBlocksSizeLimit;
    int mediumBlocksSizeLimit;

    SYSTEM_INFO systemInfo;

    int granularRound( int granulaSize, int value );

    int getPageIndex( BYTE* address );
    BYTE* getPageAddress( int pageIndex );

    BYTE* searchFreeBlock( int size );
    void logFreeBlock( BYTE* address, int size );
    void retrieveFreeBlock( BYTE* address, int size );
};