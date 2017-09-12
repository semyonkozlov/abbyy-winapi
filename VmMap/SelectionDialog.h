#pragma once

#include <Windows.h>

#include "ListView.h"

class CSelectionDialog {
public:
    CSelectionDialog();

    HWND Create( HWND parent );
    void Show( int cmdShow ) const;

protected:
    void OnInit( HWND handle );

private:
    static INT_PTR CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CListView procsList;

    HWND dialog;
    HWND list;
};