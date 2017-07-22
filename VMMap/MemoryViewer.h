#pragma once

#include <Windows.h>

struct CMemoryInfo {
    CMemoryInfo();

    void* RegionBaseAddress; 
    DWORD RegionProtection;
    int RegionSize;
    DWORD RegionType;
    int NumBlocks;
    int NumGuardedBlocks;
    bool IsStack;

    void* BlockBaseAddress;
    DWORD BlockProtection;
    int BlockSize;
    DWORD BlockType;
};

class CMemoryViewer {
public:
    CMemoryViewer() = default;
    explicit CMemoryViewer( HANDLE processHandle );

    CMemoryInfo GetMemoryInfo( const void* memory ) const;

private:
    void getRegionInfo( const void* regionBaseAddress, CMemoryInfo& memoryInfo ) const;

    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;

    HANDLE processHandle;
};
