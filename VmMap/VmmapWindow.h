#pragma once

#include "Windows.h"
#include "CSelectProcDialog.h"
#include "Utils.h"

class CVmMapWindow {
public:
    explicit CVmMapWindow( CString windowName = TEXT( "VmMap" ) );

    static void RegisterClass();

    HWND Create();
    void Show( int cmdShow ) const;

    bool IsDialogMessage( LPMSG messagePtr ) const;

protected:
    void OnDestroy();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowName;

    CSelectProcDialog selectProcDialog;

    HWND mainWindow;
    HWND procsList;
    HWND dialogWindow;
};