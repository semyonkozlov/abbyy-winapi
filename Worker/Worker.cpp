#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "Worker.h"

#define IDENT( str, id ) str#id

int CWorker::workersCounter = 0;

CWorker::CWorker( const std::string& targetWordsFilename ) : id( workersCounter++ )
{
    std::ifstream targetWordsFile( targetWordsFilename );
    targetWords = { std::istream_iterator<std::string>( targetWordsFile), {} };
    targetWordsFile.close();
    
    newTaskEvent = CreateEvent( nullptr, FALSE, FALSE, IDENT( "Global\\TFNewTaskEvent", id ) );
    finishedTaskEvent = CreateEvent( nullptr, FALSE, FALSE, IDENT( "Global\\TFFinishedTaskEvent", id ) );
    terminateEvent = CreateEvent( nullptr, TRUE, FALSE, "Global\\TFTerminateEvent" );

    auto fileMapping = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, IDENT( "Global\\TFTempFileMapping", id ) );

    fileView = static_cast<char*>( MapViewOfFile( fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) ); // TODO last arg susp
    CloseHandle( fileMapping ); // TODO mb don't close
}

CWorker::~CWorker()
{
    CloseHandle( newTaskEvent );
    CloseHandle( finishedTaskEvent );
    CloseHandle( terminateEvent );

    UnmapViewOfFile( fileView );
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
            default:
                std::cerr << "Error waiting object." << std::endl;
        }
    }
}
