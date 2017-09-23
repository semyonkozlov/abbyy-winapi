#include <cassert>

#include <Windows.h>

#include "Resource.h"
#include "SelectionDialog.h"

CSelectionDialog::CSelectionDialog() :
    procsList(),
    dialogWindow( nullptr ),
    listWindow( nullptr )
{
}

HWND CSelectionDialog::Create( HWND parent )
{
    dialogWindow = CreateDialogParam(
        GetModuleHandle( nullptr ),
        MAKEINTRESOURCE( IDD_DIALOG ),
        parent,
        dialogProc,
        reinterpret_cast<LPARAM>( this ) );
    assert( dialogWindow != nullptr ); 

    listWindow = procsList.Create( dialogWindow );
   
    return dialogWindow;
}

void CSelectionDialog::Show( int cmdShow ) const
{
    ShowWindow( dialogWindow, cmdShow );
}

void CSelectionDialog::OnInit( HWND handle )
{
}

void CSelectionDialog::OnClose()
{
    EndDialog( dialogWindow, EXIT_SUCCESS );
}

INT_PTR CSelectionDialog::OnCommand( WPARAM wParam )
{
    switch( LOWORD( wParam ) ) {
        case ID_PUSHBUTTON_OK:
        {
            OnCmdPushbuttonOk();
            return TRUE;
        }
        case ID_PUSHBUTTON_CANCEL:
        {
            OnCmdPushbuttonCancel();
            return TRUE;
        }
        default:
        {
            MessageBox( dialogWindow, TEXT( "What?" ), nullptr, MB_OK );
        }
    }

    return FALSE;
}

void CSelectionDialog::OnCmdPushbuttonOk()
{
    OnClose();
}

void CSelectionDialog::OnCmdPushbuttonCancel()
{
    OnClose();
}

INT_PTR CSelectionDialog::dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CSelectionDialog* dialog = nullptr;
    if( message == WM_INITDIALOG ) {
        dialog = reinterpret_cast<CSelectionDialog*>( lParam );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( dialog ) );

        dialog->OnInit( handle );

        return TRUE;
    }

    dialog = reinterpret_cast<CSelectionDialog*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) { // TODO
        case WM_COMMAND:
        {
            return dialog->OnCommand( wParam );
        }
        case WM_CLOSE:
        {
            dialog->OnClose();
            return TRUE;
        }
        default:
        {
            //MessageBox( dialog->dialogWindow, TEXT( "What?" ), nullptr, MB_OK );
        }
    }

    return FALSE;
}
