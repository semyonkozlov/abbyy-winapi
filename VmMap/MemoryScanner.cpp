#include <cassert>

#include <Windows.h>

#include "MemoryScanner.h"

const CMemoryScanner::CSystemInfo CMemoryScanner::systemInfo;

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

bool CMemoryScanner::GetRegionInfo( const void* memory, CRegionInfo* regionInfo ) const
{
    int queryStatus = VirtualQueryEx( process, memory, regionInfo, sizeof( CRegionInfo ) );
    if( queryStatus != sizeof( CRegionInfo ) ) {
        return false;
    }

    switch( regionInfo->State ) {
        case MEM_FREE:
            regionInfo->AllocationBase = const_cast<PVOID>( memory );
            regionInfo->Protect = 0;
            break;
        case MEM_RESERVE:
            regionInfo->Protect = regionInfo->AllocationProtect;
            break;
        case MEM_COMMIT:
            break;
        default:
            return false;
    }

    return true;
}

std::vector<CRegionInfo> CMemoryScanner::GetMemoryMap() const
{
    std::vector<CRegionInfo> memoryMap;
    CRegionInfo regionInfo;
    long long regionSize = 0;
    int i = 0; // TODO

    for( BYTE* currentAddress = 0;
        GetRegionInfo( currentAddress, &regionInfo );
        currentAddress += regionSize ) 
    {
        memoryMap.push_back( regionInfo );
        regionSize = regionInfo.RegionSize;
        //ZeroMemory( &regionInfo, sizeof( CRegionInfo ) );
    }

    return memoryMap;
}

CRegionInfo::CRegionInfo()
{
    ZeroMemory( this, sizeof( CRegionInfo ) );
}
