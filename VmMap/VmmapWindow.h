#pragma once

#include <map>

#include <Windows.h>

#include "SelectionDialog.h"
#include "ListView.h"
#include "MemoryScanner.h"
#include "Converter.h"
#include "Utils.h"

class CVmMapWindow {
public:
    CVmMapWindow();

    static void RegisterClass();

    HWND Create();
    void Show( int cmdShow ) const;

protected:
    void OnCreate();
    void OnSize();
    void OnCommand( WPARAM wParam );
    void OnDestroy();

    void OnCmdSelectProcess();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowTitle;

    CSelectionDialog selectProcDialog;
    CListView memMapList;

    CMemoryScanner memoryScanner;
    CConverter itemConverter;

    HWND mainWindow;
    HWND listWindow;
    HWND dialogWindow;

    bool shouldExpandAll;

    void updateListWindow();
    std::vector<CRegionInfo> memoryMap;
};