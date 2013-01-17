/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

extern "C" {
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
};
#include "patch_ifsmgr.h"
#include "debug.h"
#include "util.h"

#define __IFSMGR_CheckLocks		0x00400056

/*
 * This is a fix against a problem where you can't access a locked file
 * through memory mapping object.
 *
 * Why accessing a locked file through memory mapping object doesn't work?
 *
 * IFSMgr normally checks if the process is an owner of the lock
 * and depending on that either allows to access the file or not.
 *
 * This works fine in case scenario 1. where the file is read directly.
 *
 * Accessing a file through memory mapping object is a different beast.
 * The file is loaded into memory by the system on demand, through a page
 * fault trap (see 2.), this causes a read of a page-sized chunk of file.
 * However this time it is the system not the application who's reading
 * the file, so process ID is set to -1. This value is not checked
 * against when the file is verified for access regarding the locks.
 *
 * The fix is to allow the access to a locked file if PID is -1.
 */

#if 0 /* Test case */

int main()
{
	HANDLE file, mapp;
	void* view;
	char buf[4096];
	BOOL ret;
	DWORD dw;

	file = CreateFileA("File.txt", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL);

	ret = LockFile(file, 128, 0, 1, 0);

	SetFilePointer(file, 32768, 0, 0);
	SetEndOfFile(file);

	mapp = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 32768, NULL);
	view = MapViewOfFile(mapp, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 32768);

	SetFilePointer(file, 0, 0, 0);

    //!!!!! 1. This works out of the box
	ReadFile(file, buf, 4096, &dw, NULL);

    //!!!!! 2. This triggers a crash without the fix
	memcpy(buf, view, 4096);

	UnmapViewOfFile(view);
	CloseHandle(mapp);

	UnlockFile(file, 128, 0, 1, 0);

	CloseHandle(file);
}

#endif

__declspec(naked)
int IFSMGR_CheckLocks_fixed(
	void* handle,
	DWORD pos,
	DWORD count,
	DWORD owner,
	DWORD finst,
	DWORD flags
)
{
__asm {
    push    ebp
    mov     ebp,esp
    push    ebx
    push    esi
    push    edi
    mov     eax,[ebp+10h]
    or      eax,eax
    jnz     _L1
    sub     eax,eax
    jmp     _exit_grant
_L1:
    mov     edi,[ebp+0Ch]
    lea     ecx,[eax+edi-1]
    cmp     ecx,edi
    jnb     _L2
    jmp     _exit_forbid
_L2:
    mov     esi,[ebp+8]
    sub     eax,eax
    cmp     [esi+0Ch],edi
    jb      _exit_grant
    cmp     [esi+8],ecx
    ja      _exit_grant
    mov     edx,[esi]
    cmp     esi,edx
    jz      _exit_grant
	//the fix
    mov     ebx,[ebp+14h]
	cmp     ebx,-1
	jz		_exit_grant
	//end of the fix
_loop:
    cmp     [edx+8],ecx
    ja      _exit_grant
    cmp     [edx+0Ch],edi
    jb      _next
	//check lock owner
    mov     ebx,[ebp+14h]
    cmp     [edx+14h],ebx
    jnz     _owner_mismatch
	//check file handle
    mov     ebx,[ebp+18h]
    cmp     [edx+18h],ebx
    jz      _next
_owner_mismatch:
    mov     ebx,[edx+10h]
    and     ebx,[ebp+1Ch]
    test    bl,1
    jz      _exit_forbid
_next:
    mov     edx,[edx]
    cmp     esi,edx
    jnz     _loop
    jmp     _exit_grant
_exit_forbid:
    mov     eax,21h
_exit_grant:
    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    ret
	}
}

bool Patch_ifsmgr::apply()
{
	DBGPRINTF(("Applying IFSMGR_CheckLocks fix... "));
	ULONG serviceid = GetVxDServiceOrdinal(IFSMGR_CheckLocks);
	bool ret = Hook_Device_Service(serviceid, (ULONG) IFSMGR_CheckLocks_fixed);
	DBGPRINTF((ret ? "... ok" : "... error"));
	return ret;
}
