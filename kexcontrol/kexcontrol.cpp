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

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "kexcoresdk.h"
#include "debugproto.h"

bool kill_all_referents()
{
	bool kill = false;

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	BOOL ret = Process32First(h, &pe);
	while (ret)
	{
		if (pe.th32ProcessID != GetCurrentProcessId())
		{
			HANDLE m = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
			if (m != INVALID_HANDLE_VALUE)
			{
				MODULEENTRY32 me;
				me.dwSize = sizeof(me);
				BOOL ret = Module32First(m, &me);
				while (ret)
				{
					if (!strcmp(me.szModule, "KERNELEX.DLL"))
					{
						kill = true;
						break;
					}
					ret = Module32Next(m, &me);
				}
				CloseHandle(m);
			}
			if (kill)
			{
				kill = false;
				HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				TerminateProcess(p, 0);
				CloseHandle(p);
			}
		}

		ret = Process32Next(h, &pe);
	}

	CloseHandle(h);

	//let the processes die before proceeding
	Sleep(1000);
	return true;
}

int menu()
{
	printf("\n------------------------------------------------------------\n");
	printf("KernelEx control options:\n");
	printf("1. dump configurations\n");
	printf("2. dump imtes\n");
	printf("3. dump application settings\n");
	printf("4. flush application settings\n");
	printf("9. kill all depending processes\n");
	printf("0. exit\n");
	printf("\nyour choice: ");
	int ch;
	scanf("%d", &ch);
	printf("\n");
	return ch;
}

int main()
{
	printf("KernelEx control tool by Xeno86\n");
	printf("init return %d\n", kexInit());

	int ch;
	while ((ch = menu()) != 0)
	{
		switch(ch) 
		{
		case 1:
			kexDbgDumpConfigurations();
			break;
		case 2:
			kexDbgDumpImtes();
			break;
		case 3:
			kexDbgDumpAppSettings();
			break;
		case 4:
			kexFlushAppSettings();
			break;
		case 9:
			kill_all_referents();
			break;
		default:
			printf("Invalid option!\n");
		}
	}

	printf("uninit return %d\n", kexUninit());

	return 0;
}
