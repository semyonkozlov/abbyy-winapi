#include <cassert>

#include "OverlappedWindow.h"

const COverlappedWindow::CString COverlappedWindow::className = TEXT( "Overlapped Window" );

COverlappedWindow::COverlappedWindow( const CString& windowName ) :
    windowHandle( nullptr ), windowName( windowName ), childWindows( numChildren )
{
}

bool COverlappedWindow::RegisterClass()
{
    WNDCLASS wcx;

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

    return ::RegisterClass( &wcx );
}

bool COverlappedWindow::Create()
{
    windowHandle = CreateWindow(
        className.c_str(),
        windowName.c_str(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        nullptr,
        GetModuleHandle( nullptr ),
        this );

    assert( windowHandle != nullptr );

    return windowHandle;
}

void COverlappedWindow::Show( int cmdShow ) const
{
    ShowWindow( windowHandle, cmdShow );
    UpdateWindow( windowHandle );

    for( auto&& childWindow : childWindows ) {
        childWindow.Show( cmdShow );
    }
}

void COverlappedWindow::OnCreate()
{
    CEllipseWindow::RegisterClass();
    for( auto&& childWindow : childWindows ) {
        childWindow.Create( windowHandle );
    }
}

void COverlappedWindow::OnSize()
{
    RECT rect;
    GetClientRect( windowHandle, &rect );

    int childWidth = (rect.right - rect.left) / 2;
    int childHeight = (rect.bottom - rect.top) / 2;

    SetWindowPos( childWindows[0].GetHandle(), HWND_TOP,
        rect.left, rect.top, childWidth, childHeight, 0 );
    SetWindowPos( childWindows[1].GetHandle(), HWND_TOP,
        (rect.left + rect.right) / 2, rect.top, childWidth, childHeight, 0 );
    SetWindowPos( childWindows[2].GetHandle(), HWND_TOP, 
        rect.left, (rect.top + rect.bottom) / 2, childWidth, childHeight, 0 );
    SetWindowPos( childWindows[3].GetHandle(), HWND_TOP, 
        (rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2, childWidth, childHeight, 0 );
}

void COverlappedWindow::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{   
    auto windowPtr = reinterpret_cast<COverlappedWindow*>(GetWindowLongPtr( handle, GWLP_USERDATA ));

    switch( message ) {
        case WM_NCCREATE:
        {
            windowPtr = static_cast<COverlappedWindow*>(
                reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
            SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr) );
            windowPtr->windowHandle = handle;
            return DefWindowProc( handle, message, wParam, lParam );
        }
        case WM_CREATE:
        {
            windowPtr->OnCreate();
            return EXIT_SUCCESS;
        }
        case WM_SIZE:
        {
            windowPtr->OnSize();
            return EXIT_SUCCESS;
        }
        case WM_DESTROY:
        {
            windowPtr->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}
