#pragma once

#include <vector>
#include <map>

#include <Windows.h>

class CHeapManager {
    using MemorySet = std::map<BYTE*, int>; // TODO
    using MemoryBlock = std::pair<BYTE*, int>;

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
    void Free( void* mem );

    int Size( void* mem ) const;
    int CommittedMemorySize() const noexcept;

private:
    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;

    static const int smallBlocksSizeLimit;
    static const int mediumBlocksSizeLimit;

    static const int minBlockSize = sizeof( int );

    // TODO actually const
    int maxHeapSize;
    int initHeapSize;

    BYTE* heapBegin;
    int numReservedPages;   
    int numCommittedPages;

    std::vector<int> numAllocationsPerPage; 

    std::map<BYTE*, int> smallFreeBlocks;
    std::map<BYTE*, int> mediumFreeBlocks;
    std::map<BYTE*, int> bigFreeBlocks;

    static int granularRound( int granulaSize, int value ) noexcept;

    int pageIndex( BYTE* address ) const noexcept;
    BYTE* pageAddress( int pageIndex ) const noexcept;

    BYTE* reserveMemory( int size );
    BYTE* reserveMemory( std::map<BYTE*, int>& memorySet, int size );
    
    void releaseMemory( BYTE* memory, int size );
    std::pair<BYTE*, int> uniteMemory( std::map<BYTE*, int>& memorySet, BYTE* memory, int size );
    void markMemoryFree( BYTE* memory, int size );
};