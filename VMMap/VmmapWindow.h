#pragma once

#include <Windows.h>

#include "Utils.h"
#include "MemoryViewer.h"

class CVmmapWindow {
public:
    explicit CVmmapWindow();
    ~CVmmapWindow() = default;

    static bool RegisterClass();
    bool Create();
    void Show( int cmdShow ) const;

protected:
    void OnCreate();
    void OnDestroy();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    HWND mainWindow;
    HWND listBox;
    CString windowName;

    bool expandRegionsFlag;
    HANDLE observedProcess;

    CMemoryViewer memoryViewer;
};
