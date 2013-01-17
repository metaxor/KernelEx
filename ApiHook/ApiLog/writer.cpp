/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "writer.h"

#define VA_SHARED 0x8000000

const unsigned short WM_KEXAPPENDLOG = 0x6eef;

static HANDLE hFile;
static char* shbuf;
static HWND hwnd;
static CRITICAL_SECTION windows_cs;

typedef bool (*initfn_t)();

initfn_t init_fn;
writerfn_t writer_fn;

bool init_file()
{
	char path[MAX_PATH];
	char* p;
	GetModuleFileName(NULL, path, sizeof(path));
	p = strrchr(path, '.');
	if (p) *p = '\0';
	strcat(path, ".log");
	hFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

void write_file(const char* msg)
{
	DWORD written;
	WriteFile(hFile, msg, strlen(msg), &written, NULL);
}

bool init_window()
{
	shbuf = (char*) VirtualAlloc(NULL, DEBUGMSG_MAXLEN,
			VA_SHARED | MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	hwnd = FindWindow(NULL, "KernelEx Debug Console");
	InitializeCriticalSection(&windows_cs);
	if (hwnd == NULL)
		return false;
	return true;
}
void write_window(const char* msg)
{
	EnterCriticalSection(&windows_cs);
	strcpy(shbuf, msg);
	int i = strlen(shbuf);
	if (i > 0) shbuf[i-1] = 0;
	SendMessage(hwnd, WM_KEXAPPENDLOG, 0, (LPARAM) shbuf);
	LeaveCriticalSection(&windows_cs);
}

bool init_debug()
{
	return true;
}

void write_debug(const char* msg)
{
	OutputDebugString(msg);
}

bool init_writer()
{
	char output[100];
	output[0] = 0;
	if (GetEnvironmentVariable("KEXAPILOG_OUTPUT", output, sizeof(output)))
	{
		if (!strcmp(output, "file"))
		{
			init_fn = init_file;
			writer_fn = write_file;
		}
		else if (!strcmp(output, "window"))
		{
			init_fn = init_window;
			writer_fn = write_window;
		}
		else if (!strcmp(output, "debug"))
		{
			init_fn = init_debug;
			writer_fn = write_debug;
		}
	}
	//default fallback
	if (!writer_fn)
	{
		init_fn = init_file;
		writer_fn = write_file;
	}

	return init_fn();
}
