#include <cassert>

#include "Toolhelp.h"

CToolhelp::CToolhelp() :
    snapshot( nullptr ),
    process( nullptr ),
    processId( -1 )
{
}

void CToolhelp::CreateSnapshot( int procId )
{
    snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, procId );
    assert( snapshot != INVALID_HANDLE_VALUE );

    if( procId != 0 ) {
        process = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId );
        assert( process != nullptr );
    }
   
    processId = procId;
}

void CToolhelp::DestroySnapshot()
{
    CloseHandle( snapshot );
    if( processId != 0 ) {
        CloseHandle( process );
    }
}

bool CToolhelp::FindModule( const void* allocationAddress, CModuleInfo* moduleInfo ) const
{
    moduleInfo->dwSize = sizeof( CModuleInfo );

    if( Module32First( snapshot, moduleInfo ) ) {
        do {
            if( moduleInfo->modBaseAddr == allocationAddress ) {
                return true;
            }

            moduleInfo->dwSize = sizeof( CModuleInfo );
        } while( Module32Next( snapshot, moduleInfo ) );
    }

    return false;
}
bool CToolhelp::GetProcessList( std::vector<CProcessInfo>* processInfoList )
{
    HANDLE currentProc = nullptr;

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof( PROCESSENTRY32 );

    PROCESS_MEMORY_COUNTERS processMemoryCounters;
    BOOL is32bit;

    CProcessInfo processInfo;
    processInfoList->clear();

    if( Process32First( snapshot, &processEntry ) ) {
        do {
            currentProc = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, processEntry.th32ProcessID );
            GetProcessMemoryInfo( currentProc, &processMemoryCounters, sizeof( processMemoryCounters ) );
            IsWow64Process( currentProc, &is32bit );
           
            processInfo.ProcessName = processEntry.szExeFile;
            processInfo.Pid = GetProcessId( currentProc );
            processInfo.WorkingSetSize = processMemoryCounters.WorkingSetSize;
            processInfo.Is64bit = !is32bit;

            if( processInfo.Pid != 0 ) {
                processInfoList->push_back( processInfo );
            }

            CloseHandle( currentProc );

            processEntry.dwSize = sizeof( PROCESSENTRY32 );
        } while( Process32Next( snapshot, &processEntry ) );
    }

    return true;
}

CModuleInfo::CModuleInfo()
{
    ZeroMemory( this, sizeof( CModuleInfo ) );
    dwSize = sizeof( CModuleInfo );
}