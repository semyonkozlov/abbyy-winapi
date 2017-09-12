#include <cassert>

#include <Windows.h>
#include <CommCtrl.h>

#include "Utils.h"
#include "Resource.h"
#include "VmMapWindow.h"

const CString CVmMapWindow::className = TEXT( "VMMAP" );

CVmMapWindow::CVmMapWindow() :
    windowTitle(),
    selectProcDialog(),
    memoryBlocksList(),
    memoryScanner(),
    mainWindow( nullptr ),
    listWindow( nullptr ),
    dialogWindow( nullptr ),
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

    listWindow = memoryBlocksList.Create( mainWindow );
    
    dialogWindow = selectProcDialog.Create( mainWindow );

    return mainWindow;
}

void CVmMapWindow::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    memoryBlocksList.Show( cmdShow );
}

void CVmMapWindow::OnCreate()
{
    // TODO
}

void CVmMapWindow::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
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
        case WM_DESTROY:
        {
            vmmap->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}

void CVmMapWindow::updateMemoryMap( int procId )
{
    memoryScanner.AttachToProcess( procId );

    memoryMap.clear();
    CAllocationInfo allocationInfo{};

    void* currentAddress = nullptr;
    while( memoryScanner.GetAllocationInfo( currentAddress, &allocationInfo ) ) {
        memoryMap.push_back( std::move( allocationInfo ) );
        allocationInfo = std::move( CAllocationInfo{} );
    }

    memoryScanner.DetachFromProcess();
}
