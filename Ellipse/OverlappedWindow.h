#pragma once

#include <string>

#include <Windows.h>

class COverlappedWindow {
public:
    COverlappedWindow( const std::string& windowName = "Ellipse" );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

protected:
    void OnCreate();
    void OnNCCreate( HWND otherHandle );
    void OnPaint();
    void OnTimer();
    void OnDestroy();

private:
    static const std::string className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    HWND windowHandle;
    std::string windowName;

    UINT_PTR timer;
    static const int timerDelay = 50;

    double t;
    static const double dt;

    static const int a = 100;
    static const int b = 50;
};
