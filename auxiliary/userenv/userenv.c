/*
 * Implementation of userenv.dll
 *
 * Copyright 2006 Mike McCormack for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <profinfo.h>


#ifdef _MSC_VER

#pragma warning(disable:4002)
#define TRACE()
#define FIXME()

#else

#define TRACE(x,args...) debug_output(__func__,x,args)
#define FIXME(x,args...) TRACE("FIXME: "x,args)

#endif 

//WINE_DEFAULT_DEBUG_CHANNEL( userenv );

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	TRACE("%p %d %p\n", hinstDLL, fdwReason, lpvReserved);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BOOL WINAPI CreateEnvironmentBlock( LPVOID* lpEnvironment,
                     HANDLE hToken, BOOL bInherit )
{
	FIXME("%p %p %d\n", lpEnvironment, hToken, bInherit );
	return FALSE;
}

BOOL WINAPI DestroyEnvironmentBlock( LPVOID lpEnvironment )
{
	FIXME("%p\n", lpEnvironment);
	return FALSE;
}

BOOL WINAPI ExpandEnvironmentStringsForUserA( HANDLE hToken, LPCSTR lpSrc,
                     LPSTR lpDest, DWORD dwSize )
{
	BOOL ret;

	TRACE("%p %s %p %d\n", hToken, debugstr_a(lpSrc), lpDest, dwSize);

	ret = ExpandEnvironmentStringsA( lpSrc, lpDest, dwSize );
	TRACE("<- %s\n", debugstr_a(lpDest));
	return ret;
}

BOOL WINAPI ExpandEnvironmentStringsForUserW( HANDLE hToken, LPCWSTR lpSrc,
                     LPWSTR lpDest, DWORD dwSize )
{
	BOOL ret;

	TRACE("%p %s %p %d\n", hToken, debugstr_w(lpSrc), lpDest, dwSize);

	ret = ExpandEnvironmentStringsW( lpSrc, lpDest, dwSize );
	TRACE("<- %s\n", debugstr_w(lpDest));
	return ret;
}

static int getsubdirpath(int nFolder, LPSTR lpBuffer, LPSTR lpSubDir)
{
	if ( FAILED(SHGetSpecialFolderPathA(NULL,lpBuffer,CSIDL_APPDATA,TRUE)) )
		return 0;
	PathAppendA(lpBuffer,lpSubDir);
	return lstrlenA(lpBuffer) + 1;
}

static int GetSpecialSubdirPathA(int nFolder, LPSTR lpSubDir, LPSTR lpDir, LPDWORD lpcchSize )
{
	char buffer[MAX_PATH];
	DWORD bufsize = getsubdirpath(nFolder,buffer,lpSubDir);
	if (!lpDir || !lpcchSize || !bufsize) return 0;
	if (*lpcchSize<bufsize)
	{
		*lpcchSize = bufsize;
		return 0;
	}
	*lpcchSize = bufsize;
	lstrcpyA(lpDir,buffer);
	return bufsize;	
}

static int GetSpecialSubdirPathW(int nFolder, LPSTR lpSubDir, LPWSTR lpDir, LPDWORD lpcchSize )
{
	char buffer[MAX_PATH];
	DWORD bufsize = getsubdirpath(nFolder,buffer,lpSubDir);
	if (!lpDir || !lpcchSize || !bufsize) return 0;
	bufsize = MultiByteToWideChar(CP_ACP,0,buffer,-1,NULL,0);
	if (*lpcchSize<bufsize)
	{
		*lpcchSize = bufsize;
		return 0;
	}
	*lpcchSize = bufsize;
	return MultiByteToWideChar(CP_ACP,0,buffer,-1,lpDir,bufsize);
}


int WINAPI GetUserProfileDirectoryA( HANDLE hToken, LPSTR lpProfileDir,
                     LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathA(CSIDL_APPDATA,"..",lpProfileDir,lpcchSize);
}

int WINAPI GetUserProfileDirectoryW( HANDLE hToken, LPWSTR lpProfileDir,
                     LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathW(CSIDL_APPDATA,"..",lpProfileDir,lpcchSize);
}

int WINAPI GetAllUsersProfileDirectoryA( LPSTR lpProfileDir, LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathA(CSIDL_COMMON_APPDATA,"..",lpProfileDir,lpcchSize);
}

int WINAPI GetAllUsersProfileDirectoryW( LPWSTR lpProfileDir, LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathW(CSIDL_COMMON_APPDATA,"..",lpProfileDir,lpcchSize);
}

int WINAPI GetProfilesDirectoryA( LPSTR lpProfilesDir, LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathA(CSIDL_APPDATA,"..\\..",lpProfilesDir,lpcchSize);
}

int WINAPI GetProfilesDirectoryW( LPWSTR lpProfilesDir, LPDWORD lpcchSize )
{
	return GetSpecialSubdirPathW(CSIDL_APPDATA,"..\\..",lpProfilesDir,lpcchSize);
}

BOOL WINAPI GetProfileType( DWORD *pdwFlags )
{
	FIXME("%p\n", pdwFlags );
	*pdwFlags = 0;
	return TRUE;
}

BOOL WINAPI LoadUserProfileA( HANDLE hToken, LPPROFILEINFOA lpProfileInfo )
{
	FIXME("%p %p\n", hToken, lpProfileInfo );
	lpProfileInfo->hProfile = HKEY_CURRENT_USER;
	return TRUE;
}

BOOL WINAPI LoadUserProfileW( HANDLE hToken, LPPROFILEINFOW lpProfileInfo )
{
	FIXME("%p %p\n", hToken, lpProfileInfo );
	lpProfileInfo->hProfile = HKEY_CURRENT_USER;
	return TRUE;
}

BOOL WINAPI RegisterGPNotification( HANDLE event, BOOL machine )
{
	FIXME("%p %d\n", event, machine );
	return TRUE;
}

BOOL WINAPI UnregisterGPNotification( HANDLE event )
{
	FIXME("%p\n", event );
	return TRUE;
}

BOOL WINAPI UnloadUserProfile( HANDLE hToken, HANDLE hProfile )
{
	FIXME("(%p, %p): stub\n", hToken, hProfile);
	return FALSE;
}
