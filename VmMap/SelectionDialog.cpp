#include <cassert>

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#include "Resource.h"
#include "SelectionDialog.h"

CSelectionDialog::CSelectionDialog() :
    toolhelp(),
    converter(),
    procsList(),
    processId( -1 ),
    dialogWindow( nullptr ),
    listWindow( nullptr )
{
}

INT_PTR CSelectionDialog::CreateDialogBox( HWND parent )
{
    return DialogBoxParam(
        GetModuleHandle( nullptr ),
        MAKEINTRESOURCE( IDD_DIALOG ),
        parent,
        dialogProc,
        reinterpret_cast<LPARAM>( this ) );
}

void CSelectionDialog::OnInit( HWND handle )
{
    dialogWindow = handle;

    listWindow = procsList.Create( dialogWindow );
    RECT rect;
    GetClientRect( dialogWindow, &rect );

    SetWindowPos(
        listWindow,
        HWND_TOP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top - 55,
        0 );

    procsList.SetColumns( {
        TEXT( "Name" ),
        TEXT( "PID" ),
        TEXT( "Working Set" ),
        TEXT( "Architecture" )
    } );

    toolhelp.CreateSnapshot( 0 );

    //ShowWindow( listWindow, SW_SHOW );
    OnCmdPushbuttonRefresh();
}

void CSelectionDialog::OnNotify( LPARAM lParam )
{
    auto notificationMessage = reinterpret_cast<LPNMHDR>(lParam);
    if( notificationMessage->idFrom == IDC_LISTVIEW && notificationMessage->code == NM_DBLCLK ) {
        int itemIndex = reinterpret_cast<LPNMLISTVIEW>(lParam)->iItem;

        CString pidText = procsList.GetItemText( itemIndex, PLC_Pid );
        EndDialog( dialogWindow, std::stoi( pidText ) );
    }
}

void CSelectionDialog::OnClose()
{
    toolhelp.DestroySnapshot();

    EndDialog( dialogWindow, -1 );
}

INT_PTR CSelectionDialog::OnCommand( WPARAM wParam )
{
    switch( LOWORD( wParam ) ) {
        case ID_PUSHBUTTON_OK:
            OnCmdPushbuttonOk();
            return TRUE;

        case ID_PUSHBUTTON_CANCEL:
            OnCmdPushbuttonCancel();
            return TRUE;

        case ID_PUSHBUTTON_REFRESH:
            OnCmdPushbuttonRefresh();
            return TRUE;

        default:
            MessageBox( dialogWindow, TEXT( "What?" ), nullptr, MB_OK );
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

void CSelectionDialog::OnCmdPushbuttonRefresh()
{
    SetWindowRedraw( listWindow, FALSE );

    procsList.DeleteAllItems();

    std::vector<CProcessInfo> processInfoList;
    toolhelp.GetProcessList( &processInfoList );

    for( auto&& processInfo : processInfoList ) {
        procsList.AddItem( converter.ProcessInfoToItem( processInfo ) );
    }

    SetWindowRedraw( listWindow, TRUE );
    procsList.Show( SW_SHOW );
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
            return dialog->OnCommand( wParam );

        case WM_NOTIFY:
            dialog->OnNotify( lParam );
            return TRUE;

        case WM_CLOSE:
            dialog->OnClose();
            return TRUE;


        default:
        {
            //MessageBox( dialog->dialogWindow, TEXT( "What?" ), nullptr, MB_OK );
        }
    }

    return FALSE;
}
