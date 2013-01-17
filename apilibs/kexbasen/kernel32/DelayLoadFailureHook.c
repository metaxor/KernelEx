/*
 *  KernelEx
 *
 *  Copyright (C) 2009, Tihiy
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

#include "common.h"

typedef struct
{
	DWORD	unknown[3];
	LPSTR	dllName;
	DWORD	unknown2;
	LPSTR	procName;
} SHLWAPI_DELAYLOAD, *PSHLWAPI_DELAYLOAD;

/*
 *	shlwapi has half of whole Windows API reimplemented
 *	and DelayLoadFailureHook in some retarded wrapper export
 */

static HMODULE hShlwapi;
typedef FARPROC (WINAPI* DLFH) (DWORD, PSHLWAPI_DELAYLOAD);

/* MAKE_EXPORT DelayLoadFailureHook_new=DelayLoadFailureHook */
FARPROC WINAPI DelayLoadFailureHook_new(
	LPSTR pszDllName,
	LPSTR pszProcName
)
{
	SHLWAPI_DELAYLOAD param;

	if (!hShlwapi)
	{
		hShlwapi = GetModuleHandle("shlwapi.dll");
		if (!hShlwapi) 
			hShlwapi = LoadLibrary("shlwapi.dll");
	}

	DLFH ShlwapiDelayLoad = (DLFH) kexGetProcAddress(hShlwapi,"DelayLoadFailureHook");
	if (!ShlwapiDelayLoad) 
		return NULL;

	param.dllName = pszDllName;
	param.procName = pszProcName;
	
	return ShlwapiDelayLoad(4,&param);
}
