#include <cassert>

#include <Windows.h>
#include <WinUser.h>
#include <CommCtrl.h>

#include "Resource.h"
#include "TextEditor.h"

const CTextEditor::CString CTextEditor::className = TEXT( "TEXTEDITOR" );

CTextEditor::CTextEditor( const CString& windowName ) :
    windowName( windowName ),
    mainWindow( nullptr ),
    editControl( nullptr ),
    settingsDialog( nullptr ),
    hasInput( false ),
    shouldPreview( false ), 
    font( nullptr ),
    bgBrush( nullptr )
{
    ZeroMemory( &currentSettings, sizeof( CSettings ) );
    ZeroMemory( &backupSettings, sizeof( CSettings ) );
}

bool CTextEditor::RegisterClass()
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

    return ::RegisterClass( &windowClass );
}

bool CTextEditor::Create()
{
    mainWindow = CreateWindowEx(
        WS_EX_LAYERED,
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
    SetLayeredWindowAttributes( mainWindow, 0, currentSettings.opacity, LWA_ALPHA );
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
        reinterpret_cast<LPARAM>( this ) );
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
    currentSettings.fontSize = 14;
    currentSettings.fontColor = RGB( 0, 0, 0 );
    currentSettings.backgroundColor = RGB( 255, 255, 255 );
    currentSettings.opacity = 255;

    backupSettings = currentSettings;

    font = CreateFont(
        currentSettings.fontSize, 0, 0, 0,                        
        FW_DONTCARE,               
        FALSE,                     
        FALSE,                     
        FALSE,                     
        ANSI_CHARSET,              
        OUT_DEFAULT_PRECIS,        
        CLIP_DEFAULT_PRECIS,       
        DEFAULT_QUALITY,           
        FIXED_PITCH,               
        TEXT( "Lucida Console" )   
    );
    assert( font != nullptr );
    
    bgBrush = CreateSolidBrush( currentSettings.backgroundColor );
    assert( bgBrush != nullptr );
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
            backupSettings = currentSettings;
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
    DeleteObject( bgBrush );
    DeleteObject( font );

    PostQuitMessage( EXIT_SUCCESS );
}

HBRUSH CTextEditor::OnCtlColorEdit( HDC deviceContext )
{
    SetTextColor( deviceContext, currentSettings.fontColor );
    SetBkColor( deviceContext, currentSettings.backgroundColor );

    DeleteObject( bgBrush );
    bgBrush = CreateSolidBrush( currentSettings.backgroundColor );

    return bgBrush;
}

void CTextEditor::OnInitSettingsDlg( HWND handle )
{
    SendMessage( GetDlgItem( handle, IDC_SLIDER_FONTSIZE ), TBM_SETRANGE, TRUE, MAKELONG( 8, 72 ) );
    SendMessage( GetDlgItem( handle, IDC_SLIDER_OPACITY ), TBM_SETRANGE, TRUE, MAKELONG( 0, 255 ) );

    SendMessage( GetDlgItem( handle, IDC_SLIDER_FONTSIZE ), TBM_SETPOS, TRUE, currentSettings.fontSize );
    SendMessage( GetDlgItem( handle, IDC_SLIDER_OPACITY ), TBM_SETPOS, TRUE, currentSettings.opacity );

    SendMessage( editControl, WM_SETFONT, reinterpret_cast<WPARAM>( font ), TRUE );
}

INT_PTR CTextEditor::OnCommandSettingsDlg( WPARAM wParam )
{
    switch( LOWORD( wParam ) ) {
        case ID_PUSHBUTTON_FONT:
        {
            chooseColor( &currentSettings.fontColor );
            break;
        }
        case ID_PUSHBUTTON_BG:
        {
            chooseColor( &currentSettings.backgroundColor );
            break; 
        }
        case IDC_CHECKBOX_PREVIEW:
        {
            shouldPreview = IsDlgButtonChecked( settingsDialog, IDC_CHECKBOX_PREVIEW ) == BST_CHECKED;
            break;
        }
        case ID_PUSHBUTTON_OK:
        {
            updateWindow();
            EndDialog( settingsDialog, EXIT_SUCCESS );
            return TRUE;
        }
        case ID_PUSHBUTTON_CANCEL:
        {
            currentSettings = backupSettings;
            updateWindow();
            EndDialog( settingsDialog, EXIT_SUCCESS );
            return TRUE;
        }
    }

    if( shouldPreview ) {
        updateWindow();
    }
    return FALSE;
}

void CTextEditor::OnScrollSettingsDlg( LPARAM lParam )
{
    auto scrollBarControl = reinterpret_cast<HWND>( lParam );
    if( scrollBarControl == GetDlgItem( settingsDialog, IDC_SLIDER_FONTSIZE ) ) {
        currentSettings.fontSize = SendMessage( scrollBarControl, TBM_GETPOS, 0, 0 );
    }
    else if ( scrollBarControl == GetDlgItem( settingsDialog, IDC_SLIDER_OPACITY ) ) {
        currentSettings.opacity = SendMessage( scrollBarControl, TBM_GETPOS, 0, 0 );
    }

    if( shouldPreview ) {
        updateWindow();
    }
}

LRESULT CTextEditor::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CTextEditor* textEditor = nullptr;
    if( message == WM_NCCREATE ) {
        textEditor = static_cast<CTextEditor*>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( textEditor ) );

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
        case WM_CTLCOLOREDIT:
        {
            return reinterpret_cast<LRESULT>( textEditor->OnCtlColorEdit( reinterpret_cast<HDC>( wParam ) ) );
        }
        default:
        {
            return DefWindowProc( handle, message, wParam, lParam );
        }
    }
}

INT_PTR CTextEditor::settingsProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CTextEditor* textEditor = nullptr;
    if( message == WM_INITDIALOG ) {
        textEditor = reinterpret_cast<CTextEditor*>( lParam );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( textEditor ) );

        textEditor->OnInitSettingsDlg( handle );

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
            textEditor->OnScrollSettingsDlg( lParam );
            return FALSE;
        }
        case WM_CLOSE:
        {
            return textEditor->OnCommandSettingsDlg( ID_PUSHBUTTON_CANCEL );
        }
        default:
        {
            return FALSE;
        }
    }
}

void CTextEditor::chooseColor( DWORD* colorPtr )
{
    CHOOSECOLOR chooseColorStruct;
    ZeroMemory( &chooseColorStruct, sizeof( CHOOSECOLOR ) );
    COLORREF palitre[16];
    ZeroMemory( palitre, 16 * sizeof( COLORREF ) );

    chooseColorStruct.lStructSize = sizeof( CHOOSECOLOR );
    chooseColorStruct.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
    chooseColorStruct.lpCustColors = palitre;
    chooseColorStruct.rgbResult = *colorPtr;

    ChooseColor( &chooseColorStruct );
    *colorPtr = chooseColorStruct.rgbResult;
}

void CTextEditor::updateWindow()
{
    LOGFONT fontStruct;
    GetObject( font, sizeof( LOGFONT ), &fontStruct );
    fontStruct.lfHeight = currentSettings.fontSize;

    DeleteObject( font );
    font = CreateFontIndirect( &fontStruct );
    SendMessage( editControl, WM_SETFONT, reinterpret_cast<WPARAM>( font ), TRUE );

    SetLayeredWindowAttributes( mainWindow, 0, currentSettings.opacity, LWA_ALPHA );

    SendMessage( GetDlgItem( settingsDialog, IDC_SLIDER_FONTSIZE ), TBM_SETPOS, TRUE, currentSettings.fontSize );
    SendMessage( GetDlgItem( settingsDialog, IDC_SLIDER_OPACITY ), TBM_SETPOS, TRUE, currentSettings.opacity );

    UpdateWindow( editControl );
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
