/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

/** KernelEx Core SDK. */


#ifndef __KEXCORESDK_H
#define __KEXCORESDK_H

#include "kstructs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef KEXCORE_EXPORTS
#define _KEXCOREIMP __declspec(dllexport)
#else
#define _KEXCOREIMP __declspec(dllimport)
#endif


/*****************************************************************************
 *
 * Every api library has to export the following functions:
 *
 *
 * const apilib_api_table* get_api_table()
 *
 *     Function should return pointer to api tables provided by the library.
 *     Function will be called when KernelEx Core builds new api
 *     configuration table, but only if the library hasn't been loaded yet.
 *     Named api and ordinal api tables have to be sorted in ascending order, 
 *     according to operator< rules below.
 *     After the last api table there has to be a closing NULL entry
 *     (all fields set to zeros).
 *     Multiple apis with same names and ordinal numbers are allowed in single
 *     api library. However only one api with given name or ordinal will be 
 *     included in single api configuration. It is possible to specify which
 *     api should be used in core.ini configuration file.
 *
 *****************************************************************************/


/** Convenience macro for defining apis. */
#define DECL_API(name_ord,func) { name_ord, (unsigned long) func }
/** Convenience macro for defining tables. */
#define DECL_TAB(lib,named,unnamed) { lib, named, sizeof(named) / sizeof(apilib_named_api), unnamed, sizeof(unnamed) / sizeof(apilib_unnamed_api) }


/** apilib_named_api - named api structure. */
typedef struct _apilib_named_api
{
	const char*    name; /**< Api name. */
	unsigned long  addr; /**< Api address. */
} apilib_named_api;


/** apilib_unnamed_api - unnamed api structure. */
typedef struct _apilib_unnamed_api
{
	unsigned short ord;  /**< Api ordinal number. */
	unsigned long  addr; /**< Api address. */
} apilib_unnamed_api;


/** apilib_api_table - api table structure. */
typedef struct _apilib_api_table
{
	const char*               target_library;     /**< DLL library for which the apis are defined. */
	const apilib_named_api*   named_apis;         /**< Pointer to table of named apis. */
	int                       named_apis_count;   /**< Number of named apis in the table. */
	const apilib_unnamed_api* ordinal_apis;       /**< Pointer to table of unnamed apis. */
	int                       ordinal_apis_count; /**< Number of unnamed apis in the table. */
} apilib_api_table;


/** Type definition for get_api_table() function. */
typedef const apilib_api_table* (* fgat_t)();

/** kexAllocObject - allocate an object in shared memory
 * @param Size - size of the object
 * @return a pointer to the object located in shared memory
 */
_KEXCOREIMP PVOID kexAllocObject(size_t size);

/** kexReAllocObject - (re)allocate an object in shared memory
 * @param Object - Previous object
 * @param Size - size of the object
 * @return a pointer to the reallocated object located in shared memory, the new object keeps
 * the previous object's content
 */
_KEXCOREIMP PVOID kexReAllocObject(PVOID Object, size_t size);

/** kexAllocObjectName - allocate a shared name so that it can be found by kexOpenObjectByName
 * actually, names are allocated internally in the KERNEL32.DLL's names list
 * @param Object - object will be linked to the name
 * @param lpName - name of the object
 * @return actually unknown
 */
_KEXCOREIMP LPSTR kexAllocObjectName(PVOID Object, LPCSTR lpName);

/** kexFreeObject - free an object from shared memory
 * @param Object - pointer to the object
 * @return TRUE if successful, otherwise return FALSE
 */
_KEXCOREIMP VOID kexFreeObject(PVOID Object);

/** kexAllocHandle - alloc an handle for the specified process
 * @param TargetProcess - Target process to allocate the handle, NULL means the current process
 * @param Object - should contains type (offset 0x00), a ref (offset 0x02), object must be located at shared memory
 * (kexAllocObject return a pointer to shared memory), otherwise other process will have problems accessing to this object
 * @param dwDesiredAccess - desired access to the object, can have HF_INHERIT
 * if the handle can be inherited by child processes
 * @return Handle to the object
 */
_KEXCOREIMP HANDLE kexAllocHandle(PVOID TargetProcess, PVOID Object, DWORD dwDesiredAccess);

/** kexCreateRemoteThread - Create a thread on a process
 * @param Process - Pointer to a process database structure
 * @param dwStackSize - Stack size
 * @param lpStartAddress - Specify where the new thread's execution will start
 * @param lpParameter - Parameter of the lpStartAddress function
 * @param dwCreationFlags - Creation flags
 * @return Pointer to the new thread database
 *
 */
_KEXCOREIMP PTDB98 kexCreateRemoteThread(PPDB98 Process, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags);

/** kexDereferenceObject - Dereference an object that as been referenced by any handle opening/allocation functions
 * @param Object - Pointer to a valid object to dereference
 * @return TRUE if successful, FALSE otherwise
 */
_KEXCOREIMP BOOL kexDereferenceObject(PVOID Object);

/** kexFindObjectHandle - find an handle associated to the object
 * @param Process - process from which resides the object
 * @param Object - pointer to a valid K32Object
 * @param ObjectType - refers to kstructs.h for the types
 * @param Handle - pointer to a variable that recieves the handle
 * @return TRUE if the object was found, FALSE otherwise
 */
_KEXCOREIMP BOOL kexFindObjectHandle(PVOID Process, PVOID Object, WORD ObjectType, PHANDLE Handle);

/** kexEnumProcesses - enumerates processes and store them in an array
 * @param pProcessIds - pointer to an array that receive process ids, you can specify NULL if you want
 * to get the number of process
 * @param cb - size of the pProcessIds array in bytes
 * @param pBytesReturned (optional) - number of bytes returned in the pProcessIds array
 */
_KEXCOREIMP BOOL kexEnumProcesses(DWORD *pProcessIds, DWORD cb, DWORD *pBytesReturned);

/** kexEnumThreads - enumerates threads and store them in an array
 * @param dwProcessId (optional) - specify the process to enumerate threads
 * @param pThreadIds - pointer to an array that receive thread ids
 * @param cb - size of the array
 * @param pBytesReturned (optional) - Number of bytes returned in the pThreadIds array
 *
 */
_KEXCOREIMP BOOL kexEnumThreads(DWORD dwProcessId, DWORD *pThreadIds, DWORD cb, DWORD *pBytesReturned);

/** kexGetKEXVersion - obtain KernelEx Core version
 * 
 * Returned value is KernelEx version number encoded as follows:
 *   0xHHLLRRRR, where:
 *     HH - major version number
 *     LL - minor version number
 *     RRRR - revision number
 */ 
_KEXCOREIMP unsigned long kexGetKEXVersion();


/** KernelEx Core capability flags. */
#define KCC_DEBUG                1  /* Core compiled with debug features enabled */
#define KCC_APIHOOK              2  /* Core compiled with API hook feature enabled */


/** kexGetCoreCaps - determine KernelEx Core capabilities
 *
 * @return Combination of capability flags.
 */
_KEXCOREIMP DWORD kexGetCoreCaps();

#pragma warning (disable:4002)
/** DBGPRINTF - convenience macro for including debug messages only in debugs. 
 *	TRACE - Print debug message to debugger, includes file and line number
 *	WARN - Same as TRACE, includes "WARN" at the begining of the string
 *	ERR - Same as TRACE, includes "ERR" at the begining of the string
 *	FIXME - Same as TRACE, includes "FIXME" at the begining of the string
 * 
 * Sample usage: DBGPRINTF(("This is a test %d %s\n", 1, "ttt"));
 */
#ifndef KEXCORE_EXPORTS
#ifndef _DEBUG
#define DBGPRINTF(x)		do { } while(0)
#define TRACE(fmt, args)	do { } while(0)
#define TRACE_OUT(x)		do { } while(0)
#define WARN(fmt, args)		do { } while(0)
#define WARN_OUT(x)			do { } while(0)
#define ERR(fmt, args)		do { } while(0)
#define ERR_OUT(x)			do { } while(0)
#define FIXME(fmt, args)	do { } while(0)
#define FIXME_OUT(x)		do { } while(0)
#define UNIMPLEMENTED		do { } while(0)
#define UNIMPLEMENTED2		do { } while(0);
#else
#define DBGPRINTF(x)		kexDebugPrint x
#define TRACE(fmt, args)	kexDebugPrint("(%s:%d) " fmt , __FILE__ , __LINE__, args)
#define TRACE_OUT(x)		kexDebugPrint("(%s:%d) " x , __FILE__ , __LINE__)
#define WARN(fmt, args)		kexDebugPrint("(%s:%d) WARN: " fmt , __FILE__ , __LINE__, args)
#define WARN_OUT(x)			kexDebugPrint("(%s:%d) WARN: " x , __FILE__ , __LINE__)
#define ERR(fmt, args)		kexDebugPrint("(%s:%d) ERR: " fmt , __FILE__ , __LINE__, args)
#define ERR_OUT(x)			kexDebugPrint("(%s:%d) ERR: " x , __FILE__ , __LINE__)
#define FIXME(fmt, args)	kexDebugPrint("(%s:%d) FIXME: " fmt , __FILE__ , __LINE__, args)
#define FIXME_OUT(x)		kexDebugPrint("(%s:%d) FIXME: " x , __FILE__ , __LINE__)
#define UNIMPLEMENTED		kexDebugPrint("(%s:%d) WARNING: %s is UNIMPLEMENTED!\n", __FILE__, __LINE__, __FUNCTION__);
#define UNIMPLEMENTED2		kexDebugPrint("(%s:%d) WARNING: stub !\n", __FILE__, __LINE__);
#endif

#define TRACEW TRACE
#define WARNW WARN
#define ERRW ERR
#define FIXMEW FIXME
#endif


/** kexDebugPrint - output debug message
 *
 * Parameters are compatible with printf command, 
 * maximum output length is limited to 256 bytes.
 */
_KEXCOREIMP void kexDebugPrint(const char* format, ...);


/** kexGetVersion - obtain original Windows version number.
 *
 * Refer to GetVersion API documentation for parameters and output.
 */
_KEXCOREIMP DWORD kexGetVersion();


/** kexGetProcAddress - obtain original address of DLL library function.
 *
 * Refer to GetProcAddress API documentation for parameters and output.
 */
_KEXCOREIMP PROC kexGetProcAddress(HMODULE hModule, PCSTR lpProcName);

/**
 * Make the thread not being interrupted
 */
_KEXCOREIMP VOID kexGrabKrnl32Lock();
_KEXCOREIMP VOID kexGrabLocks();
_KEXCOREIMP VOID kexGrabWin16Lock();

/** kexGetHandleAccess - Get the handle access rights
 * @param hHandle - Handle
 * @return Access rights
 */
_KEXCOREIMP DWORD kexGetHandleAccess(HANDLE hHandle);

/** kexSetHandleAccess - Get the handle access rights
 * @param hHandle - Handle
 * @param dwDesiredAccess - new access right
 * @return TRUE if successful, FALSE otherwise
 */
_KEXCOREIMP BOOL kexSetHandleAccess(HANDLE hHandle, DWORD dwDesiredAccess);

/** kexGetHandleObject - Get the object pointer from an handle (in current process)
 * @param hHandle - Handle to get the object from
 * @param ObjectType - refers to kstructs.h for K32OBJ types
 * @param DWORD - unknown
 * @return a pointer to the object from the handle
 *
 */
_KEXCOREIMP PVOID kexGetHandleObject(HANDLE hHandle, WORD ObjectType, DWORD);

/** kexGetHandleObjectFromProcess - Same as kexGetHandleObject, instead this function
 * will able to get objects from another process
 * @param Process - A pointer to PPDB98
 * @return a pointer to the object from the handle
 */
_KEXCOREIMP PVOID kexGetHandleObjectFromProcess(PVOID Process, HANDLE hHandle, WORD ObjectType, DWORD);

/** kexGetProcess - get the process database from a process id
 * @param dwProcessId to get the pointer
 * @return a pointer to the process database
 */
_KEXCOREIMP PVOID kexGetProcess(DWORD dwProcessId);

/** kexGetThread - get the thread database from a thread id
 * @param dwThreadId to get the pointer
 * @return a pointer to the thread database
 */
_KEXCOREIMP PVOID kexGetThread(DWORD dwThreadId);

/** kexGetProcessName - Get the process's exe module name
 * @param dwProcessId to get the exe name
 * @param buffer - pointer to a buffer that receives the process's exe name
 * @return TRUE if found, FALSE otherwise
 */
_KEXCOREIMP BOOL kexGetProcessName(DWORD dwProcessId, char *buffer);

/** kexOpenObjectByName - open an object by name, this functions searches the name through the system
 * and check if the name match, it check it the object type also match
 * @param lpName - Name of the object
 * @param ObjectType - Object type (types are defined in kstructs.h)
 * @param dwDesiredAccess - can have HF_INHERIT if the handle can be inherited by child processes
 * @return Handle to the object found or INVALID_HANDLE_VALUE if not found
 *
 */
_KEXCOREIMP HANDLE kexOpenObjectByName(LPCSTR lpName, WORD ObjectType, DWORD dwDesiredAccess);


/** kexOpenThread - open thread object.
 *	
 * Refer to OpenThread API documentation for parameters and output.
 */
_KEXCOREIMP HANDLE kexOpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);

_KEXCOREIMP VOID kexReleaseKrnl32Lock();
_KEXCOREIMP VOID kexReleaseLocks();
_KEXCOREIMP VOID kexReleaseWin16Lock();


/** kexAreExtensionsEnabled - checks if API extensions are enabled for current process.
 *
 * @return TRUE if extensions are enabled, FALSE otherwise.
 */
_KEXCOREIMP BOOL kexAreExtensionsEnabled();


/** KernelEx resolver flags. */
#define KRF_KEX_DISABLE          1  /* disable KernelEx API extensions for this module */
#define KRF_OVERRIDE_PROC_MOD    2  /* use same configuration and flags for all modules in a process */
#define KRF_HOOK_APIS            8  /* enable API tracing */
#define KRF_NO_INHERIT          16  /* don't inherit configuration and flags to child processes */
#define KRF_VALID_FLAG         128  /* denotes that flags field is valid */


/** kexGetModuleSettings - Retrieve per module settings.
 *	
 * @param module Module path.
 * @param conf_name Receives configuration name, buffer has to be at least 256 bytes long.
 * @param mod_flags Receives resolver flags.
 */
_KEXCOREIMP void kexGetModuleSettings(const char* module, 
                                      char* conf_name, DWORD* mod_flags);


/** kexSetModuleSettings - Set per module settings.
 *
 * @param module Module path.
 * @param conf_name Configuration name to be set for the module.
 * @param mod_flags Resolver flags to be set for the module.
 */
_KEXCOREIMP void kexSetModuleSettings(const char* module, 
                                      const char* conf_name, DWORD mod_flags);


/** kexResetModuleSettings - Reset per module settings to defaults.
 *
 * @param module Module path.
 */
_KEXCOREIMP void kexResetModuleSettings(const char* module);

/** kexInit - Initialize KernelEx*/
_KEXCOREIMP int kexInit(void);

/** kexDbgDumpConfigurations - Dump KernelEx configuration */
_KEXCOREIMP void kexDbgDumpConfigurations(void);

/** kexDbgDumpImtes - Dump KernelEx imtes */
_KEXCOREIMP void kexDbgDumpImtes(void);

/** kexDbgDumpAppSettings - Dump module settings*/
_KEXCOREIMP void kexDbgDumpAppSettings(void);

/** kexFlushAppSettings - Reloads all module settings from registy. */
_KEXCOREIMP void kexFlushAppSettings(void);

/** kexUninit - Unload KernelEx*/
_KEXCOREIMP int kexUninit(void);

/** kexPsAllocIndex - Reserve process storage entry. 
 *
 * @return Index to access process storage.
 */
_KEXCOREIMP int kexPsAllocIndex(void);


/** kexPsGetValue - Get process value associated with tag. 
 *
 * @param index Index allocated with kexPsAllocIndex.
 */
_KEXCOREIMP void* kexPsGetValue(int index);


/** kexPsSetValue - Set process value associated with tag. 
 *
 * @param index Index allocated with kexPsAllocIndex.
 * @param value Any data, pass zero to free storage associated with tag.
 */
_KEXCOREIMP int kexPsSetValue(int index, void* value);


/** kexGetK32Lock - obtain pointer to Kernel32 lock object.
 *
 * @return Pointer to Kernel32 lock object.
 */
_KEXCOREIMP void* kexGetK32Lock();

/** kexGetKernelExDirectory - get the KernelEx installation directory
 * @param lpBuffer - pointer to a buffer that will receive the directory
 * @param uSize - size of the buffer in bytes
 * @return Length of the string copied to the buffer
 */
_KEXCOREIMP UINT kexGetKernelExDirectory(LPSTR lpBuffer, UINT uSize);

/** kexErrorCodeToString
 * @param ErrorCode - error code that will be converted to string
 * @param lpBuffer - pointer to a variable that will receive the address
 * of the converted error code string
 * @return Length of the string copied to the buffer
 */
_KEXCOREIMP UINT kexErrorCodeToString(ULONG ErrorCode, LPSTR lpBuffer);


#ifdef __cplusplus
} /* extern "C" */
#endif


#ifdef __cplusplus


/** Comparison operator used to order named api entries. */
inline bool operator<(const apilib_named_api& a, const apilib_named_api& b)
{
	return strcmp(a.name, b.name) < 0;
}


/** Comparison operator used to order unnamed api entries. */
inline bool operator<(const apilib_unnamed_api& a, const apilib_unnamed_api& b)
{
	return a.ord < b.ord;
}


#endif


#endif
