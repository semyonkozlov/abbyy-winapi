#pragma once

#include <vector>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "Utils.h"

struct CModuleInfo : MODULEENTRY32 {
    CModuleInfo();
};

struct CProcessInfo {
    CProcessInfo() = default;

    CString ProcessName;
    int Pid;
    long long WorkingSetSize;

    bool Is64bit;
};

class CToolhelp {
public:
    CToolhelp();

    void CreateSnapshot( int procId );
    void DestroySnapshot();

    bool FindModule( _In_ const void* allocationAddress, _Out_ CModuleInfo* moduleInfo ) const;

    bool GetProcessList( _Out_ std::vector<CProcessInfo>* processInfoList );
    
    //bool IsHeap( const void* address ) const;

private:
    HANDLE snapshot;

    HANDLE process;
    int processId;
};