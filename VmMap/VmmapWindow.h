#pragma once

#include <Windows.h>

#include "SelectionDialog.h"
#include "ListView.h"
#include "MemoryScanner.h"
#include "Utils.h"

class CVmMapWindow {
public:
    CVmMapWindow();

    static void RegisterClass();

    HWND Create();
    void Show( int cmdShow ) const;

protected:
    void OnCreate();
    void OnDestroy();
    void OnSize();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowTitle;

    CSelectionDialog selectProcDialog;
    CListView memoryBlocksList;

    CMemoryScanner memoryScanner;

    HWND mainWindow;
    HWND listWindow;
    HWND dialogWindow;

    bool shouldExpandAll;

    std::vector<CAllocationInfo> memoryMap;
    void updateMemoryMap( int procId );
};