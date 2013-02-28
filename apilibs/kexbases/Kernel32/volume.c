/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

#include "common.h"

#define K32OBJ_VOLUME	0x18

typedef struct _VOLUME
{
	WORD	Type;
	WORD	cReferences;
	LPSTR	lpName;
	LPSTR	lpCurrentDrive;
} VOLUME, *PVOLUME;

/* MAKE_EXPORT FindFirstVolumeA_new=FindFirstVolumeA */
HANDLE WINAPI FindFirstVolumeA_new(LPSTR lpszVolumeName, DWORD cchBufferLength)
{
	PVOLUME pVol = NULL;
	HANDLE hVolume = NULL;
	BOOL Result = FALSE;
	DWORD ComponentLength = 0;
	DWORD FileSystemFlags = 0;
	CHAR FileSystemName[MAX_PATH+1];
	LPSTR SVolumeName = NULL;
	LPSTR SDriveName = NULL;

	/* Retrieves the volume name */
	Result = GetVolumeInformation("A:\\",
						lpszVolumeName,
						cchBufferLength,
						NULL,
						&ComponentLength,
						&FileSystemFlags,
						FileSystemName,
						sizeof(FileSystemName));

	if(!Result)
	{
		Result = GetVolumeInformation("B:\\",
							lpszVolumeName,
							cchBufferLength,
							NULL,
							&ComponentLength,
							&FileSystemFlags,
							FileSystemName,
							sizeof(FileSystemName));

		if(!Result)
		{
			Result = GetVolumeInformation("C:\\",
								lpszVolumeName,
								cchBufferLength,
								NULL,
								&ComponentLength,
								&FileSystemFlags,
								FileSystemName,
								sizeof(FileSystemName));

			if(!Result)
				return (HANDLE)ERROR_INVALID_HANDLE;
		}
	}

	pVol = (PVOLUME)kexAllocObject(sizeof(VOLUME));
	SVolumeName = (LPSTR)kexAllocObject(strlen(lpszVolumeName));
	SDriveName = (LPSTR)kexAllocObject(6);

	if(pVol == NULL || SVolumeName == NULL || SDriveName == NULL)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return (HANDLE)ERROR_INVALID_HANDLE;
	}

	strcpy(SVolumeName, lpszVolumeName);
	strcpy(SDriveName, "C:\\");

	pVol->Type = K32OBJ_VOLUME;
	pVol->cReferences = 0;
	pVol->lpName = SVolumeName;
	pVol->lpCurrentDrive = SDriveName;

	hVolume = kexAllocHandle(NULL, pVol, 0x80000000);

	if(hVolume == NULL)
	{
		kexFreeObject(pVol->lpCurrentDrive);
		kexFreeObject(pVol->lpName);
		kexFreeObject(pVol);
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return (HANDLE)ERROR_INVALID_HANDLE;
	}

	return hVolume;
}

/* MAKE_EXPORT FindNextVolumeA_new=FindNextVolumeA */
BOOL WINAPI FindNextVolumeA_new(HANDLE hFindVolume, LPTSTR lpszVolumeName, DWORD cchBufferLength)
{
	PVOLUME pVol = (PVOLUME)kexGetHandleObject(hFindVolume, K32OBJ_VOLUME, 0);
	BOOL Result = FALSE;
	DWORD ComponentLength = 0;
	DWORD FileSystemFlags = 0;
	CHAR FileSystemName[MAX_PATH+1];

	if(pVol == NULL)
		return FALSE;

	do
	{
		pVol->lpCurrentDrive[0]++;

		TRACE("lpCurrentDrive: %s\n", pVol->lpCurrentDrive);

		Result = GetVolumeInformation(pVol->lpCurrentDrive,
							lpszVolumeName,
							cchBufferLength,
							NULL,
							&ComponentLength,
							&FileSystemFlags,
							FileSystemName,
							sizeof(FileSystemName));
	} while(pVol->lpCurrentDrive[0] != 'Z' && Result == FALSE);

	if(Result == FALSE)
	{
		SetLastError(ERROR_NO_MORE_FILES);
		return FALSE;
	}

	return TRUE;
}

/* MAKE_EXPORT FindVolumeClose_new=FindVolumeClose */
BOOL WINAPI FindVolumeClose_new(HANDLE hFindVolume)
{
	PVOLUME pVol = (PVOLUME)kexGetHandleObject(hFindVolume, K32OBJ_VOLUME, 0);

	if(pVol == NULL)
		return FALSE;

	kexDereferenceObject(pVol);
	kexDereferenceObject(pVol);

	if(pVol->cReferences < 1)
	{
		kexFreeObject(pVol->lpName);
		kexFreeObject(pVol);
	}

	return TRUE;
}

/* MAKE_EXPORT GetVolumePathNamesForVolumeNameA_new=GetVolumePathNamesForVolumeNameA */
BOOL WINAPI GetVolumePathNamesForVolumeNameA_new(LPCSTR lpszVolumeName, LPSTR lpszVolumePathNames, DWORD cchBufferLength, PDWORD lpcchReturnLength)
{
	/* FIXME: Need a VxD service that function like MOUNTMGR NT service
			  to retrieve the GUID of a volume */
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT GetVolumeNameForVolumeMountPointA_new=GetVolumeNameForVolumeMountPointA */
BOOL WINAPI GetVolumeNameForVolumeMountPointA_new(LPCSTR lpszVolumeMountPoint, LPSTR lpszVolumeName, DWORD cchBufferLength)
{
	strncpy(lpszVolumeName, "\\\\?\\Volume{00000000-0000-0000-0000-000000000000}\\", cchBufferLength);
	return TRUE;
}
