#pragma once

#include <string>

#include <Windows.h>

class CTextEditor {
    using CString = std::basic_string<TCHAR>;

public:
    explicit CTextEditor( const CString& windowName = TEXT( "Text Editor" ) );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

protected:
    void OnCreate();
    void OnSize();
    void OnCommand( WPARAM wParam, LPARAM lParam );
    bool OnClose( );
    void OnDestroy();
    
private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowName;

    HWND mainWindow;
    HWND editControl;
    HWND dialog;

    bool hasInput;
};