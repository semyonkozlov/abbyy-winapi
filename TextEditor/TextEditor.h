#pragma once

#include <string>

#include <Windows.h>

class CTextEditor {
    using CString = std::basic_string<TCHAR>;

public:
    explicit CTextEditor( const CString& windowName = TEXT( "TextEditor" ) );

    static bool RegisterClass();

    bool Create();
    void Show( int cmdShow ) const;

    bool IsDialogMessage( LPMSG messagePtr ) const;

protected:
    void OnCreate();
    void OnSize();
    void OnCommand( WPARAM wParam );
    bool OnClose();
    void OnDestroy();
    HBRUSH OnCtlColorEdit( HDC deviceContext );

    void OnInitSettingsDlg( HWND handle );
    INT_PTR OnCommandSettingsDlg( WPARAM wParam );
    void OnScrollSettingsDlg( LPARAM lParam );
    
private:
    static const CString className;

    static LRESULT CALLBACK windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    static INT_PTR CALLBACK settingsProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );

    CString windowName;

    HWND mainWindow;
    HWND editControl;
    HWND settingsDialog;

    bool hasInput;
    bool shouldPreview;

    struct CSettings {
        int fontSize;
        DWORD fontColor;
        DWORD backgroundColor;
        BYTE opacity;
    } currentSettings, backupSettings;

    HFONT font;
    HBRUSH bgBrush;

    bool saveInput() const;
    void chooseColor( DWORD* colorPtr );
    void updateWindow();
};