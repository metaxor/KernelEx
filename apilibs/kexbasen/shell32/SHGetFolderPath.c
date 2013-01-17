/*
 *  KernelEx
 *  Copyright (C) 2009-2010, Xeno86, Tihiy
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
#include <shlobj.h>
#include "kexcoresdk.h"
#include "folderfix.h"

#include <pshpack1.h>
typedef struct
{
	BYTE jmp;
	DWORD func;
} LONGJMP, *PLONGJMP;
#include <poppack.h>

typedef HRESULT (WINAPI *SHGetFolderPathA_t)(HWND, int, HANDLE, DWORD, LPSTR);
typedef HRESULT (WINAPI *SHGetFolderPathW_t)(HWND, int, HANDLE, DWORD, LPWSTR);

static HMODULE hShfolder;
static HMODULE hShell32;

static SHGetFolderPathA_t SHGetFolderPathA_pfn = (SHGetFolderPathA_t)-1;
static SHGetFolderPathW_t SHGetFolderPathW_pfn = (SHGetFolderPathW_t)-1;

static BYTE prev_entry[sizeof(LONGJMP)];

static BOOL APIENTRY shfolder_entry(HINSTANCE inst, DWORD reason, BOOL load_static)
{
	if (reason == DLL_PROCESS_DETACH)
	{
		DBGPRINTF(("kexbasen: shfolder detached\n"));
		SHGetFolderPathA_pfn = (SHGetFolderPathA_t)-1;
		SHGetFolderPathW_pfn = (SHGetFolderPathW_t)-1;
		hShfolder = NULL;
	}
	return TRUE;
}

/*
 * Protection against buggy apps which call FreeLibrary on
 * a DLL handle too many times.
 */
static void protect_shfolder()
{
	DWORD fold;
	DWORD fnew;
	DWORD entry_addr;
	PLONGJMP ljmp;
	DWORD dwShfolder = (DWORD) hShfolder;
	IMAGE_DOS_HEADER* dosh = (IMAGE_DOS_HEADER*) hShfolder;
	IMAGE_NT_HEADERS* nth = (IMAGE_NT_HEADERS*) (dwShfolder + dosh->e_lfanew);

	entry_addr = dwShfolder + nth->OptionalHeader.AddressOfEntryPoint;
	memcpy(prev_entry, (PVOID) entry_addr, sizeof(LONGJMP));
	VirtualProtect((PVOID) entry_addr, sizeof(LONGJMP), PAGE_READWRITE, &fold);
	ljmp = (PLONGJMP) entry_addr;
	ljmp->jmp = 0xe9; //jmp near rel
	ljmp->func = (DWORD) shfolder_entry - (entry_addr + sizeof(LONGJMP));
	VirtualProtect((PVOID) entry_addr, sizeof(LONGJMP), fold, &fnew);
}

static void unprotect_shfolder()
{
	DWORD fold;
	DWORD fnew;
	DWORD entry_addr;
	DWORD dwShfolder = (DWORD) hShfolder;
	IMAGE_DOS_HEADER* dosh = (IMAGE_DOS_HEADER*) hShfolder;
	IMAGE_NT_HEADERS* nth = (IMAGE_NT_HEADERS*) (dwShfolder + dosh->e_lfanew);

	entry_addr = dwShfolder + nth->OptionalHeader.AddressOfEntryPoint;
	VirtualProtect((PVOID) entry_addr, sizeof(LONGJMP), PAGE_READWRITE, &fold);
	memcpy((PVOID) entry_addr, prev_entry, sizeof(LONGJMP));
	VirtualProtect((PVOID) entry_addr, sizeof(LONGJMP), fold, &fnew);
}

static PROC LoadShProc(LPCSTR proc)
{
	static const char ShfolderFn[] = "SHFOLDER.DLL";
	static const char Shell32Fn[] = "SHELL32.DLL";
	PROC ret = NULL;
	DWORD lasterr = GetLastError();
	
	//first try with shell32
	if (!hShell32)
		hShell32 = LoadLibrary(Shell32Fn);
	if (hShell32) ret = kexGetProcAddress(hShell32, proc);
	
	//fallback to shfolder
	if (!ret)
	{
		if (!hShfolder) 
		{
			hShfolder = LoadLibrary(ShfolderFn);
			if (hShfolder)
				protect_shfolder();
		}
		if (hShfolder) 
			ret = kexGetProcAddress(hShfolder, proc);
	}
	SetLastError(lasterr);
	return ret;
}

void uninit_SHGetFolderPath()
{
	if (!hShfolder) return;

	unprotect_shfolder();
	hShfolder = NULL;
}

/* MAKE_EXPORT SHGetFolderPathA_new=SHGetFolderPathA */
HRESULT WINAPI SHGetFolderPathA_new(
		HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPSTR pszPath)
{
	if (SHGetFolderPathA_pfn == (void*)-1)
		SHGetFolderPathA_pfn = (SHGetFolderPathA_t) LoadShProc("SHGetFolderPathA");
	if (SHGetFolderPathA_pfn == NULL)
		return E_NOTIMPL;
	nFolder = folder_fix(nFolder);
	return SHGetFolderPathA_pfn(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}

/* MAKE_EXPORT SHGetFolderPathW_new=SHGetFolderPathW */
HRESULT WINAPI SHGetFolderPathW_new(
		HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath)
{
	if (SHGetFolderPathW_pfn == (void*)-1)
		SHGetFolderPathW_pfn = (SHGetFolderPathW_t) LoadShProc("SHGetFolderPathW");
	if (SHGetFolderPathW_pfn == NULL)
		return E_NOTIMPL;
	nFolder = folder_fix(nFolder);
	return SHGetFolderPathW_pfn(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}
