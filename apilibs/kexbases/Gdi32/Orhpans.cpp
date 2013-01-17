/*
 *  KernelEx
 *
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
#include "Orhpans.h"
#include "GdiObjects.h"
#include "kexcoresdk.h"


GdiOrphans::GdiOrphans()
{
	InitializeCriticalSection(&cs);
	dwTickLastRecord = GetTickCount();
}

GdiOrphans::~GdiOrphans()
{
	DeleteCriticalSection(&cs);
}

//returns TRUE if handle is found and it's not hijacked
BOOL GdiOrphans::IsOrphan(HGDIOBJ hObject, DWORD dwNumber)
{
	EnterCriticalSection(&cs);
	BOOL ret;
	map<HGDIOBJ,PGDIORPHAN>::iterator it = handle_map.find(hObject);
	if (it == handle_map.end())
		ret = FALSE;
	else
		ret = (it->second->dwNumber == dwNumber) ? TRUE : FALSE;
	LeaveCriticalSection(&cs);
	return ret;
}

//removes handle, returns TRUE if it was there
BOOL GdiOrphans::ForgetOrphan(HGDIOBJ hObject)
{
	EnterCriticalSection(&cs);
	BOOL ret;
	map<HGDIOBJ,PGDIORPHAN>::iterator it = handle_map.find(hObject);
	if (it == handle_map.end())
		ret = FALSE;
	else
	{
		delete it->second;
		handle_list.remove(it->second);
		handle_map.erase(it);		
		ret = TRUE;
	}
	LeaveCriticalSection(&cs);
	return ret;
}

//adds handle into our records if it's not already there
void GdiOrphans::RecordOrphan(HGDIOBJ hObject, DWORD dwNumber)
{	
	EnterCriticalSection(&cs);
	map<HGDIOBJ,PGDIORPHAN>::iterator it = handle_map.find(hObject);
	if (it == handle_map.end()) //not found
	{
		PGDIORPHAN neworphan = new GDIORPHAN;
		if (neworphan)
		{
			neworphan->hObject = hObject;
			neworphan->dwNumber = dwNumber;
			handle_map.insert(pair<HGDIOBJ,PGDIORPHAN>(hObject,neworphan));
			handle_list.push_back(neworphan);
			if (handle_map.size() >= MAXGDIORPHANS)
			{
				DBGPRINTF(("Too many handles (%d), cleaning up!\n",handle_map.size()));
				CleanupOrphan();
			}
		}
	}
	else //found
	{
		it->second->dwNumber = dwNumber;
	}
	LeaveCriticalSection(&cs);
	dwTickLastRecord = GetTickCount();
}

//finds one handle which is hijacked or unselected and removes it
void GdiOrphans::CleanupOrphan()
{	
	int cleanup = 0;
	list<PGDIORPHAN>::iterator it;
	for (it = handle_list.begin(); it != handle_list.end(); it++)
	{
		GrabWin16Lock();
		PGDIORPHAN orphan = *it;		
		PGDIOBJ16 obj = GetGDIObjectPtr(orphan->hObject);
		//handle somehow got destroyed without us knowing?
		if (!obj || obj->dwNumber != orphan->dwNumber)
		{
			DBGPRINTF(("WARNING: Handle %p invalidated!!!",orphan->hObject));
			cleanup = 1;
		}
		else //was object unselected?
		{			
			if ( (obj->wType == GDI_TYPE_FONT || obj->wType == GDI_TYPE_PEN)
				  && obj->wRefCount < GDI_REFCOUNT_ONCE )
			{
				DBGPRINTF(("Cleaning up unselected deleted %s %p\n",
					obj->wType == GDI_TYPE_FONT ? "font":"pen",orphan->hObject));
				cleanup = 2;
			}
			if ( obj->wType == GDI_TYPE_BITMAP && ((PBITMAPOBJ16)obj)->wSelCount == 0 )
			{
				DBGPRINTF(("Cleaning up unselected deleted bitmap %p\n",orphan->hObject));
				cleanup = 2;
			}
		}
		ReleaseWin16Lock();
		if (cleanup)
		{
			if (cleanup == 2)
				DeleteObject(orphan->hObject);
			handle_list.erase(it);
			handle_map.erase(orphan->hObject);
			delete orphan;
			break;
		}
	}	
}

//public version of cleanup
void GdiOrphans::RunCleanup()
{
	if (GetTickCount() - dwTickLastRecord < ORPHANTTL) return;
	EnterCriticalSection(&cs);
	CleanupOrphan();
	LeaveCriticalSection(&cs);
	//don't run cleanup too often either
	dwTickLastRecord = GetTickCount();
}
