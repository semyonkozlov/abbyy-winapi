#include <cctype>
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>

#include "TextFilter.h"

#define ADD_ID( str, id ) (std::string(str) + std::to_string( id )).c_str()

const std::string CTextFilter::workerExeFilename = "Worker.exe";

CTextFilter::CTextFilter( const std::string& targetWordsFilename, int numWorkers ) :
    numWorkers( numWorkers ),
    fileMaps( numWorkers ),
    fileViews( numWorkers ),
    processInfos( numWorkers ),
    newTaskEvents( numWorkers ),
    finishedTaskEvents( numWorkers )
{
    std::string workerCommandLine = workerExeFilename + ' ' + targetWordsFilename + ' ';

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TFTerminateEvent" );
    assert( terminateEvent != nullptr );

    for( int i = 0; i < numWorkers; ++i ) {
        newTaskEvents[i] = CreateEvent( nullptr,
            FALSE,
            FALSE,
            ADD_ID( "Global\\TFNewTaskEvent", i ) );
        assert( newTaskEvents[i] != nullptr );

        finishedTaskEvents[i] = CreateEvent( nullptr, 
            FALSE, 
            FALSE, 
            ADD_ID( "Global\\TFFinishedTaskEvent", i ) );
        assert( finishedTaskEvents[i] != nullptr );
        
        // creating file mappings 
        fileMaps[i] = CreateFileMapping( INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            fileMapSize,
            ADD_ID( "Global\\TFTempFileMapping", i ) );
        assert( fileMaps[i] != nullptr );

        fileViews[i] = static_cast<char*>( MapViewOfFile( fileMaps[i], FILE_MAP_WRITE, 0, 0, 0 ) );
        assert( fileViews[i] != nullptr );
        //CloseHandle( fileMappping ); // TODO mb do not close here?
           
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
        CloseHandle( fileMaps[i] );
    }

    SetEvent( terminateEvent );
    CloseHandle( terminateEvent );
}

void CTextFilter::Filter( const std::string& inputFilename, const std::string& outputFilename )
{
    HANDLE inputFile = CreateFile(
        inputFilename.c_str(),
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr );

    HANDLE outputFile = CreateFile(
        outputFilename.c_str(),
        FILE_APPEND_DATA,
        0,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr );

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

    CloseHandle( inputFile );
    CloseHandle( outputFile );
}