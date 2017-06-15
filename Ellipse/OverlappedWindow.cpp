#include <cassert>

#include "Resource.h"
#include "OverlappedWindow.h"

const COverlappedWindow::CString COverlappedWindow::className = TEXT( "Overlapped Window" );

COverlappedWindow::COverlappedWindow( const CString& windowName ) :
    windowHandle( nullptr ), windowName( windowName ), childWindows( numChildren )
{
}

bool COverlappedWindow::RegisterClass()
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

HWND COverlappedWindow::GetHandle() const noexcept
{
    return windowHandle;    
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

    int rectCenterX = (rect.left + rect.right) / 2;
    int rectCenterY = (rect.top + rect.bottom) / 2;

    SetWindowPos( 
        childWindows[0].GetHandle(), HWND_TOP,
        rect.left, rect.top,
        childWidth, childHeight, 
        0 );

    SetWindowPos(  
        childWindows[1].GetHandle(), HWND_TOP, 
        rectCenterX, rect.top,
        childWidth, childHeight, 
        0 );

    SetWindowPos(
        childWindows[2].GetHandle(), HWND_TOP,
        rect.left, rectCenterY,
        childWidth, childHeight, 
        0 );

    SetWindowPos( 
        childWindows[3].GetHandle(), HWND_TOP, 
        rectCenterX, rectCenterY, 
        childWidth, childHeight, 
        0 );
}

void COverlappedWindow::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

void COverlappedWindow::OnArrowKey( WPARAM wParam )
{
    HWND currentFocus = GetFocus();

    int activeWindowId = 0;
    for( ; activeWindowId < numChildren; ++activeWindowId ) {
        if( currentFocus == childWindows[activeWindowId].GetHandle() ) {
            break;
        }
    }
    if( activeWindowId >= numChildren ) {
        return;
    }

    HWND newFocus = nullptr;
    switch( LOWORD( wParam ) ) {
        case ID_UP:
        case ID_DOWN:
            newFocus = childWindows[(activeWindowId + 2) % numChildren].GetHandle();
            break;
        case ID_LEFT:
            newFocus = childWindows[(activeWindowId + 3) % numChildren].GetHandle();
            break;
        case ID_RIGHT:
            newFocus = childWindows[(activeWindowId + 1) % numChildren].GetHandle();
            break;
        default:
            return;
    }

    SetFocus( newFocus );
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{   
    auto windowPtr = reinterpret_cast<COverlappedWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );

    switch( message ) {
        case WM_NCCREATE:
        {
            windowPtr = static_cast<COverlappedWindow*>(
                reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams);
            SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( windowPtr ) );
            windowPtr->windowHandle = handle;
            return DefWindowProc( handle, message, wParam, lParam );
        }
        case WM_COMMAND:
        {
            windowPtr->OnArrowKey( wParam );
            return EXIT_SUCCESS;
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
