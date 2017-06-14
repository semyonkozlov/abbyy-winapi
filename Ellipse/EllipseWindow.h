#pragma once

#include <string>

#include <Windows.h>

class CEllipseWindow {
    using CString = std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>>;

public:
    CEllipseWindow( const CString& windowName = TEXT( "Ellipse" ) );

    static bool RegisterClass();

    bool Create();
    bool Create( HWND parentHandle );
    void Show( int cmdShow ) const;

    HWND GetHandle() const noexcept;

protected:
    void OnCreate();
    void OnPaint();
    void OnTimer();
    void OnDestroy();
    void OnLButtonDown();

private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    HWND windowHandle;
    CString windowName;

    UINT_PTR timer;
    static const int timerDelay = 50;

    double t;
    static const double dt;

    static const int a = 100;
    static const int b = 50;
};
