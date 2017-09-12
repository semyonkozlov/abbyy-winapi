#pragma once

#include "ListView.h"
#include "MemoryScanner.h"

class CItemConverter {
public:
    CItemConverter() = default;

    static CItem ConvertBlockInfoToItem( const CBlockInfo& blockInfo );
    static CItem ConvertAllocationInfoToItem( const CAllocationInfo& allocationInfo );
};