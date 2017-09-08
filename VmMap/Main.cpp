#include <cassert>

#include <Tchar.h>
#include <Windows.h>

#include "VmMapWindow.h"
#include "Utils.h"

int WINAPI _tWinMain( HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow )
{
    CVmMapWindow::RegisterClass();

    CVmMapWindow vmmap;
    vmmap.Create();

    vmmap.Show( cmdShow );

    BOOL getMessageStatus = FALSE;
    MSG message;
    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
        assert( getMessageStatus != -1 );

        // TODO: accelerator support
        if( !vmmap.IsDialogMessage( &message ) ) {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    return EXIT_SUCCESS;
}