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

#ifndef __ORPHANS_H
#define __ORPHANS_H

#pragma warning(disable:4530) //we don't do exception handling
#pragma warning(disable:4786) //don't care about this either
#include <list>
#include <map>

#define MAXGDIORPHANS 6	//threshold size which triggers cleanup
#define ORPHANTTL 2000	//msecs to try give for objects to live

//gdi object handle and its gdi number for consistency check
typedef struct _GDIORPHAN
{	
	HGDIOBJ hObject;
	DWORD dwNumber;
} GDIORPHAN, *PGDIORPHAN;

using namespace std;

class GdiOrphans
{
public:	
	GdiOrphans();
	~GdiOrphans();
	void RecordOrphan(HGDIOBJ hObject, DWORD dwNumber);	
	BOOL IsOrphan(HGDIOBJ hObject, DWORD dwNumber);
	BOOL ForgetOrphan(HGDIOBJ hObject);
	void RunCleanup();
private:
	map<HGDIOBJ,PGDIORPHAN> handle_map;
	list<PGDIORPHAN> handle_list;	
	CRITICAL_SECTION cs;
	void CleanupOrphan();
	DWORD dwTickLastRecord;
};

#endif