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

    ListView_SetExtendedListViewStyleEx( listView, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );

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

    LVCOLUMN listViewColumn;
    listViewColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    listViewColumn.cx = (rect.right - rect.left) / columnTitles.size();
    listViewColumn.cchTextMax = 256;

    for( int i = 0, index = -1; i < columnTitles.size(); ++i ) {
        listViewColumn.pszText = const_cast<LPTSTR>( columnTitles[i].c_str() );
        index = ListView_InsertColumn( listView, i, &listViewColumn );
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

    LVITEM listViewItem;
    listViewItem.mask = LVIF_TEXT;
    listViewItem.cchTextMax = 256;
    listViewItem.iItem = index;
    listViewItem.pszText = const_cast<LPTSTR>( item[0].c_str() );
    listViewItem.iSubItem = 0;

    ListView_InsertItem( listView, &listViewItem );
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

CString CListView::GetItemText( int index, int subitemIndex )
{
    TCHAR subitemText[subitemTextMaxSize + 1];

    ListView_GetItemText( listView, index, subitemIndex, subitemText, subitemTextMaxSize );

    return subitemText;
}

void CListView::DeleteItem( int index )
{
    ListView_DeleteItem( listView, index );
}

void CListView::DeleteAllItems()
{
    ListView_DeleteAllItems( listView );
}

int CListView::GetItemCount() const
{
    return ListView_GetItemCount( listView );
}
