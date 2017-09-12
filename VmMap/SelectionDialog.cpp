#include <cassert>

#include <Windows.h>

#include "Resource.h"
#include "SelectionDialog.h"

CSelectionDialog::CSelectionDialog() :
    procsList(),
    dialog( nullptr ),
    list( nullptr )
{
}

HWND CSelectionDialog::Create( HWND parent )
{
    dialog = CreateDialogParam(
        GetModuleHandle( nullptr ),
        MAKEINTRESOURCE( IDD_DIALOG ),
        parent,
        dialogProc,
        reinterpret_cast<LPARAM>( this ) );
    //assert( dialog != nullptr ); TODO

    //list = procsList.Create( dialog );
   
    return dialog;
}

void CSelectionDialog::Show( int cmdShow ) const
{
    ShowWindow( dialog, cmdShow );
}

void CSelectionDialog::OnInit( HWND handle )
{
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
        default:
        {
            return FALSE;
        }
    }
}
