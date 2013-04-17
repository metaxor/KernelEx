#include "common.h"
#include <ntsecapi.h>
#include <accctrl.h>
#include "_advapi32_apilist.h"

#ifndef SID_MAX_SUB_AUTHORITIES
#define	SID_MAX_SUB_AUTHORITIES		(15)	/* current max subauths */
#endif

DWORD __inline APIENTRY
WNetGetUserA(
     IN LPCSTR lpName,
     IN LPSTR lpUserName,
     OUT LPDWORD lpnLength
    )
{
	HINSTANCE hInstance = NULL;
	FARPROC Address = NULL;
	BOOL Result = FALSE;

	hInstance = LoadLibrary("MPR.DLL");

	if(hInstance == NULL)
		return FALSE;

	Address = kexGetProcAddress(hInstance, "WNetGetUserA");

	if(Address == NULL)
	{
		FreeLibrary(hInstance);
		return FALSE;
	}

	__asm	push lpName
	__asm	push lpUserName
	__asm	push lpnLength
	__asm	call Address
	__asm	mov [Result], eax

	FreeLibrary(hInstance);

	return Result;
}
DWORD __inline APIENTRY
WNetGetUserW(
     IN LPCWSTR lpName,
     IN LPWSTR lpUserName,
     OUT LPDWORD lpnLength
    )
{
	HINSTANCE hInstance = NULL;
	FARPROC Address = NULL;
	BOOL Result = FALSE;

	hInstance = LoadLibrary("MPR.DLL");

	if(hInstance == NULL)
		return FALSE;

	Address = kexGetProcAddress(hInstance, "WNetGetUserW");

	if(Address == NULL)
	{
		FreeLibrary(hInstance);
		return FALSE;
	}

	__asm	push lpName
	__asm	push lpUserName
	__asm	push lpnLength
	__asm	call Address
	__asm	mov [Result], eax

	FreeLibrary(hInstance);

	return Result;
}
BOOL __inline APIENTRY
WNetVerifyPasswordA(
     IN LPCSTR lpszPassword,
     OUT LPBOOL pfMatch
    )
{
	HINSTANCE hInstance = NULL;
	FARPROC Address = NULL;
	BOOL Result = FALSE;

	hInstance = LoadLibrary("MPR.DLL");

	if(hInstance == NULL)
		return FALSE;

	Address = kexGetProcAddress(hInstance, "WNetVerifyPasswordA");

	if(Address == NULL)
	{
		FreeLibrary(hInstance);
		return FALSE;
	}

	__asm	push [pfMatch]
	__asm	push [lpszPassword]
	__asm	call Address
	__asm	mov [Result], eax

	FreeLibrary(hInstance);

	return Result;
}
BOOL __inline APIENTRY
WNetVerifyPasswordW(
     IN LPCWSTR lpszPassword,
     OUT LPBOOL pfMatch
    )
{
	HINSTANCE hInstance = NULL;
	FARPROC Address = NULL;
	BOOL Result = FALSE;

	hInstance = LoadLibrary("MPR.DLL");

	if(hInstance == NULL)
		return FALSE;

	Address = kexGetProcAddress(hInstance, "WNetVerifyPasswordW");

	if(Address == NULL)
	{
		FreeLibrary(hInstance);
		return FALSE;
	}

	__asm	push [pfMatch]
	__asm	push [lpszPassword]
	__asm	call Address
	__asm	mov [Result], eax

	FreeLibrary(hInstance);

	return Result;
}

BOOL WINAPI IntSwitchUser(LPCSTR lpszUsername, BOOL fWait, DWORD dwMaxMilliseconds, BOOL fAllowReload);

#ifdef UNICODE
#define WNetVerifyPassword  WNetVerifyPasswordW
#else
#define WNetVerifyPassword  WNetVerifyPasswordA
#endif