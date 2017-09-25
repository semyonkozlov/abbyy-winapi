#pragma once

#include <vector>
#include <string>

#include <Windows.h>

#include "Utils.h"

using CItem = std::vector<CString>;

class CListView {
public:
    CListView();

    HWND Create( HWND parent );
    void Show( int cmdShow ) const;

    void SetColumns( const CItem& columnTitles );

    void AddItem( const CItem& item, int index = -1 );
    void SetItem( const CItem& item, int index );

    CItem GetItem( int index ) const;
    CString GetItemText( int index, int subitemIndex ) const;

    void DeleteItem( int index );
    void DeleteAllItems();

    int GetSelectedItemIndex() const;
    int GetItemCount() const;

private:
    HWND listView;

    static const int subitemTextMaxSize = 1024;
};