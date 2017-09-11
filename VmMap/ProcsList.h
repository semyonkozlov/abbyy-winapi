#pragma once

#include <vector>
#include <string>

#include <Windows.h>

#include "Utils.h"

class CProcsList {
public:
    CProcsList();

    HWND Create( HWND parent );
    void Show( int cmdShow ) const;

    void SetColumns( const std::vector<CString>& columnTitles );
    void AddItem( const std::vector<CString>& item );

private:
    HWND listView;
};