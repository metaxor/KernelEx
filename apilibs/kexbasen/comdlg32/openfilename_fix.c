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

#define WINVER 0x0501
#include <windows.h>

/* MAKE_EXPORT GetOpenFileNameA_fix=GetOpenFileNameA */
BOOL WINAPI GetOpenFileNameA_fix(LPOPENFILENAMEA lpofn)
{
	BOOL ret = GetOpenFileNameA(lpofn);
	if (!ret && CommDlgExtendedError() == CDERR_STRUCTSIZE && lpofn 
		&& lpofn->lStructSize == sizeof(OPENFILENAME))
	{
		lpofn->lStructSize = OPENFILENAME_SIZE_VERSION_400A;
		ret = GetOpenFileNameA(lpofn);
		lpofn->lStructSize = sizeof(OPENFILENAME);
	}
	return ret;
}

/* MAKE_EXPORT GetSaveFileNameA_fix=GetSaveFileNameA */
BOOL WINAPI GetSaveFileNameA_fix(LPOPENFILENAMEA lpofn)
{
	BOOL ret = GetSaveFileNameA(lpofn);
	if (!ret && CommDlgExtendedError() == CDERR_STRUCTSIZE && lpofn 
		&& lpofn->lStructSize == sizeof(OPENFILENAME))
	{
		lpofn->lStructSize = OPENFILENAME_SIZE_VERSION_400A;
		ret = GetSaveFileNameA(lpofn);
		lpofn->lStructSize = sizeof(OPENFILENAME);
	}
	return ret;
}
