#include <sstream>
#include <iomanip>

#include "Converter.h"

CItem CConverter::BlockInfoToItem( const CBlockInfo& blockInfo )
{
    CStringStream stream;

    stream << std::hex << blockInfo.AllocationBase;
    CString address = CString( TEXT( "    " ) ) + stream.str();
    stream.str( {} );

    CString type = MemTypeToString( blockInfo.State == MEM_COMMIT ? blockInfo.Type : blockInfo.State );

    stream << std::dec << blockInfo.RegionSize;
    CString size = stream.str();

    CString blocks{};

    CString protection = MemProtectionToString( blockInfo.Protect );

    CString details{};

    return { address, type, size, blocks, protection, details };
}

CItem CConverter::AllocationInfoToItem( const CAllocationInfo& allocationInfo )
{
    CStringStream stream;

    stream << std::hex << allocationInfo.AllocationBaseAddress;
    CString address = stream.str();
    stream.str( {} );

    CString type = MemTypeToString( allocationInfo.AllocationType );

    stream << std::dec << allocationInfo.AllocationSize;
    CString size = stream.str();
    stream.str( {} );

    stream  << allocationInfo.NumBlocks;
    CString blocks = stream.str();

    CString protection = MemProtectionToString( allocationInfo.AllocationProtection );

    CString details{};

    return { address, type, size, blocks, protection, details };
}

CString CConverter::MemTypeToString( DWORD type )
{
    CString memTypeString;

    switch( type ) {
        case MEM_FREE:
            memTypeString = TEXT( "Free" );
            break;
        case MEM_RESERVE:
            memTypeString = TEXT( "Reserve" );
            break;
        case MEM_IMAGE:
            memTypeString = TEXT( "Image" );
            break;
        case MEM_MAPPED:
            memTypeString = TEXT( "Mapped" );
            break;
        case MEM_PRIVATE:
            memTypeString = TEXT( "Private" );
            break;
        default:
            memTypeString = TEXT( "Unknown" );
    }

    return memTypeString;
}

CString CConverter::MemProtectionToString( DWORD protection )
{
    CString protectionString;

    switch( protection & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE) ) {
        case PAGE_READONLY: 
            protectionString = TEXT( "-R--" ); 
            break;
        case PAGE_READWRITE: 
            protectionString = TEXT( "-RW-" ); 
            break;
        case PAGE_WRITECOPY:
            protectionString = TEXT( "-RWC" );
            break;
        case PAGE_EXECUTE: 
            protectionString = TEXT( "E---" );
            break;
        case PAGE_EXECUTE_READ: 
            protectionString = TEXT( "ER--" );
            break;
        case PAGE_EXECUTE_READWRITE:
            protectionString = TEXT( "ERW-" ); 
            break;
        case PAGE_EXECUTE_WRITECOPY:
            protectionString = TEXT( "ERWC" );
            break;
        case PAGE_NOACCESS:
            protectionString = TEXT( "----" ); 
            break;
        default:
            protectionString = TEXT( "Unknown" );
    }

    // TODO : flags?

    return protectionString;
}
