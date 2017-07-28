#include <cassert>

#include <Windows.h>
#include <WinUser.h>

#include "Resource.h"
#include "TextEditor.h"
#include <CommCtrl.h>

const CTextEditor::CString CTextEditor::className = TEXT( "TEXTEDITOR" );

CTextEditor::CTextEditor( const CString& windowName ) :
    windowName( windowName ),
    mainWindow( nullptr ),
    editControl( nullptr ),
    settingsDialog( nullptr ),
    hasInput( false )
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
    windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );
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
        CW_USEDEFAULT, // TODO
        CW_USEDEFAULT,
        HWND_DESKTOP,
        nullptr,
        GetModuleHandle( nullptr ),
        this );
    assert( mainWindow != nullptr );

    editControl = CreateWindow(
        TEXT( "EDIT" ),
        nullptr,       
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        mainWindow,       
        nullptr, 
        GetModuleHandle( nullptr ),
        this );     
    assert( editControl != nullptr );

    settingsDialog = CreateDialogParam(
        GetModuleHandle( nullptr ),
        MAKEINTRESOURCE( IDD_DIALOG ),
        mainWindow,
        settingsProc,
        reinterpret_cast<LONG>(this) );
    assert( settingsDialog != nullptr );

    return mainWindow;
}

void CTextEditor::Show( int cmdShow ) const
{
    ShowWindow( mainWindow, cmdShow );
    ShowWindow( editControl, cmdShow );
}

bool CTextEditor::IsDialogMessage( LPMSG messagePtr ) const
{
    return ::IsDialogMessage( settingsDialog, messagePtr );
}

void CTextEditor::OnCreate()
{
    // TODO
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

void CTextEditor::OnCommand( WPARAM wParam )
{
    switch( HIWORD( wParam ) ) {
        case EN_CHANGE:
        {
            hasInput = true;
            break;
        }
    }

    switch( LOWORD( wParam ) ) {
        case ID_FILE_SAVE:
        {
            saveInput();
            return;
        }
        case ID_FILE_EXIT:
        {
            SendMessage( mainWindow, WM_CLOSE, 0, 0 );
            return;
        }
        case ID_VIEW_SETTINGS:
        {
            ShowWindow( settingsDialog, SW_SHOW );
            return;
        }
    }
}

bool CTextEditor::OnClose( )
{
    if( hasInput ) {
        int messageBoxId = MessageBox(
            mainWindow,
            TEXT( "Do you want to save changes?" ),
            windowName.c_str(),
            MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1 );

        switch( messageBoxId ) {
            case IDYES:
            {
                return saveInput();
            }
            case IDNO:
            {
                return true;
            }
            case IDCANCEL:
            {
                return false;
            }
            default:
            {
                return true;
            }
        }
    }
    return true;
}

void CTextEditor::OnDestroy()
{
    PostQuitMessage( EXIT_SUCCESS );
}

void CTextEditor::OnInitSettingsDlg( HWND handle )
{
    SendMessage( 
        GetDlgItem( handle, IDC_SLIDER_FONTSIZE ), 
        TBM_SETRANGE, 
        TRUE, MAKELONG( 8, 72 ) );
    SendMessage( 
        GetDlgItem( handle, IDC_SLIDER_TRANSPARENCY ),
        TBM_SETRANGE, 
        TRUE, MAKELONG( 0, 255 ) );
}

INT_PTR CTextEditor::OnCommandSettingsDlg( WPARAM wParam )
{
    return FALSE;
}

void CTextEditor::OnScrollSettingsDlg( WPARAM wParam, LPARAM lParam )
{
}

LRESULT CTextEditor::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CTextEditor* textEditor;
    if( message == WM_NCCREATE ) {
        textEditor = static_cast<CTextEditor*>(
            reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( textEditor ) );

        textEditor->mainWindow = handle;

        return DefWindowProc( handle, message, wParam, lParam );
    }
   
    textEditor = reinterpret_cast<CTextEditor*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) {
        case WM_CREATE:
        {
            textEditor->OnCreate();
            return EXIT_SUCCESS;
        }
        case WM_SIZE:
        {
            textEditor->OnSize();
            return EXIT_SUCCESS;
        }
        case WM_COMMAND:
        {
            textEditor->OnCommand( wParam );
            return EXIT_SUCCESS;
        }
        case WM_CLOSE:
        {
            bool shouldClose = textEditor->OnClose();
            if( shouldClose ) {
                return DefWindowProc( handle, message, wParam, lParam );
            }

            return EXIT_SUCCESS;
        }
        case WM_DESTROY:
        {
            textEditor->OnDestroy();
            return EXIT_SUCCESS;
        }
        default:
        {
            return DefWindowProc( handle, message, wParam, lParam );
        }
    }
}

INT_PTR CTextEditor::settingsProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CTextEditor* textEditor;
    if( message == WM_INITDIALOG ) {
        textEditor = reinterpret_cast<CTextEditor*>( lParam );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( textEditor ) );

        textEditor->OnInitSettingsDlg(handle);

        return TRUE;
    }

    textEditor = reinterpret_cast<CTextEditor*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) {
        case WM_COMMAND:
        {
            return textEditor->OnCommandSettingsDlg( wParam );
        }
        case WM_HSCROLL:
        {
            textEditor->OnScrollSettingsDlg( wParam, lParam );
            return FALSE;
        }
        default:
        {
            return FALSE;
        }
    }
}

bool CTextEditor::saveInput() const
{
    int textLength = GetWindowTextLength( editControl );
    CString text( textLength + 1, TEXT( '\0' ) );

    GetWindowText( editControl, const_cast<PTCHAR>( text.data() ), text.length() );

    const int maxFileNameLength = 256;
    CString fileName( maxFileNameLength + 1, TEXT( '\0' ) );

    OPENFILENAME fileNameStruct; 
    ZeroMemory( &fileNameStruct, sizeof( OPENFILENAME ) );
    fileNameStruct.lStructSize = sizeof( OPENFILENAME );
    fileNameStruct.hwndOwner = mainWindow;
    fileNameStruct.lpstrFile = const_cast<PTCHAR>( fileName.data() );
    fileNameStruct.nMaxFile = maxFileNameLength;

    if( GetSaveFileName( &fileNameStruct ) != 0 ) {
        HANDLE file = CreateFile(
            fileName.c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr );

        WriteFile( file, text.c_str(), textLength * sizeof( TCHAR ), nullptr, nullptr );
        
        CloseHandle( file );
        return true;
    }

    return false;
}
