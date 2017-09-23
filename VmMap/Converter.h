#pragma once

#include <sstream>

#include "ListView.h"
#include "MemoryScanner.h"

using CStringStream = std::basic_stringstream<TCHAR>;

class CConverter {
public:
    CConverter() = default;

    CItem RegionInfoToItem( const CRegionInfo& blockInfo, CString details = {} );
    CItem AllocationInfoToItem( const CAllocationInfo& allocationInfo );

private:
    CStringStream stream;

    CString memSizeToString( long long memSize );

    CString memTypeToString( DWORD type ) const;
    CString memProtectionToString( DWORD protection ) const;
};