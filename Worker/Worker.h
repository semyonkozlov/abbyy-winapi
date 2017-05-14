#pragma once

#include <Windows.h>
#include <fstream>
#include <unordered_set>

class CWorker {
public:
    CWorker( const std::string& targetWordsFilename, int id );
    ~CWorker();

    void Work();

private:
    int id;
    std::unordered_set<std::string> targetWords;

    HANDLE newTaskEvent;
    HANDLE finishedTaskEvent;
    HANDLE terminateEvent;

    std::fstream tempFile;
};
