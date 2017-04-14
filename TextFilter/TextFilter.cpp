#include <memory>
#include <fstream>
#include <string>

#include "TextFilter.h"
#include <filesystem>

CTextFilter::CTextFilter( const std::wstring& targetWordsFileName, 
    int numWorkers, const std::wstring& workerExeFileName ) :
        targetWordsFileName( targetWordsFileName ), numWorkers( numWorkers ), workerExeFileName( workerExeFileName ), 
        proñessInfos( numWorkers ), startupInfos( numWorkers ),
        newTaskEvents( numWorkers ), finishTaskEvents( numWorkers ), terminateWorkersEvent(),
        tempFiles( numWorkers )
{
    std::wstring workerCommandLine = workerExeFileName + L' ' + targetWordsFileName + L' ';

    for( int i = 0; i < numWorkers; ++i ) {
        if( CreateProcessW( workerExeFileName.c_str(),
            const_cast<LPWSTR>( (workerCommandLine + std::to_wstring( i )).c_str() ),
            nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
            &startupInfos[i], &proñessInfos[i] ) == 0 ) 
        {
            throw std::runtime_error( "Fail creating process." );
        }

        newTaskEvents[i] = CreateEventW( nullptr, TRUE, FALSE, 
            (std::wstring( L"Global\\NewTaskEvent1337_" ) + std::to_wstring( i )).c_str() );

        finishTaskEvents[i] = CreateEventW( nullptr, TRUE, FALSE, 
            (std::wstring( L"Global\\FinishTaskEvent1337_" ) + std::to_wstring( i )).c_str() );

        tempFiles[i] = { (std::wstring( L"TempFileWorker_" ) + std::to_wstring( i )).c_str() };
    }

    terminateWorkersEvent = CreateEventW( nullptr, TRUE, FALSE, L"Global\\TerminateWorkersEvent1337" );
}

CTextFilter::~CTextFilter()
{
    for( int i = 0; i < numWorkers; ++i ) {
        CloseHandle( proñessInfos[i].hProcess );
        CloseHandle( proñessInfos[i].hThread );

        CloseHandle( newTaskEvents[i] );
        CloseHandle( finishTaskEvents[i] );

        tempFiles[i].close();
        std::experimental::filesystem::remove( std::wstring( L"TempFileWorker_" ) + std::to_wstring( i ) );
    }

    CloseHandle( terminateWorkersEvent );
}

void CTextFilter::Filter( const std::wstring& fileToFilter, const std::wstring& newFileName )
{
}
