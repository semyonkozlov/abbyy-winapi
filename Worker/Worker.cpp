#include <filesystem>
#include <iterator>
#include <iostream>

#include "Worker.h"

CWorker::CWorker( const std::string& targetWordsFilename, int id ) :
        id( id ), 
        tempFile( std::string( "WorkerTempFile_" ) + std::to_string( id ) ) 
        
{
    std::ifstream targetWordsFile( targetWordsFilename );
    targetWords = { std::istream_iterator<std::string>( targetWordsFile), {} };
    targetWordsFile.close();
    
    newTaskEvent = CreateEvent( nullptr, TRUE, FALSE,
            (std::string( "Global\\NewTaskEvent1337_" ) + std::to_string( id )).c_str() );
    finishedTaskEvent = CreateEvent( nullptr, TRUE, FALSE,
            (std::string( "Global\\FinishedTaskEvent1337_" ) + std::to_string( id )).c_str() );
    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TerminateEvent1337" );
}

CWorker::~CWorker()
{
    CloseHandle( newTaskEvent );
    CloseHandle( finishedTaskEvent );
    CloseHandle( terminateEvent );

    tempFile.close();
}

void CWorker::Work()
{
    while( true ) {
        ResetEvent( finishedTaskEvent );

        std::vector<HANDLE> eventsToWait{ terminateEvent, newTaskEvent };
        DWORD waitStatus = WaitForMultipleObjects( eventsToWait.size(), eventsToWait.data(), FALSE, INFINITE );
        switch( waitStatus ) {
            case WAIT_OBJECT_0 + 0: // terminate event
                return;
            case WAIT_OBJECT_0 + 1: // new task event
            {
                std::vector<std::string> fileContent( std::istream_iterator<std::string>( tempFile ), {} );
                std::vector<std::string> filteredContent;
                for( auto&& str : fileContent ) {
                    if( targetWords.find( str ) == targetWords.end() ) {
                        filteredContent.push_back( str );
                    }
                }

                std::copy( std::cbegin( filteredContent ), std::cend( filteredContent ),
                           std::ostream_iterator<std::string>( tempFile, " " ) );

                SetEvent( finishedTaskEvent );
            }
            default:
                std::cerr << "Error waiting object." << std::endl;
        }
    }
}
