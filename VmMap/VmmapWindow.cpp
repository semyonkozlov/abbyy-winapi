#include <cassert>

#include <Windows.h>

#include "Utils.h"
#include "Resource.h"
#include "VmMapWindow.h"

const CString CVmMapWindow::className = TEXT( "VMMAP" );

CVmMapWindow::CVmMapWindow( CString windowName ) :
    windowName( windowName ),
    mainWindow( nullptr ),
    procsList( nullptr ),
    selectProcDialog( nullptr )
{
}

bool CVmMapWindow::RegisterClass()
{
    WNDCLASS windowClass;
    ZeroMemory( &windowClass, sizeof( WNDCLASS ) );

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = GetModuleHandle( nullptr );
    windowClass.hIcon = LoadIcon( GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDI_ICON ) );
    windowClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );
    windowClass.lpszClassName = className.c_str();

    return ::RegisterClass( &windowClass );
}

bool CVmMapWindow::Create()
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
    
    selectProcDialog = nullptr; // TODO

    return mainWindow && procsList && selectProcDialog;
}

void CVmMapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    ShowWindow( procsList, cmdShow );
}

bool CVmMapWindow::IsDialogMessage( LPMSG messagePtr ) const
{
    return ::IsDialogMessage( selectProcDialog, messagePtr );
}

LRESULT CVmMapWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CVmMapWindow* vmMapWindow = nullptr;
    if( message == WM_NCCREATE ) {
        vmMapWindow = static_cast<CVmMapWindow*>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( vmMapWindow ) );

        return DefWindowProc( handle, message, wParam, lParam );
    }

    vmMapWindow = reinterpret_cast<CVmMapWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) { // TODO
    }
}
