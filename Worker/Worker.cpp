#define _SCL_SECURE_NO_WARNINGS // enable unsafe iterators for regex

#include <iterator>
#include <fstream>
#include <sstream>
#include <cassert>

#include "Worker.h"

#define ADD_ID( str, id ) (std::string(str) + std::to_string( id )).c_str()

CWorker::CWorker( const std::string& targetWordsFilename, int id ) : id( id )
{
    std::ifstream targetWordsFile( targetWordsFilename );
    std::stringstream regexStream;
    // converting target words to regex
    std::copy( std::istream_iterator<std::string>( targetWordsFile ),
        {}, 
        std::ostream_iterator<std::string>( regexStream, "|" ) );
    targetWordsFile.close();

    targetWords = regexStream.str();
 
    newTaskEvent = CreateEvent( nullptr, 
        FALSE,
        FALSE,
        ADD_ID( "Global\\TFNewTaskEvent", id ) );
    assert( newTaskEvent != nullptr );
    
    finishedTaskEvent = CreateEvent( nullptr, 
        FALSE, 
        FALSE, 
        ADD_ID( "Global\\TFFinishedTaskEvent", id ) );
    assert( finishedTaskEvent != nullptr );

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TFTerminateEvent" );
    assert( terminateEvent != nullptr );

    fileMap = OpenFileMapping( FILE_MAP_ALL_ACCESS, 
        FALSE, 
        ADD_ID( "Global\\TFTempFileMapping", id ) );
    assert( fileMap != nullptr );

    fileView = static_cast<char*>( MapViewOfFile( fileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) ); // TODO last arg susp
    assert( fileView != nullptr );
}

CWorker::~CWorker()
{
    CloseHandle( newTaskEvent );
    CloseHandle( finishedTaskEvent );
    CloseHandle( terminateEvent );

    UnmapViewOfFile( fileView );
    CloseHandle( fileMap );
}

void CWorker::Work()
{
    while( true ) {
        std::vector<HANDLE> eventsToWait{ terminateEvent, newTaskEvent };
        auto waitStatus = WaitForMultipleObjects( eventsToWait.size(), eventsToWait.data(), FALSE, INFINITE );
        switch( waitStatus ) {
            case WAIT_OBJECT_0 + 0: // terminate event
                return;
            case WAIT_OBJECT_0 + 1: // new task event
            {
                // replacing target words with empty string
                char* filteredViewEnd = std::regex_replace( fileView, 
                    fileView, 
                    fileView + std::strlen( fileView ),
                    targetWords, 
                    "" ); 

                *filteredViewEnd = '\0';

                SetEvent( finishedTaskEvent );
                break;
            }
            default:
                assert( false );
        }
    }
}
