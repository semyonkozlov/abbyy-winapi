#pragma once

#include <Windows.h>
#include <fstream>
#include <vector>

class CTextFilter {
public:
    CTextFilter( const std::wstring& targetWordsFileName, 
        int numWorkers = 4, const std::wstring& workerExeFileName = L"Worker.exe" );
    ~CTextFilter();
    void Filter(const std::wstring& fileToFilter, const std::wstring& newFileName );

private:
    std::wstring targetWordsFileName;
    int numWorkers;
    std::wstring workerExeFileName;

    std::vector<PROCESS_INFORMATION> proñessInfos;
    std::vector<STARTUPINFOW> startupInfos;

    std::vector<HANDLE> newTaskEvents;
    std::vector<HANDLE> finishTaskEvents;
    HANDLE terminateWorkersEvent;

    std::vector<std::wofstream> tempFiles;
};
