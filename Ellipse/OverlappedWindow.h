#pragma once

#include <Windows.h>

#include <string>

class COverlappedWindow {
public:
    COverlappedWindow( const std::wstring& windowName = L"Main window" );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

protected:
    void OnDestroy();

private:
    static const std::wstring className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lPararm );

    HWND handle;
    std::wstring windowName;
};
