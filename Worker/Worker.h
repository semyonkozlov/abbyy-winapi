#pragma once

#include <Windows.h>
#include <fstream>
#include <unordered_set>

class CWorker {
public:
    CWorker( const std::wstring& targetWordsFile, int id );
    ~CWorker();

    void Work();

private:
    int id;
    std::unordered_set<std::wstring> targetWords;

    HANDLE newTaskEvent;
    HANDLE finishTaskEvent;
    HANDLE terminateWorkersEvent;

    std::wifstream tempFile;
};
