#include <cassert>

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include "Utils.h"
#include "Converter.h"
#include "Resource.h"
#include "VmMapWindow.h"

const CString CVmMapWindow::className = TEXT( "VMMAP" );

CVmMapWindow::CVmMapWindow() :
    windowTitle(),
    selectProcDialog(),
    memMapList(),
    memoryScanner(),
    mainWindow( nullptr ),
    listWindow( nullptr ),
    dialogWindow( nullptr ),
    processId( -1 ),
    shouldExpandAll( false ),
    memoryMap()
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
        windowTitle.c_str(),
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

    listWindow = memMapList.Create( mainWindow );
    
    dialogWindow = selectProcDialog.Create( mainWindow );

    return mainWindow;
}

void CVmMapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    memMapList.Show( cmdShow );
}

void CVmMapWindow::OnCreate()
{
    // TODO
}

void CVmMapWindow::OnDestroy()
{
    // TODO close handles
    PostQuitMessage( EXIT_SUCCESS );
}

void CVmMapWindow::OnCmdRefresh()
{
    memoryScanner.GetMemoryMap( &memoryMap );
    updateListWindow();
}

void CVmMapWindow::OnSize()
{
    RECT rect;
    GetClientRect( mainWindow, &rect );
    SetWindowPos(
        listWindow,
        HWND_TOP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0 );
}

void CVmMapWindow::OnCommand( WPARAM wParam )
{
    switch( LOWORD( wParam ) ) {
        case ID_SELECT_PROCESS:
        {
            if( processId != -1 ) {
                memoryScanner.DetachFromProcess();
            }

            processId = GetCurrentProcessId(); // TODO
            memoryScanner.AttachToProcess( processId );

            OnCmdRefresh();
            break;
        }
        case ID_QUICK_HELP:
        {
            MessageBox( mainWindow, TEXT( "Help me, please..." ), TEXT( "HELP!" ), MB_OK );
            break;
        }
        case ID_EXPAND_ALL:
        {
            shouldExpandAll = true;
            updateListWindow();
            break;
        }
        case ID_COLLAPSE_ALL:
        {
            shouldExpandAll = false;
            updateListWindow();
            break;
        }
        case ID_REFRESH:
        {
            OnCmdRefresh();
            break;
        }
        case ID_EXIT:
        {
            OnDestroy();
            break;
        }
        default:
        {
            MessageBox( mainWindow, TEXT( "What?" ), TEXT( "Error" ), MB_OK );
        }
    }
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
        case WM_CREATE:
        {
            vmmap->OnCreate();
            return EXIT_SUCCESS;
        }
        case WM_SIZE:
        {
            vmmap->OnSize();
            return EXIT_SUCCESS;
        }
        case WM_COMMAND:
        {
            vmmap->OnCommand( wParam );
            return EXIT_SUCCESS;
        }
        case WM_DESTROY:
        {
            vmmap->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}

void CVmMapWindow::updateListWindow()
{
    SetWindowRedraw( listWindow, FALSE );

    memMapList.DeleteAllItems();
    for( auto&& allocationInfo : memoryMap ) {
        memMapList.AddItem( itemConverter.AllocationInfoToItem( allocationInfo ) );

        for( auto&& regionInfo : allocationInfo.RegionsInfo ) {
            if( shouldExpandAll && regionInfo.Type != MEM_FREE ) {
                memMapList.AddItem( itemConverter.RegionInfoToItem( regionInfo ) );
            }
        }
    }

    SetWindowRedraw( listWindow, TRUE );
    UpdateWindow( listWindow );
}