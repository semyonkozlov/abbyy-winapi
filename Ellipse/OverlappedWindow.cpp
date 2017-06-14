#define NOMINMAX

#include <algorithm> 
#include <cassert>

#include "OverlappedWindow.h"

const COverlappedWindow::CString COverlappedWindow::className = TEXT( "Overlapped Window" );
const double COverlappedWindow::dt = 0.1;

COverlappedWindow::COverlappedWindow( const CString& windowName ) :
    windowHandle( nullptr ), windowName( windowName ), timer( 0 ), t( 0 )
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

void COverlappedWindow::Show( int cmdShow ) const
{
    ShowWindow( windowHandle, cmdShow );
    UpdateWindow( windowHandle );
}

void COverlappedWindow::OnCreate()
{
    timer = SetTimer( windowHandle, 0, timerDelay, nullptr );
}

void COverlappedWindow::OnNCCreate( HWND otherHandle )
{
    windowHandle = otherHandle;
}

void COverlappedWindow::OnPaint()
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

    HBRUSH brush = CreateSolidBrush( RGB( 255, 255, 255 ) );
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

void COverlappedWindow::OnTimer()
{
    t += dt;

    RECT rect;
    GetClientRect( windowHandle, &rect );
    InvalidateRect( windowHandle, &rect, FALSE );
}

void COverlappedWindow::OnDestroy()
{
    KillTimer( windowHandle, timer );
    PostQuitMessage( EXIT_SUCCESS );
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    auto windowPtr = reinterpret_cast<COverlappedWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );

    switch( message ) {
        case WM_NCCREATE:
        {
            windowPtr = static_cast<COverlappedWindow*>( 
                reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
            SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( windowPtr ) );
            windowPtr->OnNCCreate( handle );
            return DefWindowProc( handle, message, wParam, lParam );
        }
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
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}
