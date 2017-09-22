#pragma once

#include <vector>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "Utils.h"

struct CModuleInfo : MODULEENTRY32 {
    CModuleInfo();
};

class CToolhelp {
public:
    CToolhelp();

    void CreateSnapshot( int procId );
    void DestroySnapshot();

    bool FindModule( _In_ const void* allocationAddress, _Out_ CModuleInfo* moduleInfo ) const;
    bool IsHeap( const void* address ) const;
    CString GetMappedFileName( const void* allocationAddress ) const;

private:
    HANDLE snapshot;

    HANDLE process;
    int processId;
};