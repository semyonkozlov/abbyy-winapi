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

    process = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId );
    assert( process != nullptr );

    processId = procId;
}

void CToolhelp::DestroySnapshot()
{
    CloseHandle( snapshot );
    CloseHandle( process );
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
//
//bool CToolhelp::IsHeap( const void* address ) const
//{
//    HEAPLIST32 heapList;
//    MEMORY_BASIC_INFORMATION memoryInfo;
//    heapList.dwSize = sizeof( HEAPLIST32 );
//
//    if( Heap32ListFirst( snapshot, &heapList ) ) {
//        do {
//            HEAPENTRY32 heapEntry;
//            ZeroMemory( &heapEntry, sizeof( HEAPENTRY32 ) );
//            heapEntry.dwSize = sizeof( HEAPENTRY32 );
//
//            if( Heap32First( &heapEntry, processId, heapList.th32HeapID ) ) {
//                do {
//                    VirtualQueryEx( 
//                        process,
//                        reinterpret_cast<void*>( heapEntry.dwAddress ),
//                        &memoryInfo, 
//                        sizeof( MEMORY_BASIC_INFORMATION ) );
//
//                    if( memoryInfo.AllocationBase <= address &&
//                        address <= static_cast<BYTE*>( memoryInfo.AllocationBase ) + memoryInfo.RegionSize ) 
//                    {
//                        return true;
//                    }
//
//                    heapEntry.dwSize = sizeof( HEAPENTRY32 );
//                } while( Heap32Next( &heapEntry ) );
//            }
//            heapList.dwSize = sizeof( HEAPLIST32 );
//        } while( Heap32ListNext( snapshot, &heapList ) );
//    }
//
//    return false;
//}

CModuleInfo::CModuleInfo()
{
    ZeroMemory( this, sizeof( CModuleInfo ) );
    dwSize = sizeof( CModuleInfo );
}
