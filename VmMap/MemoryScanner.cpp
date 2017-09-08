#include "MemoryScanner.h"

CMemoryScanner::CMemoryScanner() : process( nullptr )
{
}

void CMemoryScanner::AttachToProcess( HANDLE process )
{
}

void CMemoryScanner::GetMemoryInfo( const void* memory, CMemoryInfo* memoryInfo ) const
{

}

void CMemoryScanner::getRegionInfo( const void* regionBaseAddress, CMemoryInfo* memoryInfo ) const
{
    MEMORY_BASIC_INFORMATION blockInfo;
    auto currentBlockAddress = static_cast<const BYTE*>( regionBaseAddress );

    // while in the same region
    do {
        auto queryStatus = VirtualQueryEx( process, currentBlockAddress, &blockInfo, sizeof( blockInfo ) );
        if( queryStatus != sizeof( blockInfo ) ) {
            break;
        }

        ++memoryInfo->NumBlocks;
        memoryInfo->RegionSize += blockInfo.RegionSize;

        if( (blockInfo.Protect & PAGE_GUARD) == PAGE_GUARD ) {
            ++memoryInfo->NumGuardedBlocks;
        }

        // TODO
        if( memoryInfo->RegionType == MEM_PRIVATE ) {
            memoryInfo->RegionType = blockInfo.Type;
        }

        currentBlockAddress += blockInfo.RegionSize;
    } while( blockInfo.AllocationBase == regionBaseAddress );

    memoryInfo->IsStack = memoryInfo->NumGuardedBlocks > 0;
}
