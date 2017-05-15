#pragma once

#include <Windows.h>
#include <unordered_set>

class CWorker {
public:
    CWorker( const std::string& targetWordsFilename );
    ~CWorker();

    void Work();

private:
    int id;
    std::unordered_set<std::string> targetWords;

    HANDLE newTaskEvent;
    HANDLE finishedTaskEvent;
    HANDLE terminateEvent;

    char* fileView;

    static int workersCounter;
};
