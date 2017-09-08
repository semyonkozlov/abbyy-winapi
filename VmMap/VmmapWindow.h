#pragma once

#include "Windows.h"
#include "Utils.h"

class CVmMapWindow {
public:
    explicit CVmMapWindow( CString windowName = TEXT( "VmMap" ) );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

    bool IsDialogMessage( LPMSG messagePtr ) const;

protected:

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowName;

    HWND mainWindow;
    HWND procsList;
    HWND selectProcDialog;
};