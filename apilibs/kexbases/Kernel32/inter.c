/*
 *  KernelEx
 *  Copyright (C) 2009 Tihiy
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

/* MAKE_EXPORT InterlockedFlushSList_new=InterlockedFlushSList */
PSLIST_ENTRY WINAPI InterlockedFlushSList_new(PSLIST_HEADER ListHead)
{
	PSLIST_ENTRY NewEntry = NULL;

	/* UNIMPLEMENTED*/

	return NewEntry;
}

/* MAKE_EXPORT InterlockedPopEntrySList_new=InterlockedPopEntrySList */
PSLIST_ENTRY WINAPI InterlockedPopEntrySList_new(PSLIST_HEADER ListHead)
{
	PSLIST_ENTRY NewEntry = NULL;

	if(ListHead->Next.Next)
	{
		NewEntry = ListHead->Next.Next;
		ListHead->Next.Next = NewEntry->Next;
	}

	return NewEntry;
}

/* MAKE_EXPORT InitializeSListHead_new=InitializeSListHead */
void WINAPI InitializeSListHead_new(PSLIST_HEADER ListHead)
{
	RtlZeroMemory(ListHead, sizeof(SLIST_HEADER));
}

/* MAKE_EXPORT InterlockedPushEntrySList_new=InterlockedPushEntrySList */
PSLIST_ENTRY WINAPI InterlockedPushEntrySList_new(PSLIST_HEADER ListHead, PSLIST_ENTRY ListEntry)
{
	PVOID PrevValue;

	do
	{
		PrevValue = ListHead->Next.Next;
		ListEntry->Next = (PSINGLE_LIST_ENTRY)PrevValue;
	}
	while (InterlockedCompareExchangePointer((PVOID volatile*)&ListHead->Next.Next,
											ListEntry,
											PrevValue) != PrevValue);

	return (PSLIST_ENTRY)PrevValue;
}

/* MAKE_EXPORT QueryDepthSList_new=QueryDepthSList */
USHORT WINAPI QueryDepthSList_new(IN PSLIST_HEADER ListHead)
{
	return (USHORT)(ListHead->Alignment & 0xffff);
}