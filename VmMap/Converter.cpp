#include <sstream>
#include <iomanip>

#include "Converter.h"

CItem CConverter::RegionInfoToItem( const CRegionInfo& regionInfo, CString details )
{
    stream << std::hex << regionInfo.BaseAddress;
    CString address = CString( TEXT( "      " ) ) + stream.str();
    stream.str( {} );

    CString type = memTypeToString( regionInfo.State == MEM_COMMIT ? regionInfo.Type : regionInfo.State );

    CString size = memSizeToString( regionInfo.RegionSize );

    CString blocks{};

    CString protection = memProtectionToString( regionInfo.Protect );

    return { address, type, size, blocks, protection, details };
}

CItem CConverter::AllocationInfoToItem( const CAllocationInfo& allocationInfo, CString details )
{
    stream << std::hex << allocationInfo.AllocationBaseAddress;
    CString address = stream.str();
    stream.str( {} );

    CString type = memTypeToString( allocationInfo.AllocationType );

    CString size = memSizeToString( allocationInfo.AllocationSize );

    if( allocationInfo.AllocationType != MEM_FREE ) {
        stream << allocationInfo.NumBlocks;
    }
    CString blocks = stream.str();

    CString protection = memProtectionToString( allocationInfo.AllocationProtection );

    return { address, type, size, blocks, protection, details };
}

CString CConverter::memSizeToString( long long memSize )
{
    auto defaultLocale = stream.getloc();
    stream.imbue( std::locale( "" ) );

    stream << std::dec << (memSize >> 10) << TEXT( " K" );
    CString memSizeString = stream.str();

    stream.imbue( defaultLocale );
    stream.str( {} );

    return memSizeString;
}

CString CConverter::memTypeToString( DWORD type ) const
{
    CString memTypeString;

    switch( type ) {
        case MEM_FREE:
            memTypeString = TEXT( "Free" );
            break;
        case MEM_RESERVE:
            memTypeString = TEXT( "Reserved" );
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

CString CConverter::memProtectionToString( DWORD protection ) const
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
            protectionString = TEXT( "" );
    }

    // TODO : flags?

    return protectionString;
}
