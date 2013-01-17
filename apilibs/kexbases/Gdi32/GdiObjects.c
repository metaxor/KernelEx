/*
 *  KernelEx
 *
 *  Copyright (C) 2009-2010, Tihiy
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
#include "GdiObjects.h"
#include "k32ord.h"
#include "kexcoresdk.h"
#include "Orhpans.h"

static WORD g_GDILH_addr;
static DWORD g_GdiBase;
static int orphan_class_psidx;

#define REBASEGDI(x) ((PVOID)( g_GdiBase + LOWORD((DWORD)(x)) ))
#define REBASEGDIHIGH(x) ( g_GdiBase + (DWORD)(x) ) 

BOOL InitGDIObjects(void)
{
	g_GdiBase = MapSL( LoadLibrary16("gdi") << 16 );
	g_GDILH_addr = ((PINSTANCE16)g_GdiBase)->pLocalHeap;
	orphan_class_psidx = kexPsAllocIndex();
	return (BOOL)g_GdiBase;
}

PGDIOBJ16 GetGDIObjectPtr( HGDIOBJ hgdiobj )
{
	WORD wHandle = (WORD)hgdiobj;
	if ( wHandle & 1 ) return NULL; //all gdi handles end in b10 or b00, not b01
	if ( wHandle & 3 ) //64K heap
	{
		PLHENTRY entry = (PLHENTRY)REBASEGDI( wHandle );
		if ( wHandle <= g_GDILH_addr || entry->bFlags == LHE_FREEHANDLE || entry->wBlock <= g_GDILH_addr )
			return NULL; //deleted or invalid handle
		if ( entry->bFlags & LHE_DISCARDED ) //discarded DC
		{
			if ( entry->wBlock & 3 ) return NULL; //they have to divide by 4			
			return GetGDIObjectPtr( (HGDIOBJ)entry->wBlock ); //recurse
		}
		else
		{
			return (PGDIOBJ16)REBASEGDI(entry->wBlock);
		}
	}
	else //high heap (not moveable)
	__try
	{
		if ( wHandle < 0x80 ) return NULL; //high heap handles start with 0x80
		if ( wHandle > *(WORD*)REBASEGDIHIGH( GDIHEAP32BASE + 0x70 ) ) return NULL; //max hadle value there
		DWORD* high = (DWORD*)REBASEGDIHIGH( GDIHEAP32BASE + wHandle );
		if ( *high < GDIHEAP32TOP ) return NULL; //points to invalid handle memory
		PGDIOBJ16 ret = (PGDIOBJ16)REBASEGDIHIGH( *high );
		if ( ret->wType == 0 ) return NULL; //points to invalid object memory 
		return ret;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}

static DWORD SwitchGDIObjectType( PGDIOBJ16 obj )
{
	__try
	{
		switch ( obj->wType & GDI_TYPEMASK )
		{		
			case GDI_TYPE_PEN:
				return OBJ_PEN;
			case GDI_TYPE_BRUSH:
				return OBJ_BRUSH;
			case GDI_TYPE_FONT:
				return OBJ_FONT;
			case GDI_TYPE_PAL:
				return OBJ_PAL;
			case GDI_TYPE_BITMAP:
				return OBJ_BITMAP;
			case GDI_TYPE_REGION:
				return OBJ_REGION;
			case GDI_TYPE_DC:
			{
				PDCOBJ dcobj = (PDCOBJ)obj;
				if ( dcobj->enhmetadc ) return OBJ_ENHMETADC;
				if ( dcobj->dcFlags & 1 ) return OBJ_MEMDC;
				//fall down
			}
			case GDI_TYPE_DCX:
			case GDI_TYPE_DCY:
				return OBJ_DC;
			case GDI_TYPE_METADC:
				return OBJ_METADC;
			case GDI_TYPE_ENHMETA:
				return OBJ_ENHMETAFILE;			
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//too bad
	}
	return 0;
}

/* MAKE_EXPORT GetObjectType_fix=GetObjectType */
DWORD WINAPI GetObjectType_fix( HGDIOBJ hgdiobj )
{
	//GetObjectType is rewritten in order to boost it's correctness and speed:
	//constantly throwing page/segfaults is very bad on virtual machines.			
	DWORD result = 0;
	if (hgdiobj)
	{
		PGDIOBJ16 obj;
		GrabWin16Lock();
		obj = GetGDIObjectPtr( hgdiobj );
		if ( obj )
			result = SwitchGDIObjectType( obj );
		ReleaseWin16Lock();
		if ( !obj )  //still, can be metafile selector
		{
			WORD wHandle = (WORD)hgdiobj;
			if ( (wHandle & 6) == 6 ) //test for ldt selector
			{
				LDT_ENTRY selector;
				GetThreadSelectorEntry( GetCurrentThread(), wHandle, &selector );
				if ( selector.HighWord.Bits.Type == 3 ) //read/write usermode?
					result = GetObjectType(hgdiobj); //resort to gdi32
			}
		}
	}
	if ( !result )
		SetLastError( ERROR_INVALID_HANDLE );
	return result;
}

#pragma warning (disable:4035) //xeno approves
static inline
WORD GetCurrentTDB() 
{
	__asm mov ax, fs:[0Ch]
}
#pragma warning (default:4035)

/************************************************************************
  The purpose of GdiObjects is to simulate NT GDI object rules, which
  allows deleting object handles while selected into DCs.

  To do this, we set up an array of those handles which are checked up
  and destroyed if unselected in FIFO order when some time passes or
  array (list+map) is big enough.
 ************************************************************************/

//those have to be under w16lock
static inline
GdiOrphans* GetOrphans()
{
	return (GdiOrphans*)kexPsGetValue(orphan_class_psidx);
}

static inline
GdiOrphans* CreateOrphans()
{
	GdiOrphans* orphans = GetOrphans();
	if (orphans == NULL)
	{
		orphans = new GdiOrphans;
		kexPsSetValue(orphan_class_psidx,orphans);
	}
	return orphans;
}

/* MAKE_EXPORT DeleteObject_fix=DeleteObject */
BOOL WINAPI DeleteObject_fix( HGDIOBJ hObject )
{	
	PGDIOBJ16 obj;
	GdiOrphans* orphans = NULL;
	DWORD dwNumber = 0;
	if (!hObject) return FALSE;
	GrabWin16Lock();
	obj = GetGDIObjectPtr( hObject );
	if ( !obj || !SwitchGDIObjectType(obj) )
	{
		ReleaseWin16Lock();
		return FALSE;
	}
	//check if object is selected
	if ( obj->wOwner == GetCurrentTDB() ) //not system or foreign objects
	{
		if (obj->wType == GDI_TYPE_FONT || obj->wType == GDI_TYPE_PEN )
		{
			if ( obj->wRefCount >= GDI_REFCOUNT_ONCE )
			{
				DBGPRINTF(("Attempt to delete selected %s %p\n",
					obj->wType == GDI_TYPE_FONT ? "font":"pen",hObject));
				dwNumber = obj->dwNumber;
			}
		}
		if (obj->wType == GDI_TYPE_BITMAP) //can be selected into one DC
		{
			PBITMAPOBJ16 bmpobj = (PBITMAPOBJ16)obj;
			if ( bmpobj->wSelCount )
			{
				DBGPRINTF(("Attempt to delete selected bitmap %p\n",
					hObject));
				dwNumber = obj->dwNumber;
			}
		}
		//if there is sth to record, initialize orphans class
		orphans = (dwNumber) ? CreateOrphans() : GetOrphans();
	}
	ReleaseWin16Lock();
	if (orphans)
	{
		if ( dwNumber )
			orphans->RecordOrphan(hObject,dwNumber);
		else
			if ( orphans->ForgetOrphan(hObject) )
				DBGPRINTF(("WARNING: deleted handle %p is destroyed!!!\n",hObject));
	}
	return DeleteObject( hObject );
}

/* MAKE_EXPORT SelectObject_fix=SelectObject */
HGDIOBJ WINAPI SelectObject_fix( HDC hdc, HGDIOBJ hgdiobj )
{
	//validation 9x should do
	if ( !hdc ) SetLastError(ERROR_INVALID_HANDLE);
	if ( !hdc || !hgdiobj ) return NULL;
	//if object was deleted, don't let it be reselected
	GrabWin16Lock();
	PGDIOBJ16 obj = GetGDIObjectPtr( hgdiobj );
	GdiOrphans* orphans = NULL;
	DWORD dwNumber;	
	if ( obj && obj->wOwner == GetCurrentTDB() )
	{		
		orphans = GetOrphans();
		dwNumber = obj->dwNumber;
	}
	ReleaseWin16Lock();	
	if ( orphans && orphans->IsOrphan(hgdiobj, dwNumber) )
	{
		DBGPRINTF(("WARNING: Attempt to select deleted handle %p\n",hgdiobj));
		return NULL;
	}
	return SelectObject( hdc, hgdiobj );
}


/* MAKE_EXPORT CreateDIBSection_fix=CreateDIBSection */
HBITMAP WINAPI CreateDIBSection_fix(
	HDC hdc,                 // handle to DC
	BITMAPINFO *pbmi,        // bitmap data
	UINT iUsage,             // data type indicator
	VOID **ppvBits,          // bit values
	HANDLE hSection,         // handle to file mapping object
	DWORD dwOffset           // offset to bitmap bit values
)
{
	if (pbmi && pbmi->bmiHeader.biSize == sizeof(BITMAPINFO)) //9x does not forgive
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //9x does not forget
	HBITMAP ret = CreateDIBSection(hdc,pbmi,iUsage,ppvBits,hSection,dwOffset);
	GdiOrphans* orphans = GetOrphans();
	if (orphans)
		orphans->RunCleanup();
	return ret;
}

/* MAKE_EXPORT CreateFontIndirectA_fix=CreateFontIndirectA */
HFONT WINAPI CreateFontIndirectA_fix( CONST LOGFONT* lplf )
{
	HFONT ret = CreateFontIndirectA(lplf);
	GdiOrphans* orphans = GetOrphans();
	if (orphans)
		orphans->RunCleanup();
	return ret;
}
