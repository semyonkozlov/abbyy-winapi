#pragma once

#include <vector>

#include <Windows.h>

#include "EllipseWindow.h"

class COverlappedWindow {
    using CString = std::basic_string<TCHAR>;

public:
    explicit COverlappedWindow( const CString& windowName = TEXT( "OverlappedWindow" ) );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

    HWND GetHandle() const noexcept;

protected:
    void OnCreate();
    void OnSize();
    void OnDestroy();
    void OnArrowKey( WPARAM wParam );

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    HWND windowHandle;
    CString windowName;

    std::vector<CEllipseWindow> childWindows;
    static const int numChildren = 4;
};