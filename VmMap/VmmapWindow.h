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
    void Show( int cmdShow );

protected:
    void OnSize();
    void OnCommand( WPARAM wParam );
    void OnNotify( LPARAM lParam );
    void OnDestroy();

    void OnCmdRefresh();
    void OnCmdSelectProcess();

    void OnCmdExpandAll();
    void OnCmdCollapseAll();

    void OnCmdCopyAddress() const;
    void OnCmdCopyAll() const;

    void OnCmdQuickHelp() const;
    void OnCmdAbout() const;

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

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

    mutable CMemoryScanner memoryScanner;
    mutable CConverter converter;

    HWND mainWindow;
    HWND listWindow;

    bool shouldExpandAll;

    std::vector<CAllocationInfo> memoryMap;

    void updateListWindow();
    void updateWindowCaption( int procId );

    void expandItem( int itemIndex );
    void collapseItem( int itemIndex, int numItems );

    CString getMemoryMapText() const;
    void saveToClipboard( const CString& text ) const;
};