#pragma once

#include <sstream>

#include "ListView.h"
#include "MemoryScanner.h"

using CStringStream = std::basic_stringstream<TCHAR>;

class CConverter {
public:
    CConverter() = default;

    CItem RegionInfoToItem( const CRegionInfo& regionInfo, CString details = {} );
    CItem AllocationInfoToItem( const CAllocationInfo& allocationInfo );
    CItem ProcessInfoToItem( const CProcessInfo& processInfo );

private:
    CStringStream stream;

    CString memSizeToString( long long memSize );

    CString memTypeToString( DWORD type ) const;
    CString memProtectionToString( DWORD protection ) const;
};