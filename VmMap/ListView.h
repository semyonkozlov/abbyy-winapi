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
    void AddItem( const CItem& item );

    void DeleteAllItems();

private:
    HWND listView;
};