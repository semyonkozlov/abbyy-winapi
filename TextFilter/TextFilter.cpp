#include <cctype>
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>
#include <locale>

#include "TextFilter.h"

#define IDENT( str, id ) str#id

const std::string CTextFilter::workerExeFilename = "Worker.exe";

CTextFilter::CTextFilter( const std::string& targetWordsFilename, int numWorkers ) :
    numWorkers( numWorkers ),
    fileViews( numWorkers ),
    processInfos( numWorkers ),
    newTaskEvents( numWorkers ),
    finishedTaskEvents( numWorkers )
{
    std::string workerCommandLine = workerExeFilename + ' ' + targetWordsFilename + ' ';

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "TFTerminateEvent" );
    assert( terminateEvent != nullptr );

    for( int i = 0; i < numWorkers; ++i ) {
        newTaskEvents[i] = CreateEvent( nullptr, FALSE, FALSE, IDENT( "TFNewTaskEvent", i ));
        assert( newTaskEvents[i] != nullptr );

        finishedTaskEvents[i] = CreateEvent( nullptr, FALSE, FALSE, IDENT( "TFFinishedTaskEvent", i ));
        assert( finishedTaskEvents[i] != nullptr );
        
        // creating file mappings 
        auto fileMappping = CreateFileMapping( INVALID_HANDLE_VALUE, 
            nullptr, 
            PAGE_READWRITE, 
            0, 
            fileMapSize, 
            IDENT( "TFTempFileMapping",  i ));
        assert( fileMappping != nullptr );

        fileViews[i] = static_cast<char*>( MapViewOfFile( fileMappping, FILE_MAP_WRITE, 0, 0, 0 ) );
        assert( fileViews[i] != nullptr );
        CloseHandle( fileMappping ); // TODO mb do not close here?
           
        STARTUPINFO startupInfo;
        ZeroMemory( &startupInfo, sizeof( startupInfo ) );
        startupInfo.cb = sizeof( startupInfo );
        ZeroMemory( &processInfos[i], sizeof( processInfos[i] ) );

        int createProcStatus = CreateProcess( workerExeFilename.c_str(),
            const_cast<LPSTR>( (workerCommandLine + std::to_string( i )).c_str() ),
            nullptr,
            nullptr,
            FALSE,
            CREATE_DEFAULT_ERROR_MODE,
            nullptr,
            nullptr,
            &startupInfo,
            &processInfos[i] );
        assert( createProcStatus != 0 );
    }
}

CTextFilter::~CTextFilter()
{
    for( int i = 0; i < numWorkers; ++i ) {
        CloseHandle( processInfos[i].hProcess );
        CloseHandle( processInfos[i].hThread );

        CloseHandle( newTaskEvents[i] );
        CloseHandle( finishedTaskEvents[i] );

        UnmapViewOfFile( fileViews[i] );
    }

    SetEvent( terminateEvent );
    CloseHandle( terminateEvent );
}

void CTextFilter::Filter( HANDLE inputFile, HANDLE outputFile )
{
    long long inputFileSize = GetFileSize( inputFile, nullptr );
   
    auto fileContent = std::make_unique<char[]>( inputFileSize + 1 );
    ReadFile( inputFile, fileContent.get(), inputFileSize, nullptr, nullptr );
    
    char* chunk = fileContent.get();

    for( int i = 0; i < numWorkers; ++i ) {
        long long chunkSize = std::min( inputFileSize / numWorkers, 
            (fileContent.get() + inputFileSize) - chunk); 

        // looking for end of current word
        while( !std::isspace( chunk[chunkSize] ) && chunk[chunkSize] != '\0' ) { 
            ++chunkSize;
        }

        CopyMemory( fileViews[i], chunk, chunkSize );
        fileViews[i][chunkSize + 1] = '\0';
       
        SetEvent( newTaskEvents[i] );

        chunk += chunkSize;
    }

    WaitForMultipleObjects( numWorkers, finishedTaskEvents.data(), TRUE, INFINITE );

    for( int i = 0; i < numWorkers; ++i ) {
        WriteFile( outputFile, fileViews[i], std::strlen( fileViews[i] ), nullptr, nullptr );
    }
}
