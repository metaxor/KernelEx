#include <windows.h>
#include "init.h"

void WinMainCRTStartup(void)
{
    int mainret;
    char* lpszCommandLine;
    STARTUPINFO StartupInfo;

    lpszCommandLine = GetCommandLine();

    // Skip past program name (first token in command line).

    if ( *lpszCommandLine == '"' )  // Check for and handle quoted program name
    {
		lpszCommandLine++;	// Get past the first quote

        // Now, scan, and skip over, subsequent characters until  another
        // double-quote or a null is encountered
        while( *lpszCommandLine && (*lpszCommandLine != '"') )
            lpszCommandLine++;

        // If we stopped on a double-quote (usual case), skip over it.

        if ( *lpszCommandLine == '"' )
            lpszCommandLine++;
    }
    else    // First token wasn't a quote
    {
        while ( *lpszCommandLine > ' ' )
            lpszCommandLine++;
    }

    // Skip past any white space preceeding the second token.

    while ( *lpszCommandLine && (*lpszCommandLine <= ' ') )
        lpszCommandLine++;

    StartupInfo.dwFlags = 0;
    GetStartupInfo( &StartupInfo );

	__init();

    mainret = WinMain( GetModuleHandle(NULL),
                       NULL,
                       lpszCommandLine,
                       StartupInfo.dwFlags & STARTF_USESHOWWINDOW
                            ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );

    __exit();

    ExitProcess(mainret);
}
