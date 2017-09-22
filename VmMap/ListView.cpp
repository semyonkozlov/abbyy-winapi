#include <cassert>

#include <Windows.h>
#include <CommCtrl.h>

#include "Resource.h"
#include "ListView.h"

#pragma comment(lib, "ComCtl32.Lib")

CListView::CListView() : 
    listView( nullptr )
{
}

HWND CListView::Create( HWND parent )
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof( INITCOMMONCONTROLSEX );
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx( &icex );

    RECT rect;
    GetClientRect( parent, &rect );

    listView = CreateWindow(
        WC_LISTVIEW,
        nullptr,
        WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        parent,
        reinterpret_cast<HMENU>( IDC_LISTVIEW ),
        GetModuleHandle( nullptr ),  // TODO replace with const static variable
        this );
    assert( listView != nullptr );

    // TODO
    SetColumns( { 
        TEXT( "Address" ),
        TEXT( "Type" ),
        TEXT( "Size" ),
        TEXT( "Blocks" ),
        TEXT( "Protection" ),
        TEXT( "Details" )
    } );

    return listView;
}

void CListView::Show( int cmdShow ) const
{
    ShowWindow( listView, cmdShow );
}

void CListView::SetColumns( const CItem& columnTitles )
{
    RECT rect;
    GetClientRect( listView, &rect );

    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = (rect.right - rect.left) / columnTitles.size();
    lvc.cchTextMax = 256;

    for( int i = 0, index = -1; i < columnTitles.size(); ++i ) {
        lvc.pszText = const_cast<LPTSTR>( columnTitles[i].c_str() );
        index = ListView_InsertColumn( listView, i, &lvc );
        if( index == -1 ) {
            break;
        }
    }
}

void CListView::AddItem( const CItem& item, int index )
{
    if( index == -1 ) {
        index = ListView_GetItemCount( listView );
    }

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.cchTextMax = 256;
    lvi.iItem = index;
    lvi.pszText = const_cast<LPTSTR>( item[0].c_str() );
    lvi.iSubItem = 0;

    ListView_InsertItem( listView, &lvi );
    for( int i = 1; i < item.size(); ++i ) {
        ListView_SetItemText( listView, index, i, const_cast<LPTSTR>( item[i].c_str() ) );
    }
}

void CListView::SetItem( const CItem& item, int index )
{
    for( int i = 0; i < item.size(); ++i ) {
        ListView_SetItemText( listView, index, i, const_cast<LPTSTR>( item[i].c_str() ) );
    }
}

void CListView::DeleteAllItems()
{
    ListView_DeleteAllItems( listView );
}
