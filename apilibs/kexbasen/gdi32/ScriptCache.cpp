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

#include "ScriptCache.h"

ScriptCache ScriptCache::instance;

ScriptCache::ScriptCache()
{
	InitializeCriticalSection(&cs);
}

ScriptCache::~ScriptCache()
{
	DeleteCriticalSection(&cs);
}

SCRIPT_CACHE ScriptCache::GetCache(FONTUID hFont)
{	
	list<FONTCACHE>::const_iterator it;
	for (it = cache.begin(); it != cache.end(); it++)
	{
		if (it->hFont == hFont)
			return it->cache;
	}
	return NULL;
}

void ScriptCache::SetCache(FONTUID hFont,SCRIPT_CACHE newcache)
{
	list<FONTCACHE>::iterator it;
	for (it = cache.begin(); it != cache.end(); it++)
	{
		if (it->hFont == hFont)
			break;
	}
	if (it == cache.end())
	{
		FONTCACHE fc;
		fc.hFont = hFont;
		fc.cache = newcache;
		cache.push_front(fc);
	}
	else
	{
		it->cache = newcache;
		cache.splice(cache.begin(), cache, it);
	}

	if (cache.size() > MAXSCRIPTCACHESIZE)
	{
		list<FONTCACHE>::reference ref = cache.back();
		ScriptFreeCache(&ref.cache);
		cache.pop_back();
	}	
}

void ScriptCache::Lock()
{
	EnterCriticalSection(&cs);
}

void ScriptCache::Unlock()
{
	LeaveCriticalSection(&cs);
}
