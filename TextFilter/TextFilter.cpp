#include <cctype>
#include <memory>
#include <string>
#include <algorithm>

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
    std::string workerCommandLine = workerExeFilename + ' ' + targetWordsFilename;

    for( int i = 0; i < numWorkers; ++i ) {
        newTaskEvents[i] = CreateEvent( nullptr, FALSE, FALSE, IDENT( "Global\\TFNewTaskEvent", i ));
        finishedTaskEvents[i] = CreateEvent( nullptr, FALSE, FALSE, IDENT( "Global\\TFFinishedTaskEvent", i ));
        
        // creating file mappings 
        auto fileMappping = CreateFileMapping( INVALID_HANDLE_VALUE, 
            nullptr, 
            PAGE_READWRITE, 
            0, 
            fileMapSize, 
            IDENT( "Global\\TFTempFileMapping",  i ));

        fileViews[i] = static_cast<char*>( MapViewOfFile( fileMappping, FILE_MAP_WRITE, 0, 0, 0 ) );
        CloseHandle( fileMappping ); // TODO mb do not close here?
           
        STARTUPINFO startupInfo;
        ZeroMemory( &startupInfo, sizeof( startupInfo ) );
        startupInfo.cb = sizeof( startupInfo );
        ZeroMemory( &processInfos[i], sizeof( processInfos[i] ) );

        CreateProcess( workerExeFilename.c_str(),
            const_cast<LPSTR>( workerCommandLine.c_str() ),
            nullptr,
            nullptr,
            FALSE,
            CREATE_DEFAULT_ERROR_MODE,
            nullptr,
            nullptr,
            &startupInfo,
            &processInfos[i] );
    }

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "TFTerminateEvent" );
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

        chunk += chunkSize + 1;
    }

    WaitForMultipleObjects( numWorkers, finishedTaskEvents.data(), TRUE, INFINITE );
    
    // TODO write to outputFile
}
