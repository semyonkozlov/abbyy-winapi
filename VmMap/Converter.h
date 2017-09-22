#pragma once

#include <sstream>

#include "ListView.h"
#include "MemoryScanner.h"

using CStringStream = std::basic_stringstream<TCHAR>;

class CConverter {
public:
    CConverter() = default;

    CItem RegionInfoToItem( const CRegionInfo& blockInfo, CString details = {} );
    CItem AllocationInfoToItem( const CAllocationInfo& allocationInfo, CString details = {} );

private:
    CStringStream stream;

    static CString memTypeToString( DWORD type );
    static CString memProtectionToString( DWORD protection );
};