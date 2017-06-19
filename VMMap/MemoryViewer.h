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

struct CRegionInfo {
    CRegionInfo();

    int RegionSize;
    DWORD RegionType;
    int NumBlocks;
    int NumGuardedBlocks;
    bool IsStack;
};

class CMemoryViewer {
public:
    explicit CMemoryViewer( HANDLE processHandle );

    CMemoryInfo GetMemoryInfo( const void* memory );
    CRegionInfo GetRegionInfo( const void* memory ) const;

private:

    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;

    HANDLE processHandle;
};