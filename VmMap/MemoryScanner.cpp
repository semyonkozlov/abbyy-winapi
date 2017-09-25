#include <cassert>

#include <Windows.h>
#include <Psapi.h>

#include "MemoryScanner.h"

const CMemoryScanner::CSystemInfo CMemoryScanner::systemInfo;

CMemoryScanner::CMemoryScanner() :
    process( nullptr ),
    toolhelp()
{
}

bool CMemoryScanner::AttachToProcess( int procId )
{
    process = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId );

    toolhelp.CreateSnapshot( procId );

    return process;
}

void CMemoryScanner::DetachFromProcess()
{
    toolhelp.DestroySnapshot();
    CloseHandle( process );
}

bool CMemoryScanner::GetRegionInfo( const void* memory, CRegionInfo* regionInfo ) const
{
    int queryStatus = VirtualQueryEx( process, memory, regionInfo, sizeof( CRegionInfo ) );
    if( queryStatus != sizeof( CRegionInfo ) ) {
        return false;
    }

    switch( regionInfo->State ) {
        case MEM_FREE:
            regionInfo->AllocationBase = const_cast<void*>( memory );
            regionInfo->BaseAddress = const_cast<void*>( memory );
            regionInfo->Protect = 0;
            regionInfo->Type = MEM_FREE;
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

int CMemoryScanner::GetMemoryMap( std::vector<CAllocationInfo>* memoryMap ) const
{
    memoryMap->clear();

    CRegionInfo regionInfo{};
    CAllocationInfo allocationInfo{};
    long long regionSize = 0;

    for( const BYTE* currentAddress = 0;
        GetRegionInfo( currentAddress, &regionInfo );
        currentAddress += regionSize ) 
    {
        if( allocationInfo.AllocationBaseAddress == regionInfo.AllocationBase ) {
            allocationInfo.AddRregion( regionInfo );
        } else {
            obtainAllocationDetails( &allocationInfo );
            memoryMap->push_back( allocationInfo );
            allocationInfo.Clear();

            allocationInfo.AllocationBaseAddress = regionInfo.AllocationBase;
            allocationInfo.AddRregion( regionInfo );
        }

        regionSize = regionInfo.RegionSize;
    }
    memoryMap->push_back( allocationInfo );

    return memoryMap->size();
}

bool CMemoryScanner::obtainAllocationDetails( CAllocationInfo* allocationInfo ) const
{
    CModuleInfo moduleInfo;
    TCHAR filename[MAX_PATH];

    if( allocationInfo->NumGuardedBlocks > 0 ) {
        allocationInfo->Details = TEXT( "Thread stack" );
    } else if( allocationInfo->AllocationType == MEM_IMAGE &&
        toolhelp.FindModule( allocationInfo->AllocationBaseAddress, &moduleInfo ) ) 
    {
        allocationInfo->Details = moduleInfo.szExePath;
    } else if( allocationInfo->AllocationType == MEM_MAPPED &&
        GetMappedFileName( process, const_cast<void*>(allocationInfo->AllocationBaseAddress), filename, MAX_PATH ) )
    {
        allocationInfo->Details = filename;
    } else {
        return false;
    }

     /*else if( toolhelp.IsHeap( allocationInfo.AllocationBaseAddress ) ) {
        details = TEXT( "Heap" );
        }*/
    
    return true;
}

CRegionInfo::CRegionInfo()
{
    ZeroMemory( this, sizeof( CRegionInfo ) );
}

void CAllocationInfo::AddRregion( const CRegionInfo& regionInfo )
{
    if( NumBlocks == 0 ) {
        AllocationType = regionInfo.Type;
        AllocationProtection = regionInfo.AllocationProtect;
    }
    if( AllocationType == MEM_PRIVATE ) {
        AllocationType = regionInfo.Type;
    }

    ++NumBlocks;
    AllocationSize += regionInfo.RegionSize;

    if( (regionInfo.Protect & PAGE_GUARD) == PAGE_GUARD ) {
        NumGuardedBlocks++;
    }

    RegionsInfo.push_back( regionInfo );
}

void CAllocationInfo::Clear()
{
    AllocationBaseAddress = 0;
    AllocationSize = 0;

    AllocationProtection = 0;
    AllocationType = 0;

    NumBlocks = 0;
    NumGuardedBlocks = 0;

    Details.clear();
    RegionsInfo.clear();
}
