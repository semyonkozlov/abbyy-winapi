#include <cassert>

#include <Tchar.h>
#include <Windows.h>

#include "TextEditor.h"

int WINAPI _tWinMain(
    HINSTANCE instance, HINSTANCE prevInstance,
    LPTSTR cmdLine, int cmdShow )
{
    bool registerClassStatus = CTextEditor::RegisterClass();
    assert( registerClassStatus != false );

    CTextEditor window;
    bool createStatus = window.Create();
    assert( createStatus != false );

    window.Show( cmdShow );

    BOOL getMessageStatus = FALSE;
    MSG message;
    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
        assert( getMessageStatus != -1 );
        
        if( !window.IsDialogMessage( &message ) ) {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    return EXIT_SUCCESS;
}
