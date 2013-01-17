/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy, Xeno86
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

#ifndef __SCRIPTCACHE_H
#define __SCRIPTCACHE_H

#include <windows.h>
#include <usp10.h>
#pragma warning(disable:4530) //we don't do exception handling
#include <list>
#pragma warning(default:4530)

#define MAXSCRIPTCACHESIZE 10

typedef DWORD FONTUID;

typedef struct
{
	FONTUID hFont;
	SCRIPT_CACHE cache;
} FONTCACHE, *PFONTCACHE;

using namespace std;

class ScriptCache
{
public:
		static ScriptCache instance;
		~ScriptCache();
		SCRIPT_CACHE GetCache(FONTUID hFont);
		void SetCache(FONTUID hFont, SCRIPT_CACHE newcache);
		void Lock();
		void Unlock();

private:
		list<FONTCACHE> cache;
		CRITICAL_SECTION cs;

		ScriptCache();
};

#endif
