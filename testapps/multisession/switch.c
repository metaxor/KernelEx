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

#include "main.h"

/*
	Switch to our personnal space,
	the user can do what he wants
	from his personnal space.
*/

void SwitchToSession1(void)
{
	int ret;
	TCHAR buffer[255];
	ZeroMemory(&buffer, sizeof(buffer));

	ret = SetThreadDesktop(Desktop1);
	if(!ret)
	{
		wsprintf(buffer, "Failed to set the thread's desktop (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
		return;
	}

	ret = SwitchDesktop(Desktop1);
	if(!ret)
	{
        SetThreadDesktop(Desktop0);
		wsprintf(buffer, "Switching to the desktop's Window Station has failed (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
	}
}

/*
	Let's go back to the default desktop
*/

void RevertToSession0(void)
{
	int ret;
	TCHAR buffer[255];
	ZeroMemory(&buffer, sizeof(buffer));

	ret = SetThreadDesktop(Desktop0);
	if(!ret)
	{
		wsprintf(buffer, "Failed to set the thread's desktop (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
		return;
	}

	ret = SwitchDesktop(Desktop0);
	if(!ret)
	{
        SetThreadDesktop(Desktop1);
		wsprintf(buffer, "Switching to the desktop's Window Station has failed (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
	}
}
