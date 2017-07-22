#include <cassert>
#include <stdexcept>

#include "MemoryViewer.h"

CMemoryInfo::CMemoryInfo()
{
    ZeroMemory( this, sizeof( CMemoryInfo ) );
}

CMemoryViewer::CMemoryViewer( HANDLE processHandle ) : processHandle( nullptr )
{
}

CMemoryInfo CMemoryViewer::GetMemoryInfo( const void* memory ) const
{
    CMemoryInfo memoryInfo{};

    MEMORY_BASIC_INFORMATION blockInfo;
    auto queryStatus = VirtualQueryEx(
        processHandle,
        memory,
        &blockInfo,
        sizeof( blockInfo ) );

    if( queryStatus != sizeof( blockInfo ) ) {
        throw std::runtime_error( "Fail obtaining memory information" ); // TODO
    }

    // TODO
    switch( blockInfo.State ) {
        case MEM_FREE:
        {
            memoryInfo.RegionBaseAddress = blockInfo.BaseAddress;
            memoryInfo.RegionProtection = blockInfo.AllocationProtect;
            memoryInfo.RegionSize = blockInfo.RegionSize;
            memoryInfo.RegionType = MEM_FREE;
            memoryInfo.NumBlocks = 0;
            memoryInfo.NumGuardedBlocks = 0;
            memoryInfo.IsStack = false;

            memoryInfo.BlockBaseAddress = nullptr;
            memoryInfo.BlockSize = 0;
            memoryInfo.BlockProtection = 0;
            memoryInfo.BlockType = MEM_FREE;
            break;
        }
        case MEM_RESERVE:
        {
            getRegionInfo( blockInfo.AllocationBase, memoryInfo );

            memoryInfo.BlockBaseAddress = blockInfo.BaseAddress;
            memoryInfo.BlockProtection = blockInfo.AllocationProtect;
            memoryInfo.BlockSize = blockInfo.RegionSize;
            memoryInfo.BlockType = MEM_RESERVE;
            break;
        }
        case MEM_COMMIT:
        {
            getRegionInfo( blockInfo.AllocationBase, memoryInfo );

            memoryInfo.BlockBaseAddress = blockInfo.BaseAddress;
            memoryInfo.BlockProtection = blockInfo.Protect;
            memoryInfo.BlockSize = blockInfo.RegionSize;
            memoryInfo.BlockType = blockInfo.Type;
            break;
        }
        default:
        {
            DebugBreak();
            break;
        }
    }

    return memoryInfo;
}

void CMemoryViewer::getRegionInfo( const void* regionBaseAddress, CMemoryInfo& memoryInfo ) const
{
    MEMORY_BASIC_INFORMATION blockInfo;
    const BYTE* currentBlockAddress = static_cast<const BYTE*>( regionBaseAddress );

    // while in the same region
    do {
        auto queryStatus = VirtualQueryEx( processHandle, currentBlockAddress, &blockInfo, sizeof( blockInfo ) );
        if( queryStatus != sizeof( blockInfo ) ) {
            break;
        }

        ++memoryInfo.NumBlocks;
        memoryInfo.RegionSize += blockInfo.RegionSize;

        if( (blockInfo.Protect & PAGE_GUARD) == PAGE_GUARD ) {
            ++memoryInfo.NumGuardedBlocks;
        }
        
        // TODO
        if( memoryInfo.RegionType == MEM_PRIVATE ) {
            memoryInfo.RegionType = blockInfo.Type;
        }

        currentBlockAddress += blockInfo.RegionSize;
    } while( blockInfo.AllocationBase == regionBaseAddress );

    memoryInfo.IsStack = memoryInfo.NumGuardedBlocks > 0;
}