#include <cassert>

#include <Windows.h>

#include "Resource.h"
#include "CSelectProcDialog.h"

CSelectProcDialog::CSelectProcDialog() :
    dialog( nullptr )
{
}

HWND CSelectProcDialog::Create( HWND parent )
{
    dialog = CreateDialogParam(
        GetModuleHandle( nullptr ),
        MAKEINTRESOURCE( IDD_DIALOG ),
        parent,
        dialogProc,
        reinterpret_cast<LPARAM>( this ) );
    assert( dialog != nullptr );
   
    return dialog;
}

void CSelectProcDialog::Show( int cmdShow ) const
{
    ShowWindow( dialog, cmdShow );
}

void CSelectProcDialog::OnInit( HWND handle )
{
}

INT_PTR CSelectProcDialog::dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    CSelectProcDialog* dialog = nullptr;
    if( message == WM_INITDIALOG ) {
        dialog = reinterpret_cast<CSelectProcDialog*>( lParam );
        SetWindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( dialog ) );

        dialog->OnInit( handle );

        return TRUE;
    }

    dialog = reinterpret_cast<CSelectProcDialog*>( GetWindowLongPtr( handle, GWLP_USERDATA ) );
    switch( message ) { // TODO
        default:
        {
            return FALSE;
        }
    }
}
