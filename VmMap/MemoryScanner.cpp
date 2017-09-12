#include <cassert>

#include <Windows.h>

#include "MemoryScanner.h"

CMemoryScanner::CMemoryScanner() :
    process( nullptr )
{
}

bool CMemoryScanner::AttachToProcess( int procId )
{
    process = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, procId );
    assert( process != nullptr ); // TODO

    return process;
}

void CMemoryScanner::DetachFromProcess()
{
    CloseHandle( process );
    process = nullptr;
}

bool CMemoryScanner::GetAllocationInfo( const void* memory, CAllocationInfo* regionInfo ) const
{
    CBlockInfo blockInfo;
    int queryStatus = VirtualQueryEx( process, memory, &blockInfo, sizeof( CBlockInfo ) );
    if( queryStatus != sizeof( CBlockInfo ) ) {
        return false;
    }

    regionInfo->AllocationBaseAddress = blockInfo.AllocationBase;
    auto currentAddress = static_cast<const BYTE*>( blockInfo.AllocationBase );

    regionInfo->AllocationType = blockInfo.Type;
    regionInfo->AllocationProtection = blockInfo.AllocationProtect;

    while( VirtualQueryEx( process, currentAddress, &blockInfo, sizeof( CBlockInfo ) == sizeof( CBlockInfo ) ) )
    {
        if( blockInfo.AllocationBase != regionInfo->AllocationBaseAddress ) {
            return true;
        }

        regionInfo->NumBlocks++;
        regionInfo->AllocationSize += blockInfo.RegionSize;

        if( (blockInfo.Protect & PAGE_GUARD) == PAGE_GUARD ) {
            regionInfo->NumGuardedBlocks++;
        }

        if( regionInfo->AllocationType == MEM_PRIVATE ) { 
            regionInfo->AllocationType = blockInfo.Type;
        }
        currentAddress += blockInfo.RegionSize;

        regionInfo->BlocksInfo.push_back( blockInfo );
    }
    regionInfo->IsStack = regionInfo->NumGuardedBlocks > 0;

    return false;
}

CAllocationInfo::CAllocationInfo() :
    AllocationBaseAddress( nullptr ),
    AllocationSize( 0 ),
    AllocationType( 0 ),
    AllocationProtection( 0 ),
    NumBlocks( 0 ),
    NumGuardedBlocks( 0 ),
    IsStack( false ),
    BlocksInfo()
{
}
