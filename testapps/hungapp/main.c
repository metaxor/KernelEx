/*
 *  KernelEx
 *  Copyright (C) 2013, Ley0k
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
#include "resource.h"

INT_PTR CALLBACK DialogProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_MAKEHANG:
					while(1){}
					break;

				case IDC_CLOSE:
					EndDialog(hwndDlg, 0);
					return 1;
			}
			break;
	}

	return 0;
}

DWORD __fastcall CreateHangTestWindow()
{
	return DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_HANGTEST), NULL, DialogProc);
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
	CreateHangTestWindow();

	ExitProcess(0);
	return 0;
}