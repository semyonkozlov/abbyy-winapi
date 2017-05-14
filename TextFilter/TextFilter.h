#pragma once

#include <Windows.h>
#include <fstream>
#include <vector>

class CTextFilter {
public:
    CTextFilter( const std::string& targetWordsFilename, int numWorkers = 4 );
    ~CTextFilter();

    void Filter( std::ifstream& inputFile, std::ofstream& outputFile );

private:
    int numWorkers;

    std::vector<std::ofstream> tempFiles;

    std::vector<STARTUPINFO> startupInfos;
    std::vector<PROCESS_INFORMATION> processInfos;

    std::vector<HANDLE> newTaskEvents;
    std::vector<HANDLE> finishedTaskEvents;

    HANDLE terminateEvent;

    static const std::string workerExeFilename;
};
