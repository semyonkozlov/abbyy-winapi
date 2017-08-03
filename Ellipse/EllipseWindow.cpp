#define NOMINMAX

#include <algorithm> 
#include <cassert>

#include "EllipseWindow.h"

const CEllipseWindow::CString CEllipseWindow::className = TEXT( "ELLIPSE" );
const double CEllipseWindow::dt = 0.1;

CEllipseWindow::CEllipseWindow( const CString& windowName ) :
    windowHandle( nullptr ), windowName( windowName ), timer( 0 ), t( 0 )
{
}

bool CEllipseWindow::RegisterClass()
{
    WNDCLASS windowClass;

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle( nullptr );
    windowClass.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
    windowClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = className.c_str();

    return ::RegisterClass( &windowClass );
}

bool CEllipseWindow::Create()
{
    windowHandle = CreateWindow(
        className.c_str(),
        windowName.c_str(),
        WS_OVERLAPPEDWINDOW, 
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

bool CEllipseWindow::Create( HWND parentHandle )
{
    windowHandle = CreateWindow(
        className.c_str(),
        windowName.c_str(),
        WS_CHILD | WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        parentHandle,
        nullptr,
        GetModuleHandle( nullptr ),
        this );
    assert( windowHandle != nullptr );

    return windowHandle;
}

void CEllipseWindow::Show( int cmdShow ) const
{
    ShowWindow( windowHandle, cmdShow );
    UpdateWindow( windowHandle );
}

void CEllipseWindow::OnCreate()
{
    timer = SetTimer( windowHandle, 0, timerDelay, nullptr );
}

void CEllipseWindow::OnPaint()
{ 
    PAINTSTRUCT paintStruct;
    HDC windowContext = BeginPaint( windowHandle, &paintStruct );

    RECT rect;
    GetClientRect( windowHandle, &rect );

    HDC bufferContext = CreateCompatibleDC( windowContext );
    HBITMAP windowBuffer = CreateCompatibleBitmap( windowContext, 
        rect.right - rect.left, 
        rect.bottom - rect.top );

    HGDIOBJ oldWindowBuffer = SelectObject( bufferContext, windowBuffer );

    FillRect( bufferContext, &rect, static_cast<HBRUSH>( GetStockObject( LTGRAY_BRUSH ) ) );
    
    // drawing ellipse
    HPEN pen = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
    HGDIOBJ oldPen = SelectObject( bufferContext, pen );

    const int numPalettes = 16;
    HBRUSH brush = CreateSolidBrush( PALETTEINDEX( int( t ) % numPalettes ) );
    HGDIOBJ oldBrush = SelectObject( bufferContext, brush );

    int r = std::min( (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 ) - std::max( a, b );
    int x = (rect.left + rect.right) / 2 + r * std::cos( t );
    int y = (rect.top + rect.bottom) / 2 + r * std::sin( t );
    Ellipse( bufferContext, x - a, y - b, x + a, y + b );

    SelectObject( bufferContext, oldPen );
    SelectObject( bufferContext, oldBrush );
    DeleteObject( brush );
    DeleteObject( pen );

    BitBlt( windowContext,
        rect.left,
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        bufferContext, 
        0, 
        0,
        SRCCOPY );

    SelectObject( bufferContext, oldWindowBuffer );
    DeleteObject( windowBuffer );
    DeleteDC( bufferContext );
    
    EndPaint( windowHandle, &paintStruct );
}

void CEllipseWindow::OnTimer()
{
    if (GetFocus() != windowHandle ) {
        return;
    }

    t += dt;

    RECT rect;
    GetClientRect( windowHandle, &rect );
    InvalidateRect( windowHandle, &rect, FALSE );
}

void CEllipseWindow::OnDestroy()
{
    KillTimer( windowHandle, timer );
    PostQuitMessage( EXIT_SUCCESS );
}

void CEllipseWindow::OnLButtonDown()
{
    SetFocus( windowHandle );
}

LRESULT CEllipseWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CEllipseWindow* windowPtr;
    if( message == WM_NCCREATE ) {
        windowPtr = static_cast<CEllipseWindow*>(
            reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( windowPtr ) );
        windowPtr->windowHandle = handle;

        return DefWindowProc( handle, message, wParam, lParam );
    }

    windowPtr = reinterpret_cast<CEllipseWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) {
        case WM_CREATE:
        {
            windowPtr->OnCreate();
            return EXIT_SUCCESS;
        }
        case WM_DESTROY:
        {
            windowPtr->OnDestroy();
            return EXIT_SUCCESS;
        }
        case WM_ERASEBKGND:
        {
            return EXIT_SUCCESS;
        }
        case WM_PAINT:
        {
            windowPtr->OnPaint();
            return EXIT_SUCCESS;
        }
        case WM_TIMER:
        {
            windowPtr->OnTimer();
            return EXIT_SUCCESS;
        }
        case WM_LBUTTONDOWN:
        {
            windowPtr->OnLButtonDown();
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}

HWND CEllipseWindow::GetHandle() const noexcept
{
    return windowHandle;
}
