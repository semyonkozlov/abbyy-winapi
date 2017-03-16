#include <Windows.h>
#include <Psapi.h>

#include <iostream>
#include <random>

#include "WinApiLimits.h"

void PrintError( DWORD messageId )
{
    wchar_t errorMessage[256];
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, messageId,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), errorMessage, 255, NULL );
    std::wcerr << "ERROR: " << errorMessage;
}

void FindAllocatedMemorySizeLimit()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo( &systemInfo );
    DWORD pageSize = systemInfo.dwPageSize;

    for( DWORD memorySize = pageSize; ; memorySize += pageSize ) {
        LPVOID memoryBuffer = VirtualAlloc( NULL, memorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
        if( memoryBuffer == NULL ) {
            PrintError( GetLastError() );
            std::cout << "Available memory size: " << memorySize - pageSize << std::endl;
            return;
        } else {
            VirtualFree( memoryBuffer, 0, MEM_RELEASE );
        }
    }
}

void FindNumKernelObjectsLimit()
{
    for( int i = 0; ; ++i ) {
        if( CreateEvent( NULL, TRUE, TRUE, NULL ) == NULL ) {
            PrintError( GetLastError() );
            std::cout << "Number of available kernel objects: " << i << std::endl;
            return;
        }
    }
}

void FindNumHandlesLimit()
{
    HANDLE handle = CreateMutex( NULL, FALSE, NULL );
    for( int i = 0; ; ++i ) {
        if( DuplicateHandle( GetCurrentProcess(), handle, GetCurrentProcess(),
            NULL, 0, FALSE, DUPLICATE_SAME_ACCESS ) == 0 ) {
            PrintError( GetLastError() );
            std::cout << "Number of available handles: " << i << std::endl;
            return;
        }
    }
}

void EmptyFunction()
{
    int stackLabel;
    std::cout << (int)&stackLabel << std::endl;
    EmptyFunction();
}

int FunctionWithParameters( int a, char b, long c )
{
    int stackLabel;
    std::cout << (int)&stackLabel << std::endl;
    return FunctionWithParameters( a, b, c );
}

void FindFunctionMemoryUsage()
{
    //PROCESS_MEMORY_COUNTERS memoryInfo;
    //GetProcessMemoryInfo( GetCurrentProcess(), &memoryInfo, sizeof( memoryInfo ) );
    EmptyFunction();
    //std::cout << memoryInfo.WorkingSetSize << std::endl;
}

void FindPathLimit()
{
    SetCurrentDirectory( L"C:/" );
    for( int i = 0; ; ++i ) {
        if( CreateDirectory( L"a", NULL ) == 0 ) {
            PrintError( GetLastError() );
            wchar_t currentPath[512];
            GetCurrentDirectory( 512, currentPath );
            std::cout << "Path limit: " << std::wstring( currentPath ).length() << std::endl;
            SHFILEOPSTRUCT fileOperation = { NULL, FO_DELETE, L"C:/a", NULL,
                FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION, FALSE, NULL, NULL };

            SHFileOperation( &fileOperation ); // removing temp folder
            return;
        };
        SetCurrentDirectory( L"a" );
    }
}

void FragmentMemory( int numAllocs )
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo( &systemInfo );
    auto pageSize = systemInfo.dwPageSize;

    for( int i = 0; i < numAllocs; ++i ) {
        SIZE_T sizeOfNewChunk = (std::rand() % 10 + 1) * pageSize;
        auto protect = std::rand() % 2 ? PAGE_GUARD | PAGE_NOACCESS : PAGE_READWRITE;
        VirtualAlloc( NULL, sizeOfNewChunk, MEM_COMMIT, protect );
    }

    while( true ) {}
}

void FindNumGdiObjectsLimit()
{
    for( int i = 0; ; ++i ) {
        if( CreatePen( PS_DOT, 1, RGB( 0, 255, 0 ) ) == NULL ) {
            PrintError( GetLastError() );
            std::cout << "Number of available GDI Objects: " << i << std::endl;
            return;
        }
    }
}