#pragma once

#include <Windows.h>

class CSelectProcDialog {
public:
    CSelectProcDialog();

    HWND Create( HWND parent );
    void Show( int cmdShow ) const;

protected:
    void OnInit( HWND handle );

private:
    static INT_PTR CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    HWND dialog;
};