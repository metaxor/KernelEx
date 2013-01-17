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

#include "common.h"

#define MF_TYPEMASK (MF_STRING | MF_OWNERDRAW | MF_BITMAP | MF_SEPARATOR)

/* MAKE_EXPORT LoadMenuIndirectA=LoadMenuIndirectW */

/* MAKE_EXPORT LoadMenuA_fix=LoadMenuA */
HMENU WINAPI LoadMenuA_fix(HINSTANCE hInstance, LPCSTR lpMenuName)
{
	//9x can't load resources from mmaped libraries (LOAD_LIBRARY_AS_DATAFILE)
	HMENU ret = NULL;
	//in case of mmaped dll the lowest bit in hInstance will be set to 1
	if ((DWORD) hInstance & 1)
	{
		HRSRC fnd = FindResource(hInstance, lpMenuName,RT_MENU);
		if (fnd)
		{
			LPMENUTEMPLATE lpMenuTemplate = LoadResource(hInstance, fnd);
			if (lpMenuTemplate)
				ret = LoadMenuIndirect(lpMenuTemplate);
		}
	}
	else
		ret = LoadMenuA(hInstance, lpMenuName);
	return ret;
}

/* MAKE_EXPORT LoadMenuW_new=LoadMenuW */
HMENU WINAPI LoadMenuW_new(HINSTANCE hInstance, LPCWSTR lpMenuNameW)
{
	LPSTR lpMenuNameA;
	STACK_WtoA(lpMenuNameW, lpMenuNameA);
	return LoadMenuA_fix(hInstance, lpMenuNameA);
}

//Append/Insert/Modify are not handled right by MSLU
/* MAKE_EXPORT InsertMenuW_new=InsertMenuW */
BOOL WINAPI InsertMenuW_new(HMENU hMenu, UINT uPosition, UINT uFlags, 
							UINT_PTR uIDNewItem, LPCWSTR lpNewItemW)
{
	LPSTR lpNewItemA = (LPSTR)lpNewItemW;
	if ((uFlags & MF_TYPEMASK) == MF_STRING)
	{
		STACK_WtoA(lpNewItemW, lpNewItemA);
	}
	return InsertMenuA(hMenu, uPosition, uFlags, uIDNewItem, lpNewItemA);	
}

/* MAKE_EXPORT AppendMenuW_new=AppendMenuW */
BOOL WINAPI AppendMenuW_new(HMENU hMenu, UINT uFlags, 
							UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
{
	return InsertMenuW_new(hMenu, -1, uFlags | MF_BYPOSITION, uIDNewItem, lpNewItem);
}

/* MAKE_EXPORT ChangeMenuW_new=ChangeMenuW */
BOOL WINAPI ChangeMenuW_new(HMENU hMenu, UINT cmd, LPCWSTR lpNewItemW,
							UINT uIDNewItem, UINT uFlags)
{
	LPSTR lpNewItemA = (LPSTR) lpNewItemW;
	if ((uFlags & MF_TYPEMASK) == MF_STRING)
	{
		STACK_WtoA(lpNewItemW, lpNewItemA);
	}
	return ChangeMenuA(hMenu, cmd, lpNewItemA, uIDNewItem, uFlags);
}

/* MAKE_EXPORT ModifyMenuW_new=ModifyMenuW */
BOOL WINAPI ModifyMenuW_new(HMENU hMenu, UINT uPosition, UINT uFlags,
							UINT_PTR uIDNewItem, LPCWSTR lpNewItemW)
{
	LPSTR lpNewItemA = (LPSTR) lpNewItemW;
	if ((uFlags & MF_TYPEMASK) == MF_STRING)
	{
		STACK_WtoA(lpNewItemW, lpNewItemA);
	}
	return ModifyMenuA(hMenu, uPosition, uFlags, uIDNewItem, lpNewItemA);
}
