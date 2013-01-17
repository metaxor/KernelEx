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

#ifndef __FOLDERFIX_H
#define __FOLDERFIX_H

/* 
 * The purpose of the fix is that the Start Menu in Windows 98 
 * and Windows Me don't display any items from "All Users\Start Menu" folder.
 * Thus the fix is to redirect CSIDL_COMMON_* to approperiate CSIDL_* folders.
 *
 * Interesingly Desktop displays icons from "All Users\Desktop"...
 *
 * This fix is applied to following functions:
 * SHGetFolderPathA (both SHELL32 and SHFOLDER)
 * SHGetFolderPathW (both SHELL32 and SHFOLDER)
 * SHGetFolderLocation
 * SHGetSpecialFolderPathA
 * SHGetSpecialFolderPathW
 * SHGetSpecialFolderLocation
 */

inline int folder_fix(int nFolder)
{
	int nFolderFlags = nFolder & CSIDL_FLAG_MASK;
	nFolder &= ~CSIDL_FLAG_MASK;
	if (nFolder == CSIDL_COMMON_PROGRAMS)
		nFolder = CSIDL_PROGRAMS;
	else if (nFolder == CSIDL_COMMON_STARTMENU)
		nFolder = CSIDL_STARTMENU;
	else if (nFolder == CSIDL_COMMON_STARTUP)
		nFolder = CSIDL_STARTUP;
	else if (nFolder == CSIDL_COMMON_DESKTOPDIRECTORY)
		nFolder = CSIDL_DESKTOPDIRECTORY;
	nFolder |= nFolderFlags;
	return nFolder;
}

#endif
