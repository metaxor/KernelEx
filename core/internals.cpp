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
#include <cstdio>
#include "internals.h"
#include "resolver.h"
#include "debug.h"
#include "pemanip.h"
#include "ModInit.h"

extern "C" int snprintf(char*, size_t, const char*, ...);

#ifdef _DEBUG
#define _D(x) x
#else
#define _D(x) NULL
#endif

static bool is_winme;
HINSTANCE hInstance;

IMTE*** volatile ppmteModTable = NULL;
HMODULE h_kernel32;
CRITICAL_SECTION* krnl32lock = NULL;
CRITICAL_SECTION* win16lock = NULL;
PDB98** pppdbCur = NULL;
WORD* pimteMax = NULL;
DWORD Obsfucator = 0;

MRFromHLib_t MRFromHLib = NULL;
TIDtoTDB_t TIDtoTDB = NULL;
MRLoadTree_t MRLoadTree = NULL;
FreeLibTree_t FreeLibTree = NULL;
FreeLibRemove_t FreeLibRemove = NULL;
AllocHandle_t AllocHandle = NULL;
GetHandleObject_t GetHandleObject = NULL;
GetHandleObjectFromProcess_t GetHandleObjectFromProcess = NULL;
OpenObjectByName_t OpenObjectByName = NULL;
AllocObjectName_t AllocObjectName = NULL;
CreateRemoteThread_t _CreateRemoteThread = NULL;

sstring kernelex_dir("");
sstring own_path("");

bool isWinMe()
{
	return is_winme;
}

void ShowError(UINT id, ...)
{
	char format[512];
	char out[512];
	va_list vargs;
	
	va_start(vargs, id);
	if (!LoadString(hInstance, id, format, sizeof(format)))
		sprintf(out, "ERROR: %d [Missing error description]", id);
	else
		_vsnprintf(out, sizeof(out), format, vargs);
	va_end(vargs);
	MessageBox(NULL, out, "KernelEx Core", MB_OK | MB_ICONERROR);
}

bool VKernelEx_ioctl(DWORD command, PVOID buffer, DWORD buffer_size)
{
	BOOL result;
	DWORD retlen;
    HANDLE VKernelEx;

	char vxdpath[MAX_PATH];
	snprintf(vxdpath, sizeof(vxdpath), "\\\\.\\%sVKRNLEX.VXD", 
		(const char*) kernelex_dir);

	VKernelEx = CreateFile(vxdpath, 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (VKernelEx == INVALID_HANDLE_VALUE)
	{
		DBGPRINTF(("Failed to connect to VKernelEx!\n"));
		return false;
	}

	result = DeviceIoControl(VKernelEx, command, NULL, 0, 
		buffer, buffer_size, &retlen, NULL);

	CloseHandle(VKernelEx);

	if (!result || retlen > buffer_size)
		return false;

	return true;
}

DWORD* find_unique_pattern(void* start, int size, const short* pattern, int pat_len, const char* pat_name)
{
	unsigned char* pos = (unsigned char*) start;
	unsigned char* end_pos = pos + size - pat_len;
	unsigned char* found_loc = NULL;
	int found = 0;

	while (pos <= end_pos)
	{
		if (pattern[0] < 0 || *pos == pattern[0])
		{
			int j;
			for (j = 1 ; j < pat_len ; j++)
			{
				if (pattern[j] >= 0 && pos[j] != pattern[j])
					break;
			}
			if (j == pat_len) //pattern found
			{
				found++;
				if (!found_loc)
					found_loc = pos;
			}
		}
		pos++;
	}

	if (found != 1)
	{
		if (!found)
			DBGPRINTF(("%s: pattern not found\n", pat_name));
		else
			DBGPRINTF(("%s: pattern not unique, found occurrences: %d\n", pat_name, found));
		return NULL;
	}

	for (int i = 0 ; i < pat_len ; i++)
		if (pattern[i] == -2)
			return (DWORD*) &found_loc[i];

	DBGPRINTF(("%s: invalid pattern\n", pat_name));
	return NULL;
}

static DWORD decode_calljmp(DWORD* addr)
{
	unsigned char* code = (unsigned char*)addr;

	if (code[-1] == 0xe8 || code[-1] == 0xe9) /* call/jmp rel32 */
	{
		return (DWORD)(code + 4 + *(DWORD*)code);
	}
	else if (code[-2] == 0xff 
			&& (code[-1] == 0x15 || code[-1] == 0x25)) /* call/jmp m32 */
	{
		return *(DWORD*)code;
	}
	else if (code[-1] == 0xeb) /* jmp rel8 */
	{
		return (DWORD)(code + 1 + *(char*)code);
	}
	else return 0;
}

MODREF* MRfromCallerAddr(DWORD addr)
{
	MODREF* mr;
	PDB98* ppdbCur = *pppdbCur;
	IMTE** pmteModTable = *ppmteModTable;

	mr = ppdbCur->pExeMODREF;
	if (mr)
	{
		IMTE* imte = pmteModTable[mr->mteIndex];
		IMAGE_NT_HEADERS* nthdr = imte->pNTHdr;
		DWORD img_base = nthdr->OptionalHeader.ImageBase;
		if (addr >= img_base && addr < img_base + nthdr->OptionalHeader.SizeOfImage)
			return mr;
	}
	mr = ppdbCur->MODREFList;
	if (mr)
	{
		do 
		{
			IMTE* imte = pmteModTable[mr->mteIndex];
			IMAGE_NT_HEADERS* nthdr = imte->pNTHdr;
			DWORD img_base = nthdr->OptionalHeader.ImageBase;
			if (addr >= img_base && addr < img_base + nthdr->OptionalHeader.SizeOfImage)
				return mr;
			mr = mr->pNextModRef;
		} 
		while (mr);
	}
	return NULL;
}

HANDLE _OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
	HANDLE ret;
	TDB98* tdb = TIDtoTDB(dwThreadId);
	if (!tdb || tdb->Type != WIN98_K32OBJ_THREAD)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	dwDesiredAccess &= THREAD_ALL_ACCESS;
	if (bInheritHandle)
		dwDesiredAccess |= 0x80000000;
	ret = AllocHandle(*pppdbCur, tdb, dwDesiredAccess);
	if (ret == INVALID_HANDLE_VALUE)
		return NULL;
	return ret;
}

HANDLE _GetProcessHeap()
{
	HANDLE hp = GetProcessHeap();

	if (!hp)
	{
		PDB98* pdb = *pppdbCur;
		IMAGE_NT_HEADERS* nth = (*ppmteModTable)[pdb->pExeMODREF->mteIndex]->pNTHdr;
		//create new default heap
		hp = pdb->DefaultHeap = HeapCreate(0, nth->OptionalHeader.SizeOfHeapCommit, 0);
		//this will prevent the system from creating another default heap
		nth->OptionalHeader.SizeOfHeapReserve = 0;
	}

	DBGASSERT(hp != NULL);
	return hp;
}

/* find win32 mutex */
static CRITICAL_SECTION* find_krnl32lock()
{
	static const char* pat_name = _D("Win32 lock");
	static const short pat[] = {0x55,0xA1,-2,-2,-2,-2,0x8B,0xEC,0x56,0x57,0x33,0xF6,0x50,0xE8};
	static const int pat_len = sizeof(pat) / sizeof(short);

	CRITICAL_SECTION* ret;

	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "VirtualQueryEx"), pat_len, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = *(CRITICAL_SECTION**)*res;
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

/* find current process PDB */
static PDB98** find_curPDB()
{
	static const char* pat_name = _D("pppdbCur");
	static const short pat[] = {0xA1,-2,-2,-2,-2,0xFF,0x30,0xE8,-1,-1,-1,-1,0xC3};
	static const int pat_len = sizeof(pat) / sizeof(short);

	PDB98** ret;

	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "GetCurrentProcessId"), pat_len, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = *(PDB98***)*res;
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

/* find global module tables */
static IMTE*** find_mod_table()
{
	static const char* pat_name = _D("Module table");
	static const short pat[] = {0x8B,0x0D,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);

	IMTE*** ret;
	
	DWORD* res = find_unique_pattern((void*) GetK32OrdinalAddress(23), 0x20, pat, pat_len, pat_name);
	
	ret = (IMTE***)*res;
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static MRFromHLib_t find_MRFromHLib()
{
	static const char* pat_name = _D("MRFromHLib");
	static const short pat[] = {0xE8,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);

	MRFromHLib_t ret;

	DWORD* res = find_unique_pattern((void*) GetK32OrdinalAddress(23), 0x20, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (MRFromHLib_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static WORD* find_pimteMax()
{
	static const char* pat_name = _D("pimteMax");
	static const short pat[] = {0x66,0x8B,0x44,0x24,0x04,0x66,-1,0x05,-2,-2,-2,-2,-1,0x17,0x8B,0x0D,-1,-1,-1,-1,0x0F,0xBF,0xC0,0x8B,0x04,0x81,0x85,0xC0,0x74,0x07,0x8B,0x40,0x04,0x85,0xC0,0x75,0x09};
	static const int pat_len = sizeof(pat) / sizeof(short);
	static const char* sec_name = ".text";

	WORD* ret;
	PEmanip pe(h_kernel32);
	if (!pe.HasTarget())
		return NULL;
	
	DWORD* res = find_unique_pattern(pe.GetSectionByName(sec_name), pe.GetSectionSize(sec_name), pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (WORD*) *res;
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static TIDtoTDB_t find_TIDtoTDB()
{
	static const char* pat_name = _D("TIDtoTDB");
	static const short pat[] = {0x89,-1,0xFF,0x75,0xFC,0xFF,0x77,0x14,0xE8,-2,-2,-2,-2,0x50};
	static const int pat_len = sizeof(pat) / sizeof(short);
	static const char* sec_name = ".text";
	
	TIDtoTDB_t ret;
	PEmanip pe(h_kernel32);
	if (!pe.HasTarget())
		return NULL;

	DWORD* res = find_unique_pattern(pe.GetSectionByName(sec_name), pe.GetSectionSize(sec_name), pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (TIDtoTDB_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static MRLoadTree_t find_MRLoadTree()
{
	static const char* pat_name = _D("MRLoadTree");
	static const short pat[] = {0x33,0xF6,0xE8,-1,-1,-1,-1,0x39,0x35,-1,-1,-1,-1,0x74,0x11,0xA1,-1,-1,-1,-1,0x50,0xE8,-1,-1,-1,-1,0x89,0x35,-1,-1,-1,-1,0xFF,0x74,0x24,0x14,0xE8,-2,-2,-2,-2,0x8B,0xF0,0x85,0xF6};
	static const int pat_len = sizeof(pat) / sizeof(short);
	static const char* sec_name = ".text";

	MRLoadTree_t ret;
	PEmanip pe(h_kernel32);
	if (!pe.HasTarget())
		return NULL;

	DWORD* res = find_unique_pattern(pe.GetSectionByName(sec_name), pe.GetSectionSize(sec_name), pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (MRLoadTree_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static FreeLibTree_t find_FreeLibTree()
{
	static const char* pat_name = _D("FreeLibTree");
	static const short pat[] = {0x75,0x09,0x6A,0x06,0xE8,-1,-1,-1,-1,0xEB,0x08,0x50,0xE8,-2,-2,-2,-2,0x8B,0xF0};
	static const int pat_len = sizeof(pat) / sizeof(short);
	
	FreeLibTree_t ret;
	
	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "FreeLibrary"), 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (FreeLibTree_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static FreeLibRemove_t find_FreeLibRemove()
{
	static const char* pat_name = _D("FreeLibRemove");
	static const short pat[] = {0x8B,0xF0,0x85,0xF6,0x75,0x05,0xE8,-2,-2,-2,-2,0xA1,-1,-1,-1,-1,0x50};
	static const int pat_len = sizeof(pat) / sizeof(short);
	static const char* sec_name = ".text";

	FreeLibRemove_t ret;
	PEmanip pe(h_kernel32);
	if (!pe.HasTarget())
		return NULL;

	DWORD* res = find_unique_pattern(pe.GetSectionByName(sec_name), pe.GetSectionSize(sec_name), pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (FreeLibRemove_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static AllocHandle_t find_AllocHandle()
{
	static const char* pat_name = _D("AllocHandle");
	static const short pat[] = {0x83,0xD1,0xFF,0x81,0xE2,0xFF,0x0F,0x1F,0x00,0x81,0xE1,0x00,0x00,0x00,0x80,0x0B,0xCA,0x8B,0x15,-1,-1,-1,-1,0x51,0x50,0xFF,0x32,0xE8,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);
	
	AllocHandle_t ret;
	
	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "OpenProcess"), 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (AllocHandle_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static GetHandleObject_t find_GetHandleObject()
{
	static const char* pat_name = _D("GetHandleObject");
	static const short pat[] = {0x6A,0x00,0x68,0x02,0x00,0x00,0x80,0xFF,0x74,0x24,0x0C,0xE8,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);
	
	GetHandleObject_t ret;

	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "OpenVxDHandle"), 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (GetHandleObject_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static GetHandleObjectFromProcess_t find_GetHandleObjectFromProcess()
{
	static const char* pat_name = _D("GetHandleObjectFromProcess");
	static const short pat[] = {0x55,0xA1,-1,-1,-1,-1,0x8B,0xEC,0xFF,0x74,0x24,0x10,0xFF,0x75,0x0C,0xFF,0x75,0x08,0xFF,0x30,0xE8,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);
	
	GetHandleObjectFromProcess_t ret;

	DWORD* res = find_unique_pattern((void*) GetHandleObject, 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (GetHandleObjectFromProcess_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static OpenObjectByName_t find_OpenObjectByName()
{
	/* ObOpenObjectByName is inside ICreateEvent */
	static const char* pat_name = _D("ICreateEvent");
	static const short pat[] = {0x8B,0x7C,0x24,0x20,0x0B,0xFF,0x74,0x07,0x2B,0xC0,0x8D,0x48,0xFF,0xF2,0xAE,0x64,0x8F,0x02,0x83,0xC4,0x08,0x5F,0xE9,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);

	static const char* pat2_name = _D("OpenObjectByName");
	static const short pat2[] = {0x85,0xF6,0x74,0x24,0x55,0x6A,0x02,0x56,0xE8,-2,-2,-2,-2};
	static const int pat2_len = sizeof(pat2) / sizeof(short);
	
	OpenObjectByName_t ret;
	DWORD ICreateEvent;

	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "CreateEventA"), 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ICreateEvent = decode_calljmp(res);

	if(ICreateEvent == NULL)
		return NULL;

	res = find_unique_pattern((void*) ICreateEvent, 0x80, pat2, pat2_len, pat2_name);
	if (!res)
		return NULL;

	ret = (OpenObjectByName_t)decode_calljmp(res);

	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static AllocObjectName_t find_AllocObjectName()
{
	static const char* pat_name = _D("ICreateEvent");
	static const short pat[] = {0x8B,0x7C,0x24,0x20,0x0B,0xFF,0x74,0x07,0x2B,0xC0,0x8D,0x48,0xFF,0xF2,0xAE,0x64,0x8F,0x02,0x83,0xC4,0x08,0x5F,0xE9,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);

	static const char* pat2_name = _D("AllocObjectName");
	static const short pat2[] = {0xEB,0x26,0x85,0xF6,0x74,0x2A,0x56,0x53,0xE8,-2,-2,-2,-2};
	static const int pat2_len = sizeof(pat2) / sizeof(short);
	
	AllocObjectName_t ret;
	DWORD ICreateEvent;

	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "CreateEventA"), 0x80, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ICreateEvent = decode_calljmp(res);

	if(ICreateEvent == NULL)
		return NULL;

	res = find_unique_pattern((void*) ICreateEvent, 0x100, pat2, pat2_len, pat2_name);
	if (!res)
		return NULL;

	ret = (AllocObjectName_t)decode_calljmp(res);

	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static CreateRemoteThread_t find_CreateRemoteThread()
{
	static const char* pat_name = _D("CreateRemoteThread");
	static const short pat[] = {0x85,0xF6,0x74,0x22,0x6A,0x08,0x57,0x68,-1,-1,-1,-1,0x68,0x00,0xF0,0xFF,0xFF,0x57,0xE8,-2,-2,-2,-2};
	static const int pat_len = sizeof(pat) / sizeof(short);
	
	CreateRemoteThread_t ret;
	
	DWORD* res = find_unique_pattern((void*) iGetProcAddress(h_kernel32, "DebugActiveProcess"), 0x100, pat, pat_len, pat_name);
	if (!res)
		return NULL;

	ret = (CreateRemoteThread_t)decode_calljmp(res);
	DBGPRINTF(("%s @ 0x%08x\n", pat_name, ret));
	return ret;
}

static bool find_kernelex_install_dir()
{
	//registry value InstallDir is written by the installer
	HKEY key;
	DWORD type;
	char path[MAX_PATH];
	DWORD len = sizeof(path);
	DWORD attr;
	long result;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KernelEx", 
			0, KEY_QUERY_VALUE, &key);
	if (result != ERROR_SUCCESS)
	{
		DBGPRINTF(("Failed to open KernelEx registry key\n"));
		return false;
	}

	result = RegQueryValueEx(key, "InstallDir", NULL, &type, 
			(BYTE*)path, &len);
	RegCloseKey(key);
	if (result != ERROR_SUCCESS || type != REG_SZ || len == 0)
	{
		DBGPRINTF(("Failed to read InstallDir registry value\n"));
		return false;
	}

	if (path[strlen(path) - 1] == '\\')
		path[strlen(path) - 1] = '\0';

	attr = GetFileAttributes(path);
	if (attr == 0xffffffff || !(attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		DBGPRINTF(("KernelEx directory [%s] doesn't exist\n", path));
		return false;
	}

	DBGPRINTF(("KernelEx directory: %s\n", path));
	strcat(path, "\\");
	kernelex_dir = strupr(path);
	return true;
}

int internals_init()
{
	DBGPRINTF(("KernelEx loaded @ %08x\n", hInstance));
	DBGPRINTF(("internals_init()\n"));
	h_kernel32 = GetModuleHandle("kernel32");
	ppmteModTable = find_mod_table();
	MRFromHLib = find_MRFromHLib();
	krnl32lock = find_krnl32lock();
	_GetpWin16Lock(&win16lock);
	pppdbCur = find_curPDB();
	pimteMax = find_pimteMax();
	TIDtoTDB = find_TIDtoTDB();
	MRLoadTree = find_MRLoadTree();
	FreeLibTree = find_FreeLibTree();
	FreeLibRemove = find_FreeLibRemove();
	AllocHandle = find_AllocHandle();
	GetHandleObject = find_GetHandleObject();
	GetHandleObjectFromProcess = find_GetHandleObjectFromProcess();
	OpenObjectByName = find_OpenObjectByName();
	AllocObjectName = find_AllocObjectName();
	_CreateRemoteThread = find_CreateRemoteThread();
	bool instdir_rslt = find_kernelex_install_dir();
	is_winme = (GetVersion() == 0xc0005a04);
	bool modinit_rslt = ModuleInitializer_init();
	Obsfucator = (DWORD)get_pdb() ^ GetCurrentProcessId();

	if (!h_kernel32 || !ppmteModTable || !MRFromHLib || !krnl32lock || !win16lock || !pppdbCur
			|| !pimteMax || !TIDtoTDB || !MRLoadTree || !FreeLibTree 
			|| !FreeLibRemove || !AllocHandle || !GetHandleObject || !GetHandleObjectFromProcess
			|| !OpenObjectByName || !AllocObjectName || !_CreateRemoteThread || !instdir_rslt || !modinit_rslt)
		return 0;
	return 1;
}

void internals_uninit()
{
	DBGPRINTF(("internals_uninit()\n"));
}
