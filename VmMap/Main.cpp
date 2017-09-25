#include <cassert>

#include <Tchar.h>
#include <Windows.h>

#include "Resource.h"
#include "VmMapWindow.h"

int WINAPI _tWinMain( HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow )
{
    CVmMapWindow::RegisterClass();

    CVmMapWindow vmmap;
    HWND vmmapWindow = vmmap.Create();

    vmmap.Show( cmdShow );

    HACCEL accelTable = LoadAccelerators( instance, MAKEINTRESOURCE( IDR_ACCEL ) );
    assert( accelTable != nullptr );

    BOOL getMessageStatus = FALSE;
    MSG message;
    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
        assert( getMessageStatus != -1 );

        if( !TranslateAccelerator( vmmapWindow, accelTable, &message ) && 
            !IsDialogMessage( vmmapWindow, &message ) ) 
        {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    return EXIT_SUCCESS;
}