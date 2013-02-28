/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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

#include <windows.h>
#include <stdarg.h>
#include <Tlhelp32.h>
#include "version.h"
#include "kexcoresdk.h"
#include "kstructs.h"
#include "resolver.h"
#include "internals.h"
#include "SettingsDB.h"
#include "sharedmem.h"
#include "debug.h"
#undef MAKE_PS_TAG
#include "ProcessStorage.h"

unsigned long kexGetKEXVersion()
{
	return VERSION_CODE;
}

DWORD kexGetCoreCaps()
{
	DWORD caps = 0;
#ifdef _DEBUG
	caps |= KCC_DEBUG;
#endif
#ifdef _ENABLE_APIHOOK
	caps |= KCC_APIHOOK;
#endif
	return caps;
}

void kexDebugPrint(const char* format, ...)
{
#ifdef _DEBUG
	va_list argp;
	va_start(argp, format);
	dbgvprintf(format, argp);
	va_end(argp);
#endif
}

DWORD kexGetVersion()
{
	return GetVersion();
}

HANDLE kexAllocHandle(PVOID TargetProcess, PVOID Object, DWORD dwDesiredAccess)
{
	HANDLE hObject;

	if(TargetProcess == NULL)
		TargetProcess = get_pdb();

	hObject = AllocHandle((PDB98*)TargetProcess, Object, dwDesiredAccess);

	return hObject == INVALID_HANDLE_VALUE ? NULL : hObject;
}

PVOID kexAllocObject(size_t size)
{
	void* ptr = malloc(size);
	return ptr;
}

LPSTR kexAllocObjectName(PVOID Object, LPCSTR lpName)
{
	return AllocObjectName(Object, lpName);
}

PTDB98 kexCreateRemoteThread(PPDB98 Process,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags
)
{
	return _CreateRemoteThread(Process, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags);
}

BOOL kexDereferenceObject(PVOID Object)
{
	PK32OBJHEAD K32Object = NULL;

	if(IsBadWritePtr(Object, sizeof(K32OBJHEAD)))
		return FALSE;

	K32Object = (PK32OBJHEAD)Object;

	K32Object->cReferences--;

	return TRUE;
}

BOOL kexEnumProcesses(DWORD *pProcessIds, DWORD cb, DWORD *pBytesReturned)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot;

	if (cb < sizeof(DWORD))
		return FALSE;

	cb = cb & ~sizeof(DWORD);

	if (IsBadWritePtr(pProcessIds,sizeof(DWORD)) || IsBadWritePtr(pBytesReturned,sizeof(DWORD)))
	{
		SetLastError(ERROR_NOACCESS);
		return FALSE;
	}

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return FALSE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if ( Process32First(hSnapshot, &pe32) )
	{
		*pProcessIds = pe32.th32ProcessID;
		pProcessIds++;
		*pBytesReturned = sizeof(DWORD);

		while ( Process32Next(hSnapshot, &pe32) && *pBytesReturned < cb )
		{
			*pProcessIds = pe32.th32ProcessID;
			pProcessIds++;
			*pBytesReturned += sizeof(DWORD);			
		}
	}

	CloseHandle(hSnapshot);

	return TRUE;
}

BOOL kexEnumThreads(DWORD dwProcessId, DWORD *pThreadIds, DWORD cb, DWORD *pBytesReturned)
{
	HANDLE hSnapshot = NULL;
	THREADENTRY32 te32;
	DWORD count = 0;
	DWORD BytesReturned = 0;

	if(IsBadReadPtr(pThreadIds, cb) && IsBadReadPtr(pBytesReturned, sizeof(DWORD)))
		return FALSE;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if(hSnapshot == NULL)
		return FALSE;

	memset(&te32, 0, sizeof(THREADENTRY32));

	te32.dwSize = sizeof(THREADENTRY32);

	if(!Thread32First(hSnapshot, &te32))
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}

	do
	{
		if(dwProcessId != 0 && dwProcessId != te32.th32OwnerProcessID)
			goto nextthread;

		pThreadIds[count] = te32.th32ThreadID;
		BytesReturned += sizeof(DWORD);
		count++;
nextthread:
;
	} while(Thread32Next(hSnapshot, &te32) && BytesReturned <= cb);

	CloseHandle(hSnapshot);

	if(!IsBadReadPtr(pBytesReturned, sizeof(DWORD)))
		*pBytesReturned = BytesReturned;

	if(count == 0)
		return FALSE;

	return TRUE;
}

VOID kexFreeObject(PVOID Object)
{
    free(Object);
}

BOOL kexFindObjectHandle(PVOID Process, PVOID Object, WORD ObjectType, PHANDLE Handle)
{
	PPDB98 ppdb;
	PHANDLE_TABLE pHandleTable;
	ULONG index;
	PK32OBJHEAD k32obj;

	if(Process != NULL && IsBadReadPtr(Process, sizeof(PDB98)))
		return FALSE;

	if(IsBadReadPtr(Object, sizeof(K32OBJHEAD)) && ObjectType == 0)
		return FALSE;

	if(IsBadReadPtr(Handle, sizeof(DWORD)) || IsBadReadPtr(Process, sizeof(PDB98)))
		return FALSE;

	if(Process == NULL)
		Process = get_pdb();

	ppdb = (PPDB98)Process;
	pHandleTable = ppdb->pHandleTable;

	*Handle = NULL;

	for(index=1;index<=pHandleTable->cEntries;index++)
	{
		k32obj = (PK32OBJHEAD)pHandleTable->array[index].pObject;

		if(IsBadReadPtr(k32obj, sizeof(K32OBJHEAD)))
			continue;

		if(ObjectType != 0)
		{
			if(k32obj->Type == ObjectType)
			{
				if(Object != NULL && k32obj != Object)
					continue;

				*Handle = (HANDLE)(index*4);
				return TRUE;
			}
		}
		else
		{
			if(k32obj == Object)
			{
				*Handle = (HANDLE)(index*4);
				return TRUE;
			}
		}
	}

	/* FIXME: Should we return FALSE or the first handle found? */
	return FALSE;
}

DWORD kexGetHandleAccess(HANDLE hHandle)
{
	DWORD access;
	DWORD index;
	PHANDLE_TABLE pHandleTable;

	pHandleTable = get_pdb()->pHandleTable;

	index = (DWORD)hHandle/4;
	if(index > pHandleTable->cEntries)
		return 0;

	if(IsBadReadPtr(pHandleTable->array[index].pObject, sizeof(K32OBJHEAD)))
		return 0;

	access = pHandleTable->array[index].flags;

	return access;
}

BOOL kexSetHandleAccess(HANDLE hHandle, DWORD dwDesiredAccess)
{
	DWORD index;
	PHANDLE_TABLE pHandleTable;

	pHandleTable = get_pdb()->pHandleTable;

	index = (DWORD)hHandle/4;
	if(index > pHandleTable->cEntries)
		return FALSE;

	if(IsBadReadPtr(pHandleTable->array[index].pObject, sizeof(K32OBJHEAD)))
		return FALSE;

	pHandleTable->array[index].flags = dwDesiredAccess;

	return TRUE;
}

PVOID kexGetHandleObject(HANDLE hHandle, WORD ObjectType, DWORD un1)
{
	DWORD HandleType;

	HandleType = K32OBJTYPE_TO_HANDLETYPE(ObjectType);

	return GetHandleObject(hHandle, HandleType, un1);
}

PVOID kexGetHandleObjectFromProcess(PVOID Process, HANDLE hHandle, WORD ObjectType, DWORD un1)
{
	DWORD HandleType;

	HandleType = K32OBJTYPE_TO_HANDLETYPE(ObjectType);

	return GetHandleObjectFromProcess((PPDB98)Process, hHandle, HandleType, un1);
}

PROC kexGetProcAddress(HMODULE hModule, PCSTR lpProcName)
{
	return iGetProcAddress(hModule, lpProcName);
}

VOID kexGrabKrnl32Lock()
{
	if(krnl32lock == NULL)
		return;

	_EnterSysLevel(krnl32lock);
}

VOID kexGrabLocks()
{
	if(win16lock == NULL || krnl32lock == NULL)
		return;

	_EnterSysLevel(win16lock);
	_EnterSysLevel(krnl32lock);
}

VOID kexGrabWin16Lock()
{
	if(win16lock == NULL)
		return;

	_EnterSysLevel(win16lock);
}

PVOID kexGetProcess(DWORD dwProcessId)
{
	PPDB98 Process;

	Process = (PPDB98)(dwProcessId ^ Obsfucator);

	if(IsBadReadPtr(Process, sizeof(DWORD)))
		return NULL;

	if(Process->Type != WIN98_K32OBJ_PROCESS)
		return NULL;

	return (PVOID)Process;
}

PVOID kexGetThread(DWORD dwThreadId)
{
    return TIDtoTDB(dwThreadId);
}

BOOL kexGetProcessName(DWORD dwProcessId, char *buffer)
{
	HANDLE hSnapshot = NULL;
	PROCESSENTRY32 pe32;
	char *pName = NULL;

	if(dwProcessId == 0)
		return FALSE;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hSnapshot == NULL)
		return FALSE;

	memset(&pe32, 0, sizeof(PROCESSENTRY32));

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}

	do
	{
		if(pe32.th32ProcessID == dwProcessId)
		{
			CloseHandle(hSnapshot);
			pName = strrchr(pe32.szExeFile, '\\');
			if(pName != NULL)
			{
				*pName = 0;
				pName++;
				strcpy(buffer, pName);
			}
			else
				strcpy(buffer, pe32.szExeFile);
			return TRUE;
		}
	} while(Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);

	return FALSE;
}

HANDLE kexOpenObjectByName(LPCSTR lpName, WORD ObjectType, DWORD dwDesiredAccess)
{
	HANDLE hObject;

	hObject = OpenObjectByName(lpName, ObjectType, dwDesiredAccess);

	return hObject == (HANDLE)INVALID_HANDLE_VALUE ? NULL : hObject;
}

HANDLE kexOpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
	return _OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
}

VOID kexReleaseKrnl32Lock()
{
	_LeaveSysLevel(krnl32lock);
}

VOID kexReleaseLocks()
{
	_LeaveSysLevel(win16lock);
	_LeaveSysLevel(krnl32lock);
}

VOID kexReleaseWin16Lock()
{
	_LeaveSysLevel(win16lock);
}

BOOL kexAreExtensionsEnabled()
{
	return are_extensions_enabled();
}

void kexGetModuleSettings(const char* module, 
                          char* conf_name, DWORD* mod_flags)
{
	appsetting as = SettingsDB::instance.get_appsetting(module);
	if (!as.conf)
	{
		conf_name[0] = '\0';
	}
	else
	{
		strncpy(conf_name, as.conf->get_name(), 256);
		conf_name[255] = '\0';
	}

	DWORD flags = 0;
	if (as.flags & LDR_KEX_DISABLE) flags |= KRF_KEX_DISABLE;
	if (as.flags & LDR_OVERRIDE_PROC_MOD) flags |= KRF_OVERRIDE_PROC_MOD;
	if (as.flags & LDR_HOOK_APIS) flags |= KRF_HOOK_APIS;
	if (as.flags & LDR_NO_INHERIT) flags |= KRF_NO_INHERIT;
	if (as.flags & LDR_VALID_FLAG) flags |= KRF_VALID_FLAG;
	*mod_flags = flags;
}

void kexSetModuleSettings(const char* module,
                          const char* conf_name, DWORD mod_flags)
{
	BYTE flags = 0;
	if (mod_flags & KRF_KEX_DISABLE) flags |= LDR_KEX_DISABLE;
	if (mod_flags & KRF_OVERRIDE_PROC_MOD) flags |= LDR_OVERRIDE_PROC_MOD;
	if (mod_flags & KRF_HOOK_APIS) flags |= LDR_HOOK_APIS;
	if (mod_flags & KRF_NO_INHERIT) flags |= LDR_NO_INHERIT;
	SettingsDB::instance.write_single(module, conf_name, flags);
}

void kexResetModuleSettings(const char* module)
{
	SettingsDB::instance.reset_single(module);
}

void kexFlushAppSettings(void)
{
	SettingsDB::instance.flush_all();
}

int kexPsAllocIndex(void)
{
	return ProcessStorage::allocate();
}

void* kexPsGetValue(int index)
{
	return ProcessStorage::get_instance()->get(index);
}

int kexPsSetValue(int index, void* value)
{
	return ProcessStorage::get_instance()->set(index, value);
}

void* kexGetK32Lock()
{
	return krnl32lock;
}

UINT kexGetKernelExDirectory(LPSTR lpBuffer, UINT uSize)
{
	__try
	{
		return strlen(strncpy(lpBuffer, (const char*)kernelex_dir, uSize));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
}

UINT kexErrorCodeToString(ULONG ErrorCode, LPSTR lpBuffer)
{
	return FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					ErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) lpBuffer,
					0,
					NULL);
}
