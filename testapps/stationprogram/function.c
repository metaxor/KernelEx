#include "main.h"

/*********************************************************\
* CreateProcessInDesktopW
*
* Create a process in a specified window station or
* desktop
*
\*********************************************************/
BOOL CreateProcessInDesktop(HWINSTA hWinSta,
                             HDESK hDesktop,
                             LPCTSTR lpApplicationName,
                             LPTSTR lpCommandLine,
                             LPSECURITY_ATTRIBUTES lpProcessAttributes,
                             LPSECURITY_ATTRIBUTES lpThreadAttributes,
                             BOOL bInheritHandles,
                             DWORD dwCreationFlags,
                             LPVOID lpEnvironment,
                             LPCTSTR lpCurrentDirectory,
                             LPSTARTUPINFO lpStartupInfo,
                             LPPROCESS_INFORMATION lpProcessInformation)
{
    LPTSTR WinStaName[255];
    DWORD WinStaNameLength = 255;
    LPTSTR DesktopName[255];
    DWORD DesktopNameLength = 255;
    TCHAR buf[255];

    if(!GetUserObjectInformation(hWinSta, UOI_NAME, WinStaName, 255, &WinStaNameLength) || hWinSta == NULL)
    {
        return FALSE;
    }

    if(!GetUserObjectInformation(hDesktop, UOI_NAME, DesktopName, 255, &DesktopNameLength) || hWinSta == NULL)
    {
        return FALSE;
    }

    wsprintf(buf, TEXT("%s\\%s"), WinStaName, DesktopName);
    lpStartupInfo->lpDesktop = buf;

    return CreateProcess(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwCreationFlags,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          lpProcessInformation);
}

/*********************************************************\
* ErrorCodeToString
*
* Transform an error code into a string error it will give
* us informations about the error
*
\*********************************************************/
BOOL ErrorCodeToString(DWORD dwErrorCode, LPVOID lpMsgBuf)
{
    return FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) lpMsgBuf,
        0, NULL);
}