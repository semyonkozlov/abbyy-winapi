#pragma once

#include "ListView.h"
#include "MemoryScanner.h"

class CConverter {
public:
    CConverter() = default;

    static CItem RegionInfoToItem( const CRegionInfo& blockInfo );
//    static CItem AllocationInfoToItem( const CAllocationInfo& allocationInfo );

    static CString MemTypeToString( DWORD type );
    static CString MemProtectionToString( DWORD protection );
};