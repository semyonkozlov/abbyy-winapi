#include <cassert>

#include <Windows.h>

#include "Utils.h"
#include "Resource.h"
#include "VmMapWindow.h"

const CString CVmMapWindow::className = TEXT( "VMMAP" );

CVmMapWindow::CVmMapWindow( CString windowName ) :
    windowName( windowName ),
    selectProcDialog(),
    mainWindow( nullptr ),
    procsList( nullptr ),
    dialogWindow( nullptr )
{
}

void CVmMapWindow::RegisterClass()
{
    WNDCLASS windowClass;
    ZeroMemory( &windowClass, sizeof( WNDCLASS ) );

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = GetModuleHandle( nullptr );
    windowClass.hIcon = LoadIcon( GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDI_ICON ) );
    windowClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
    windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );
    windowClass.lpszClassName = className.c_str();

    ATOM classAtom = ::RegisterClass( &windowClass );
    assert( classAtom != 0 );
}

HWND CVmMapWindow::Create()
{
    mainWindow = CreateWindow(
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
    assert( mainWindow != nullptr );

    procsList = nullptr; // TODO
    //assert( procsList != nullptr );
    
    dialogWindow = selectProcDialog.Create( mainWindow );

    return mainWindow;
}

void CVmMapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    ShowWindow( procsList, cmdShow );
}

bool CVmMapWindow::IsDialogMessage( LPMSG messagePtr ) const
{
    return ::IsDialogMessage( dialogWindow, messagePtr );
}

void CVmMapWindow::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

LRESULT CVmMapWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CVmMapWindow* vmmap = nullptr;
    if( message == WM_NCCREATE ) {
        vmmap = static_cast<CVmMapWindow*>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( vmmap ) );

        return DefWindowProc( handle, message, wParam, lParam );
    }

    vmmap = reinterpret_cast<CVmMapWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) { // TODO
        case WM_DESTROY:
        {
            vmmap->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}
