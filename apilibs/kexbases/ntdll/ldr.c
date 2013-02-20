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

#include "_ntdll_apilist.h"
#include <stdio.h>

/* MAKE_EXPORT LdrLoadDll=LdrLoadDll */
NTSTATUS NTAPI LdrLoadDll(IN PWCHAR PathToFile OPTIONAL,
	IN ULONG Flags OPTIONAL,
	IN PUNICODE_STRING ModuleFileName,
	OUT PHANDLE ModuleHandle
)
{
	PCHAR lpFileName;
	char pathfile[255];

	STACK_WtoA(ModuleFileName->Buffer, lpFileName);

	if(PathToFile != NULL)
	{
		PCHAR PathToFileA;

		STACK_WtoA(PathToFile, PathToFileA);
		sprintf(pathfile, "%s\\%s", PathToFileA, lpFileName);
	}

	SetLastError(0);

	*ModuleHandle = LoadLibraryEx(PathToFile == NULL ? lpFileName : pathfile, NULL, Flags);

	return GetLastError();
}

/* MAKE_EXPORT LdrUnloadDll=LdrUnloadDll */
NTSTATUS NTAPI LdrUnloadDll(IN PHANDLE ModuleHandle)
{
	SetLastError(0);

	FreeLibrary((HMODULE)*ModuleHandle);

	return GetLastError();
}