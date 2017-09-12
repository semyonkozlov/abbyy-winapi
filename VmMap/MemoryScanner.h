#pragma once

#include <memory>
#include <vector>

#include <Windows.h>

#include "Utils.h"

using CBlockInfo = MEMORY_BASIC_INFORMATION;

struct CAllocationInfo {
    CAllocationInfo();

    void* AllocationBaseAddress;
    int AllocationSize;

    DWORD AllocationType;
    DWORD AllocationProtection;

    int NumBlocks;
    int NumGuardedBlocks;

    bool IsStack;

    std::vector<CBlockInfo> BlocksInfo;
};

class CMemoryScanner {
public:
    CMemoryScanner();

    bool AttachToProcess( int procId );
    void DetachFromProcess();

    bool GetAllocationInfo( _In_ const void* memory, _Out_ CAllocationInfo* allocationInfo ) const;

private:
    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;
    
    HANDLE process;
};