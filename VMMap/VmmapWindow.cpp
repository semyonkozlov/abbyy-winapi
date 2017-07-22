#define WINVER 0x0A00 
#define _WIN32_WINNT 0x0A00

#include <cassert>

#include <Windowsx.h>

#include "Utils.h"
#include "VmmapWindow.h"

#include <CommCtrl.h>

const CString CVmmapWindow::className = TEXT( "Vmmap window" );

CVmmapWindow::CVmmapWindow() :
    mainWindow( nullptr ), 
    listBox( nullptr ),
    expandRegionsFlag( false ),
    observedProcess( nullptr ),
    memoryViewer()
{
}

bool CVmmapWindow::RegisterClass()
{
    WNDCLASS windowClass;

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle( nullptr );
    windowClass.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
    windowClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = className.c_str();

    return ::RegisterClass( &windowClass );
}

bool CVmmapWindow::Create()
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

    listBox = CreateWindowEx(
        WS_EX_CLIENTEDGE, 
        TEXT( "LISTBOX" ),
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL, 
        7, 
        35, 
        300 * 2, 
        200 * 2, 
        mainWindow,
        nullptr,
        GetModuleHandle( nullptr ),
        nullptr);

    ListBox_AddString( listBox, TEXT( "Win sucks" ) );

    return mainWindow;
}

void CVmmapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    UpdateWindow( mainWindow );
}

void CVmmapWindow::OnCreate()
{
    // TODO: prompt user to enter process id
    observedProcess = GetCurrentProcess();

    int aTabs[5];
    aTabs[0] = 48;
    aTabs[1] = aTabs[0] + 40;
    aTabs[2] = aTabs[1] + 52;  // count
    aTabs[3] = aTabs[2] + 12;  // flags
    aTabs[4] = aTabs[3] + 20;  // description
    //ListBox_SetTabStops( listBox, _countof( aTabs ), aTabs );

    //ListBox_ResetContent( listBox );
    //ListBox_SetHorizontalExtent( listBox, 300 * LOWORD( GetDialogBaseUnits() ) );

    ListBox_AddString( listBox, TEXT( "Win sucks" ) );
    
    UpdateWindow( listBox );
    UpdateWindow( mainWindow );
}

void CVmmapWindow::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

LRESULT CVmmapWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    auto windowPtr = reinterpret_cast<CVmmapWindow*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );

    switch( message ) {
        case WM_NCCREATE:
        {
            windowPtr = static_cast<CVmmapWindow*>(
                reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
            SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr) );
            windowPtr->mainWindow = handle;
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
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}
