#include <cassert>

#include <Windows.h>
#include <WinUser.h>

#include "TextEditor.h"

const CTextEditor::CString CTextEditor::className = TEXT( "TEXTEDITOR" );

CTextEditor::CTextEditor( const CString& windowName ) :
    windowName( windowName ),
    mainWindow( nullptr ),
    editControl( nullptr ), 
    dialog( nullptr )
{
}

bool CTextEditor::RegisterClass()
{
    WNDCLASS windowClass;
    ZeroMemory( &windowClass, sizeof( WNDCLASS ) );

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = GetModuleHandle( nullptr );
    windowClass.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
    windowClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
    windowClass.lpszClassName = className.c_str();

    return ::RegisterClass( &windowClass );
}

bool CTextEditor::Create()
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

    editControl = CreateWindow(
        TEXT( "EDIT" ),
        nullptr,       
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        mainWindow,       
        nullptr, 
        GetModuleHandle( nullptr ),
        this );     
    assert( editControl != nullptr );

    return mainWindow;
}

void CTextEditor::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    ShowWindow( editControl, cmdShow );
}

void CTextEditor::OnCreate()
{
    return;
}

void CTextEditor::OnSize()
{
    RECT rect;
    GetClientRect( mainWindow, &rect );
    SetWindowPos( 
        editControl,
        HWND_TOP, 
        rect.left, 
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        0 );
}

void CTextEditor::OnCommand( WPARAM wParam, LPARAM lParam )
{
}

int CTextEditor::OnClose()
{
    return MessageBox(
        mainWindow,
        TEXT( "Quit?" ),
        TEXT( "Quit?" ),
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 );
}

void CTextEditor::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

LRESULT CTextEditor::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    auto windowPtr = reinterpret_cast<CTextEditor*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );

    switch( message ) {
        case WM_NCCREATE:
        {
            windowPtr = static_cast<CTextEditor*>(
                reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
            SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr) );
            windowPtr->mainWindow = handle;
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
        case WM_COMMAND:
        {
            windowPtr->OnCommand( wParam, lParam );
            return EXIT_SUCCESS;
        }
        case WM_CLOSE:
        {
            int msgBoxId = windowPtr->OnClose();
            if( msgBoxId == IDYES ) {
                return DefWindowProc( handle, message, wParam, lParam );
            }
            else {
                return EXIT_SUCCESS;
            }
        }
        case WM_DESTROY:
        {
            windowPtr->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
        {
            return DefWindowProc( handle, message, wParam, lParam );
        }
    }
}
