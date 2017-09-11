#pragma once

#include <Windows.h>

#include "SelectProcDialog.h"
#include "ProcsList.h"
#include "Utils.h"

class CVmMapWindow {
public:
    CVmMapWindow();

    static void RegisterClass();

    HWND Create();
    void Show( int cmdShow ) const;

    bool IsDialogMessage( LPMSG messagePtr ) const;

protected:
    void OnDestroy();
    void OnSize();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowTitle;

    CSelectProcDialog selectProcDialog;
    CProcsList procsList;

    HWND mainWindow;
    HWND listWindow;
    HWND dialogWindow;
};