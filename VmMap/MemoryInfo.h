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