#include <iostream>
#include <cassert>

#include <Tchar.h>
#include <Windows.h>
#include <Windowsx.h>

#include "Utils.h"
#include "Resource.h"


LRESULT WinProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message ) {
        case WM_CREATE:
        {
            
            return EXIT_SUCCESS;
        }
        case WM_DESTROY:
        {
            PostQuitMessage( EXIT_SUCCESS );
            return EXIT_SUCCESS;
        }
        default:
            return DefWindowProc( handle, message, wParam, lParam );
    }
}

int WINAPI _tWinMain( 
    HINSTANCE instance, HINSTANCE prevInstance,
    LPTSTR cmdLine, int cmdShow )
{
//    bool registerClassStatus = CVmmapWindow::RegisterClass();
//    assert( registerClassStatus != false );
//
//    CVmmapWindow window;
//    bool createStatus = window.Create();
//    assert( createStatus != false );
//
//    window.Show( cmdShow );
//
//  /*  HACCEL accelTable = LoadAccelerators( instance, MAKEINTRESOURCE(  ) );
//    assert( accelTable != nullptr );
//TODO */
//
//    BOOL getMessageStatus = FALSE;
//    MSG message;
//    while( (getMessageStatus = GetMessage( &message, nullptr, 0, 0 )) != 0 ) {
//        assert( getMessageStatus != -1 );
//
//       // if( !TranslateAccelerator( window.GetHandle(), accelTable, &message ) ) {
//            TranslateMessage( &message );
//            DispatchMessage( &message );
//       // }
//    }
//
    DialogBox( instance, MAKEINTRESOURCE( IDD_VMMAP ), NULL, WinProc );
    return EXIT_SUCCESS;
}