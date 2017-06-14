#pragma once

#include <regex>

#include <Windows.h>

class CWorker {
public:
    CWorker( const std::string& targetWordsFilename, int id );
    ~CWorker();

    void Work();

private:
    int id;
    std::regex targetWords;

    HANDLE newTaskEvent;
    HANDLE finishedTaskEvent;
    HANDLE terminateEvent;

    HANDLE fileMap;
    char* fileView;
};
