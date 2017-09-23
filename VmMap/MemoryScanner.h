#pragma once

#include <vector>

#include <Windows.h>

#include "Toolhelp.h"
#include "Utils.h"

struct CRegionInfo : MEMORY_BASIC_INFORMATION
{
    CRegionInfo();
};

struct CAllocationInfo 
{
    CAllocationInfo() = default;

    const void* AllocationBaseAddress;
    long long AllocationSize;

    DWORD AllocationProtection;
    DWORD AllocationType;

    int NumBlocks;
    int NumGuardedBlocks;

    CString Details;

    std::vector<CRegionInfo> RegionsInfo;

    void AddRregion( const CRegionInfo& regionInfo );
    void Clear();
};

class CMemoryScanner {
public:
    CMemoryScanner();

    bool AttachToProcess( int procId );
    void DetachFromProcess();

    bool GetRegionInfo( _In_ const void* memory, _Out_ CRegionInfo* regionInfo ) const;
    int GetMemoryMap( _Out_ std::vector<CAllocationInfo>* memoryMap ) const;

private:
    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;

    HANDLE process;

    CToolhelp toolhelp;

    bool obtainAllocationDetails( _Inout_ CAllocationInfo* allocationInfo ) const;
};