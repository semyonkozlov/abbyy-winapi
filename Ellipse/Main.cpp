#include <iostream>
#include <cassert>

#include <Windows.h>

#include "OverlappedWindow.h"

int WINAPI wWinMain( HINSTANCE instance,
    HINSTANCE prevInstance,
    PWSTR cmdLine,
    int cmdShow )
{
    bool registerClassStatus = COverlappedWindow::RegisterClass();
    assert( registerClassStatus != false );

    COverlappedWindow window;
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

    return EXIT_SUCCESS;
}