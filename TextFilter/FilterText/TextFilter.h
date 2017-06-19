#pragma once

#include <vector>

#include <Windows.h>

class CTextFilter {
public:
    explicit CTextFilter( const std::string& targetWordsFilename, int numWorkers = 4 );
    ~CTextFilter();

    void Filter( const std::string& inputFilename, const std::string& outputFilename );

private:
    int numWorkers;

    std::vector<HANDLE> fileMaps;
    std::vector<char*> fileViews;

    std::vector<PROCESS_INFORMATION> processInfos;

    std::vector<HANDLE> newTaskEvents;
    std::vector<HANDLE> finishedTaskEvents;

    HANDLE terminateEvent;

    static const std::string workerExeFilename;
    static const int fileMapSize = 100000;
};
