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

#ifndef __INTERNALS_H
#define __INTERNALS_H

#include "kstructs.h"
#include "sharedmem.h"
#include "sstring.hpp"
#include "resource.h"
#include "k32ord.h"

#define countof(x) (sizeof(x)/sizeof(x[0]))

#define IS_SHARED(x) (((DWORD)x) >= 0x80000000)

extern HINSTANCE hInstance;

extern IMTE*** volatile ppmteModTable;
extern HMODULE h_kernel32;
extern CRITICAL_SECTION* krnl32lock;
extern CRITICAL_SECTION* win16lock;
extern PDB98** pppdbCur;
extern WORD* pimteMax;
extern DWORD Obsfucator;

extern sstring kernelex_dir;
extern sstring own_path;

void ShowError(UINT id, ...);
bool VKernelEx_ioctl(DWORD command, PVOID buffer, DWORD buffer_size);
bool isWinMe();

typedef MODREF* (__stdcall *MRFromHLib_t)(HMODULE);
typedef TDB98* (__stdcall *TIDtoTDB_t)(DWORD);
typedef MODREF* (__stdcall * MRLoadTree_t)(LPCSTR);
typedef BOOL (__stdcall * FreeLibTree_t)(MODREF*);
typedef BOOL (__stdcall * FLoadTreeNotify_t)(MODREF*, BOOL);
typedef VOID (__stdcall * FreeLibRemove_t)(VOID);
typedef HANDLE (__stdcall *AllocHandle_t)(PDB98*, PVOID Object, DWORD dwDesiredAccess);
typedef HANDLE (__stdcall *GetHandleObject_t)(HANDLE hHandle, DWORD HandleType, DWORD);
typedef HANDLE (__stdcall *GetHandleObjectFromProcess_t)(PPDB98 Process, HANDLE hHandle, DWORD HandleType, DWORD);
typedef HANDLE (__stdcall *OpenObjectByName_t)(LPCSTR lpName, WORD ObjectType, DWORD dwDesiredAccess);
typedef LPSTR (__stdcall *AllocObjectName_t)(PVOID Object, LPCSTR lpName);
typedef PTDB98 (__stdcall *CreateRemoteThread_t)(PPDB98, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD);


extern MRFromHLib_t MRFromHLib;

/** Convert Thread ID into pointer to Thread Database. 
 * @param tid Thread ID.
 * @return Pointer to Thread Database.
 */
extern TIDtoTDB_t TIDtoTDB;

extern MRLoadTree_t MRLoadTree;
extern FreeLibTree_t FreeLibTree;
extern FLoadTreeNotify_t FLoadTreeNotify;
extern FreeLibRemove_t FreeLibRemove;
extern AllocHandle_t AllocHandle;
extern GetHandleObject_t GetHandleObject;
extern GetHandleObjectFromProcess_t GetHandleObjectFromProcess;
extern OpenObjectByName_t OpenObjectByName;
extern AllocObjectName_t AllocObjectName;
extern CreateRemoteThread_t _CreateRemoteThread;

MODREF* MRfromCallerAddr(DWORD addr);
HANDLE _OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
HANDLE _GetProcessHeap();

#endif
