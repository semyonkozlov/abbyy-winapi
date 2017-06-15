#include <iostream>
#include <cassert>

#include <tchar.h>
#include <Windows.h>

#include "EllipseWindow.h"
#include "OverlappedWindow.h"
#include "Resource.h"

int WINAPI _tWinMain( HINSTANCE instance,
    HINSTANCE prevInstance,
    LPTSTR cmdLine,
    int cmdShow )
{
    /*bool registerClassStatus = CEllipseWindow::RegisterClass();
    assert( registerClassStatus != false );

    CEllipseWindow window;
    bool createStatus = window.Create();
    assert( createStatus != false );

    window.Show( cmdShow );

    BOOL getMessageStatus = FALSE;
    MSG message;
    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
        assert( getMessageStatus != -1 );

        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    return EXIT_SUCCESS;*/

    bool registerClassStatus = COverlappedWindow::RegisterClass();
    assert( registerClassStatus != false );

    COverlappedWindow window;
    bool createStatus = window.Create();
    assert( createStatus != false );

    window.Show( cmdShow );

    HACCEL accelTable = LoadAccelerators( instance, MAKEINTRESOURCE( IDR_ACCEL ) );
    assert( accelTable != nullptr );

    BOOL getMessageStatus = FALSE;
    MSG message;
    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
        assert( getMessageStatus != -1 );

        if( !TranslateAccelerator( window.GetHandle(), accelTable, &message ) ) {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    return EXIT_SUCCESS;
}
