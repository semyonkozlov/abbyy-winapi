#pragma once

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
    void OnNotify( LPARAM lParam );
    void OnDestroy();

    void OnCmdRefresh();
    void OnCmdSelectProcess();
    void OnCmdExpandAll();
    void OnCmdCollapseAll();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowTitle;

    CSelectionDialog selectProcDialog;

    CListView memoryMapList;
    enum TMapListColumn {
        MLC_Address,
        MLC_Type,
        MLC_Size,
        MLC_Blocks,
        MLC_Protection,
        MLC_Details
    };

    CMemoryScanner memoryScanner;
    CConverter converter;

    HWND mainWindow;
    HWND listWindow;

    int processId;
    bool shouldExpandAll;

    std::vector<CAllocationInfo> memoryMap;

    void updateListWindow();
    void updateWindowCaption();

    void expandItem( int itemIndex );
    void collapseItem( int itemIndex, int numItems );
};