#include <fstream>
#include <string>
#include <filesystem>
#include <iterator>

#include "TextFilter.h"

const std::string CTextFilter::workerExeFilename = "Worker.exe";

CTextFilter::CTextFilter( const std::string& targetWordsFilename, int numWorkers ) :
        numWorkers( numWorkers ), targetWordsFile( targetWordsFilename ),
        tempFiles( numWorkers ), startupInfos( numWorkers ), processInfos( numWorkers ), 
        newTaskEvents( numWorkers ), finishedTaskEvents( numWorkers )
{
    std::string workerCommandLine = workerExeFilename + ' ' + targetWordsFilename + ' ';
    for( int i = 0; i < numWorkers; ++i ) {
        ZeroMemory( &startupInfos[i], sizeof( startupInfos[i] ) );
        startupInfos[i].cb = sizeof( startupInfos[i] );
        ZeroMemory( &processInfos[i], sizeof( processInfos[i] ) );

        CreateProcess( workerExeFilename.c_str(),
                const_cast<LPSTR>( (workerCommandLine + ' ' + std::to_string( i )).c_str() ),
                nullptr,
                nullptr,
                FALSE,
                CREATE_DEFAULT_ERROR_MODE,
                nullptr,
                nullptr,
                &startupInfos[i],
                &processInfos[i] );

        newTaskEvents[i] = CreateEvent( nullptr, TRUE, FALSE,
                (std::string( "Global\\NewTaskEvent1337_" ) + std::to_string( i )).c_str() );

        finishedTaskEvents[i] = CreateEvent( nullptr, TRUE, FALSE,
                (std::string( "Global\\FinishTaskEvent1337_" ) + std::to_string( i )).c_str() );

        tempFiles[i] = { (std::string( "WorkerTempFile_" ) + std::to_string( i )).c_str() };
    }

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TerminateWorkersEvent1337" );
}

CTextFilter::~CTextFilter()
{
    for( int i = 0; i < numWorkers; ++i ) {
        CloseHandle( processInfos[i].hProcess );
        CloseHandle( processInfos[i].hThread );

        CloseHandle( newTaskEvents[i] );
        CloseHandle( finishedTaskEvents[i] );

        tempFiles[i].close();
        std::experimental::filesystem::remove( std::string( "WorkerTempFile_" ) + std::to_string( i ) );
    }

    SetEvent( terminateEvent );
    CloseHandle( terminateEvent );
}

void CTextFilter::Filter( std::ifstream& inputFile, std::ofstream& outputFile )
{
    std::vector<std::string> fileContent( std::istream_iterator<std::string>( inputFile ), {});
    
    int chunkSize = fileContent.size() / numWorkers;
    int remainderSize = fileContent.size() % numWorkers;

    int i = 0;
    for( auto leftIt = std::cbegin( fileContent ), rightIt = leftIt;
         leftIt < std::cend( fileContent );
         leftIt = rightIt, ++i ) 
    {
        rightIt = leftIt + chunkSize + (remainderSize > 0 ? 1 : 0);
        if ( remainderSize > 0 ) {
            --remainderSize;
        }

        std::copy( leftIt, rightIt, std::ostream_iterator<std::string>( tempFiles[i], " " ) );
        ResetEvent( finishedTaskEvents[i] );
        SetEvent( newTaskEvents[i] );
    }

    WaitForMultipleObjects( numWorkers, finishedTaskEvents.data(), TRUE, INFINITE );

    for( i = 0; i < numWorkers; ++i ) {
        outputFile << tempFiles[i].rdbuf();
    }
}
