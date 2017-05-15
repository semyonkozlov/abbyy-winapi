#pragma once

#define NOMINMAX // to enable std::min

#include <Windows.h>
#include <vector>

class CTextFilter {
public:
    CTextFilter( const std::string& targetWordsFilename, int numWorkers = 4 );
    ~CTextFilter();

    void Filter( HANDLE inputFile, HANDLE outputFile );

private:
    int numWorkers;

    std::vector<char*> fileViews;

    std::vector<STARTUPINFO> startupInfos;
    std::vector<PROCESS_INFORMATION> processInfos;

    std::vector<HANDLE> newTaskEvents;
    std::vector<HANDLE> finishedTaskEvents;

    HANDLE terminateEvent;

    static const std::string workerExeFilename;
};
