#include "Worker.h"
#include <filesystem>
#include <sstream>

CWorker::CWorker( const std::wstring& targetWordsFileName, int id ) :
    id( id ), tempFile( std::wstring( L"TempFileWorker_" ) + std::to_wstring( id ) )
{
    std::wifstream targetWordsFile( targetWordsFileName );
    std::wstring fileContent( std::istreambuf_iterator<wchar_t>( targetWordsFile ), {} );
    std::wstringstream stringStream( fileContent );

    std::wstring temp;
    while( std::getline( stringStream, temp, L' ' ) ) {
        targetWords.insert( temp );
    }

    newTaskEvent = CreateEventW( nullptr, TRUE, FALSE,
        (std::wstring( L"Global\\NewTaskEvent1337_" ) + std::to_wstring( id )).c_str() );
    finishTaskEvent = CreateEventW( nullptr, TRUE, FALSE,
        (std::wstring( L"Global\\FinishTaskEvent1337_" ) + std::to_wstring( id )).c_str() );
    terminateWorkersEvent = CreateEventW( nullptr, TRUE, FALSE, L"Global\\TerminateWorkersEvent1337" );

}

CWorker::~CWorker()
{
    CloseHandle( newTaskEvent );
    CloseHandle( finishTaskEvent );
    CloseHandle( terminateWorkersEvent );

    tempFile.close();
}

void CWorker::Work()
{
}
