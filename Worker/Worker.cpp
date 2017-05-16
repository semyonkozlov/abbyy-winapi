#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cassert>

#include "Worker.h"
#include "Utils.h"

CWorker::CWorker( const std::string& targetWordsFilename, int id ) : id( id )
{
    std::ifstream targetWordsFile( targetWordsFilename );
    targetWords = { std::istream_iterator<std::string>( targetWordsFile), {} };
    targetWordsFile.close();

    newTaskEvent = CreateEvent( nullptr, 
        FALSE,
        FALSE,
        AddId( "Global\\TFNewTaskEvent", id ).c_str() );
    assert( newTaskEvent != nullptr );
    
    finishedTaskEvent = CreateEvent( nullptr, 
        FALSE, 
        FALSE, 
        AddId( "Global\\TFFinishedTaskEvent", id ).c_str() );
    assert( finishedTaskEvent != nullptr );

    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TFTerminateEvent" );
    assert( terminateEvent != nullptr );

    fileMap = OpenFileMapping( FILE_MAP_ALL_ACCESS, 
        FALSE, 
        AddId( "Global\\TFTempFileMapping", id ).c_str() );
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
                // two scan lines
                char* currentWordPtr = fileView;
                int filteredViewShift = 0;

                while( *currentWordPtr != '\0' ) {
                    while( std::isspace( *currentWordPtr ) ) {
                        fileView[filteredViewShift++] = *currentWordPtr; // copy whitespaces
                        ++currentWordPtr;
                    }

                    int wordSize = 0;

                    // set word apart
                    while( !isspace( currentWordPtr[wordSize] ) && currentWordPtr[wordSize] != '\0' ) { 
                        ++wordSize;
                    }
                    std::string word( currentWordPtr, wordSize );
                    if( targetWords.find( word ) == targetWords.end() ) {
                        CopyMemory( fileView + filteredViewShift, word.c_str(), word.length() );
                        filteredViewShift += wordSize;
                    }

                    currentWordPtr += wordSize;
                }

                fileView[filteredViewShift + 1] = '\0';

                SetEvent( finishedTaskEvent );
            }
        }
    }
}
