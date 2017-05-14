#include "OverlappedWindow.h"

const std::wstring COverlappedWindow::className = L"Overlapped Window";

COverlappedWindow::COverlappedWindow( const std::wstring& windowName ) :
    handle( nullptr ), windowName( windowName )
{
}

bool COverlappedWindow::RegisterClass()
{
    WNDCLASSEXW wcx;

    wcx.cbSize = sizeof( wcx );
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = windowProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = GetModuleHandle( nullptr );
    wcx.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
    wcx.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcx.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
    wcx.lpszMenuName = nullptr;
    wcx.lpszClassName = className.c_str();
    wcx.hIconSm = nullptr;

    return RegisterClassExW( &wcx );
}

bool COverlappedWindow::Create()
{
    handle = CreateWindowW(
        className.c_str(),
        windowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        GetModuleHandle( nullptr ),
        this );

    return handle;
}

void COverlappedWindow::Show( int cmdShow ) const
{
    ShowWindow( handle, cmdShow );
    UpdateWindow( handle );
}

void COverlappedWindow::OnDestroy()
{
    DestroyWindow( handle );
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lPararm )
{
    return 0;
}


