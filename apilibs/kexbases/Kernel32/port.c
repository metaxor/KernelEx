/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy
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
#include "kexcoresdk.h"

/*
 *	Note that only get-post functionality is rougly simulated;
 *	no actual I/O completion notification can be done.
 *
 *	Also handles don't get proper cleanup!
 */

#define PORT_SIGNATURE 0xABADC0FE
#define MAX_QUEUES 200

typedef struct 
{
	DWORD dwBytes;
	ULONG_PTR dwKey;
	LPOVERLAPPED lpOverlapped;
} COMPLET_PACKET;

typedef struct
{
	DWORD signature;
	HANDLE hEvent;
	CRITICAL_SECTION cs;
	DWORD count;
	COMPLET_PACKET packets[MAX_QUEUES];
} COMPLET_PORT, *PCOMPLET_PORT;


/* MAKE_EXPORT CreateIoCompletionPort_new=CreateIoCompletionPort */
HANDLE WINAPI CreateIoCompletionPort_new(
	HANDLE FileHandle,              // handle to file
	HANDLE ExistingCompletionPort,  // handle to I/O completion port
	ULONG_PTR CompletionKey,        // completion key
	DWORD NumberOfConcurrentThreads // number of threads to execute concurrently
)
{
	//kexDebugPrint("CreateIoCompletionPort FileHandle %p Port %p Key %p Threads %d",FileHandle,ExistingCompletionPort,CompletionKey,NumberOfConcurrentThreads);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		DBGPRINTF(("CreateIoCompletionPort is used for real (file handle %p), not supported.\n",FileHandle));
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return NULL;
	}
	//go create a handle we will hack on
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(COMPLET_PORT),NULL);
	if (hFileMap)
	{
		PCOMPLET_PORT port = (PCOMPLET_PORT)MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
		port->signature = PORT_SIGNATURE;
		port->count = 0;
		port->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection( &port->cs );
		UnmapViewOfFile(port);
	}
	return hFileMap;
}


/* MAKE_EXPORT GetQueuedCompletionStatus_new=GetQueuedCompletionStatus */
BOOL WINAPI GetQueuedCompletionStatus_new(
	HANDLE CompletionPort,       // handle to completion port
	LPDWORD lpNumberOfBytes,     // bytes transferred
	PULONG_PTR lpCompletionKey,  // file completion key
	LPOVERLAPPED *lpOverlapped,  // buffer
	DWORD dwMilliseconds         // optional timeout value
)
{
	PCOMPLET_PORT port = (PCOMPLET_PORT)MapViewOfFile(CompletionPort,FILE_MAP_ALL_ACCESS,0,0,0);
	if (!port || port->signature != PORT_SIGNATURE || !lpOverlapped || !lpNumberOfBytes || !lpCompletionKey)
	{
		UnmapViewOfFile(port);
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	//kexDebugPrint("GetQueuedCompletionStatus waiting port %p for %d ms",port,dwMilliseconds);
	DWORD status = WaitForSingleObject(port->hEvent,dwMilliseconds);
	if (status != WAIT_OBJECT_0)
	{
		//kexDebugPrint("GetQueuedCompletionStatus waiting port %p timed up!",port);
		UnmapViewOfFile(port);
		*lpOverlapped = NULL;
		SetLastError(WAIT_TIMEOUT);
		return FALSE;
	}
	EnterCriticalSection(&port->cs);
	*lpNumberOfBytes = port->packets[0].dwBytes;
	*lpCompletionKey = port->packets[0].dwKey;
	*lpOverlapped = port->packets[0].lpOverlapped;
	port->count--;
	memmove(&port->packets[0],&port->packets[1],sizeof(COMPLET_PACKET)*port->count);
	if (port->count > 0) SetEvent(port->hEvent);
	LeaveCriticalSection(&port->cs);
	UnmapViewOfFile(port);
	//kexDebugPrint("GetQueuedCompletionStatus dequeued from port %p: %p %p %p",port,*lpNumberOfBytes,*lpCompletionKey,*lpOverlapped);
	return TRUE;
}

/* MAKE_EXPORT PostQueuedCompletionStatus_new=PostQueuedCompletionStatus */
BOOL WINAPI PostQueuedCompletionStatus_new(
	HANDLE CompletionPort,            // handle to an I/O completion port
	DWORD dwNumberOfBytesTransferred, // bytes transferred
	ULONG_PTR dwCompletionKey,        // completion key 
	LPOVERLAPPED lpOverlapped         // overlapped buffer
)
{
	PCOMPLET_PORT port = (PCOMPLET_PORT)MapViewOfFile(CompletionPort,FILE_MAP_ALL_ACCESS,0,0,0);
	//kexDebugPrint("PostQueuedCompletionStatus queued from port %p: %p %p %p",port,dwNumberOfBytesTransferred,dwCompletionKey,lpOverlapped);
	if (!port || port->signature != PORT_SIGNATURE)
	{
		UnmapViewOfFile(port);
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	BOOL ret = FALSE;
	EnterCriticalSection(&port->cs);
	if (port->count < MAX_QUEUES)
	{
		DWORD last = port->count;
		port->packets[last].dwBytes = dwNumberOfBytesTransferred;
		port->packets[last].dwKey = dwCompletionKey;
		port->packets[last].lpOverlapped = lpOverlapped;
		port->count++;
		SetEvent(port->hEvent);
		ret = TRUE;
	}
	LeaveCriticalSection(&port->cs);
	UnmapViewOfFile(port);
	return ret;
}

