/*
 *  KernelEx
 *  Copyright (C) 2013, Ley0k
 *
 *  This file is part of KernelEx source code.
 *
 *  KernelEx is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; version 2 of the License.
 *
 *  KernelEx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

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