#pragma once

#include <memory>
#include <vector>

#include <Windows.h>

#include "Utils.h"

struct CRegionInfo : MEMORY_BASIC_INFORMATION
{
    CRegionInfo();
};

struct CAllocationInfo 
{
    CAllocationInfo();

    void* AllocationBaseAddress;
    long long AllocationSize;

    DWORD AllocationProtection;
    DWORD AllocationType;

    int NumBlocks;
    int NumGuardedBlocks;
};

class CMemoryScanner {
public:
    CMemoryScanner();

    bool AttachToProcess( int procId );
    void DetachFromProcess();

    bool GetRegionInfo( _In_ const void* memory, _Out_ CRegionInfo* regionInfo ) const;
    std::vector<CRegionInfo> GetMemoryMap() const;

private:
    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;

    HANDLE process;
};