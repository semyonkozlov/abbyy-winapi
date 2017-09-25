#pragma once

#include <Windows.h>
#include <CommCtrl.h>

#include "ListView.h"
#include "Toolhelp.h"
#include "Converter.h"

class CSelectionDialog {
public:
    CSelectionDialog();

    INT_PTR CreateDialogBox( HWND parent );

protected:
    void OnInit( HWND handle );
    void OnNotify( LPARAM lParam );
    INT_PTR OnCommand( WPARAM wParam );
    void OnClose();

    void OnCmdPushbuttonOk();
    void OnCmdPushbuttonRefresh();

private:
    static INT_PTR CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CToolhelp toolhelp;
    CConverter converter;

    CListView procsList;
    enum TProcListColumn {
        PLC_Name,
        PLC_Pid,
        PLC_WorkingSet,
        PLT_Architecture
    };

    HWND dialogWindow;
    HWND listWindow;
};