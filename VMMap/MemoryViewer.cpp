#include <cassert>
#include <stdexcept>

#include "MemoryViewer.h"

CMemoryInfo::CMemoryInfo()
{
    ZeroMemory( this, sizeof( CMemoryInfo ) );
}

CRegionInfo::CRegionInfo()
{
    ZeroMemory( this, sizeof( CRegionInfo ) );
}

CMemoryViewer::CMemoryViewer( HANDLE processHandle ) : processHandle( nullptr )
{
}

CMemoryInfo CMemoryViewer::GetMemoryInfo( const void* memory )
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

    CRegionInfo regionInfo{};
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
            // TODO
            regionInfo = GetRegionInfo( memory );

            memoryInfo.RegionBaseAddress = blockInfo.AllocationBase ;
            memoryInfo.RegionProtection = blockInfo.AllocationProtect;
            memoryInfo.RegionSize = regionInfo.RegionSize;
            memoryInfo.RegionType = regionInfo.RegionType;
            memoryInfo.NumBlocks = regionInfo.NumBlocks;
            memoryInfo.NumGuardedBlocks = regionInfo.NumGuardedBlocks;
            memoryInfo.IsStack = regionInfo.IsStack;

            memoryInfo.BlockBaseAddress = blockInfo.BaseAddress;
            memoryInfo.BlockSize = blockInfo.RegionSize;
            memoryInfo.BlockProtection = blockInfo.AllocationProtect;
            memoryInfo.BlockType = MEM_RESERVE;
            break;
        }
        case MEM_COMMIT:
        {
            regionInfo = GetRegionInfo( memory );

            memoryInfo.RegionBaseAddress = blockInfo.AllocationBase;
            memoryInfo.RegionProtection = blockInfo.AllocationProtect;
            memoryInfo.RegionSize = regionInfo.RegionSize;
            memoryInfo.RegionType = regionInfo.RegionType;
            memoryInfo.NumBlocks = regionInfo.NumBlocks;
            memoryInfo.NumGuardedBlocks = regionInfo.NumGuardedBlocks;
            memoryInfo.IsStack = regionInfo.IsStack;

            memoryInfo.BlockBaseAddress = blockInfo.BaseAddress;
            memoryInfo.BlockSize = blockInfo.RegionSize;
            memoryInfo.BlockProtection = blockInfo.Protect;
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

CRegionInfo CMemoryViewer::GetRegionInfo( const void* memory ) const
{
    CRegionInfo regionInfo{};

    MEMORY_BASIC_INFORMATION blockInfo;
    auto queryStatus = VirtualQueryEx( 
        processHandle,
        memory, 
        &blockInfo, 
        sizeof( blockInfo ) );

    if( queryStatus != sizeof( blockInfo ) ) {
        throw std::runtime_error( "Fail obtaining memory information" ); // TODO
    }

    BYTE* regionBaseAddress = static_cast<BYTE*>( blockInfo.AllocationBase );
    BYTE* currentBlockAddress = regionBaseAddress;

    // while in the same region
    while( blockInfo.AllocationBase == regionBaseAddress ) {
        queryStatus = VirtualQueryEx( processHandle, currentBlockAddress, &blockInfo, sizeof( blockInfo ) );
        if( queryStatus != sizeof( blockInfo) ) {
            break;
        }

        ++regionInfo.NumBlocks;
        regionInfo.RegionSize += blockInfo.RegionSize;

        if(( blockInfo.Protect & PAGE_GUARD) == PAGE_GUARD ) {
            ++regionInfo.NumGuardedBlocks;
        }

        if( regionInfo.RegionType == MEM_PRIVATE ) {
            regionInfo.RegionType = blockInfo.Type;
        }

        currentBlockAddress += blockInfo.RegionSize;
    }

    regionInfo.IsStack = regionInfo.NumGuardedBlocks > 0;

    return regionInfo;
}
