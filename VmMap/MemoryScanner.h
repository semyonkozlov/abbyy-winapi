#pragma once

#include <memory>

#include <Windows.h>

#include "MemoryInfo.h"

class CMemoryScanner {
public:
    CMemoryScanner();

    void AttachToProcess( HANDLE process );
    void GetMemoryInfo( _In_ const void* memory, _Out_ CMemoryInfo* memoryInfo ) const;

private:
    void getRegionInfo( _In_ const void* regionBaseAdress, _Out_ CMemoryInfo* memoryInfo ) const;

    static const struct CSystemInfo : SYSTEM_INFO {
        CSystemInfo()
        {
            GetSystemInfo( this );
        }
    } systemInfo;
    
    HANDLE process;
};