#include <cassert>
#include <string>
#include <algorithm>

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
    memoryMapList(),
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

    listWindow = memoryMapList.Create( mainWindow );

    memoryMapList.SetColumns( {
        TEXT( "Address" ),
        TEXT( "Type" ),
        TEXT( "Size" ),
        TEXT( "Blocks" ),
        TEXT( "Protection" ),
        TEXT( "Details" ) } );
    
    dialogWindow = selectProcDialog.Create( mainWindow );

    return mainWindow;
}

void CVmMapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    memoryMapList.Show( cmdShow );
}

void CVmMapWindow::OnCreate()
{
    // TODO
}

void CVmMapWindow::OnDestroy()
{
    // TODO close handles
    memoryScanner.DetachFromProcess();
    PostQuitMessage( EXIT_SUCCESS );
}

void CVmMapWindow::OnCmdRefresh()
{
    memoryScanner.GetMemoryMap( &memoryMap );
    updateListWindow();
}

void CVmMapWindow::OnCmdSelectProcess()
{
    selectProcDialog.Show( SW_SHOW );
    if( processId != -1 ) {
        memoryScanner.DetachFromProcess();
    }

    processId = GetCurrentProcessId(); // TODO
    memoryScanner.AttachToProcess( processId );

    OnCmdRefresh();
}

void CVmMapWindow::OnCmdExpandAll()
{
    shouldExpandAll = true;
    updateListWindow();
}

void CVmMapWindow::OnCmdCollapseAll()
{
    shouldExpandAll = false;
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
            OnCmdSelectProcess();
            break;
        }
        case ID_QUICK_HELP:
        {
            MessageBox( mainWindow, TEXT( "Help me, please..." ), TEXT( "HELP!" ), MB_OK );
            break;
        }
        case ID_EXPAND_ALL:
        {
            OnCmdExpandAll();
            break;
        }
        case ID_COLLAPSE_ALL:
        {
            OnCmdCollapseAll();
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

void CVmMapWindow::OnNotify( LPARAM lParam )
{
    auto notificationMessage = reinterpret_cast<LPNMHDR>( lParam );
    if( notificationMessage->idFrom == IDC_LISTVIEW && notificationMessage->code == NM_DBLCLK ) {
        int itemIndex = reinterpret_cast<LPNMLISTVIEW>( lParam )->iItem;
        
        CString blocksText = memoryMapList.GetItemText( itemIndex, MLC_Blocks );
        if( blocksText.empty() ) {
            return;
        }
        int numBlocks = std::stoi( blocksText );

        // region info item has leading whitespace
        bool isExpanded = (memoryMapList.GetItemText( itemIndex + 1, MLC_Address ).front() == TEXT( ' ' ));
        
        if( isExpanded ) {
            collapseItem( itemIndex, numBlocks );
        } else {
            expandItem( itemIndex );
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
        case WM_NOTIFY:
        {
            vmmap->OnNotify( lParam );
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

    memoryMapList.DeleteAllItems();
    for( auto&& allocationInfo : memoryMap ) {
        memoryMapList.AddItem( itemConverter.AllocationInfoToItem( allocationInfo ) );

        for( auto&& regionInfo : allocationInfo.RegionsInfo ) {
            if( shouldExpandAll && regionInfo.Type != MEM_FREE ) {
                memoryMapList.AddItem( itemConverter.RegionInfoToItem( regionInfo ) );
            }
        }
    }

    SetWindowRedraw( listWindow, TRUE );
    UpdateWindow( listWindow );
}

void CVmMapWindow::expandItem( int itemIndex )
{
    CString addressText = memoryMapList.GetItemText( itemIndex, MLC_Address );
    const void* address = reinterpret_cast<const void*>(std::stoll( addressText, nullptr, 16 ));

    auto allocationIter = std::find_if(
        std::begin( memoryMap ),
        std::end( memoryMap ),
        [address]( const CAllocationInfo& a ) { return a.AllocationBaseAddress == address; } );

    for( auto it = std::rbegin( allocationIter->RegionsInfo );
        it != std::rend( allocationIter->RegionsInfo );
        ++it ) 
    {
        memoryMapList.AddItem( itemConverter.RegionInfoToItem( *it ), itemIndex + 1 );
    }
}

void CVmMapWindow::collapseItem( int itemIndex, int numItems )
{
    for( int i = 0; i < numItems; ++i ) {
        memoryMapList.DeleteItem( itemIndex + 1 );
    }
}
