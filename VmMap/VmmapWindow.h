#pragma once

#include <Windows.h>

#include "SelectionDialog.h"
#include "ListView.h"
#include "MemoryScanner.h"
#include "Converter.h"
#include "Toolhelp.h"
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

    void OnCmdRefresh();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowTitle;

    CSelectionDialog selectProcDialog;
    CListView memMapList;

    CMemoryScanner memoryScanner;
    CConverter itemConverter;
    CToolhelp toolhelp;

    HWND mainWindow;
    HWND listWindow;
    HWND dialogWindow;

    int processId;
    bool shouldExpandAll;
    std::vector<CRegionInfo> memoryMap;

    void updateListWindow();
    CString obtainAllocationDetails( const CAllocationInfo& allocationInfo );
};