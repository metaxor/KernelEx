/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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
#include "unifwd.h"
#include "kexcoresdk.h"
#include "kernel32/_kernel32_apilist.h"
#include "gdi32/_gdi32_apilist.h"
#include "user32/_user32_apilist.h"
#include "advapi32/_advapi32_apilist.h"
#include "comdlg32/_comdlg32_apilist.h"
#include "shell32/_shell32_apilist.h"
#include "rpcrt4/_rpcrt4_apilist.h"
#include "winspool/_winspool_apilist.h"
#include "shfolder/_shfolder_apilist.h"
#include "winmm/_winmm_apilist.h"
#include "ws2_32/_ws2_32_apilist.h"
#include "ole32/_ole32_apilist.h"
#include "cryptui/_cryptui_apilist.h"
#include "ntdll/_ntdll_apilist.h"
#include "netapi32/_netapi32_apilist.h"
#include "unicows/_unicows_apilist.h"
//#include "/__apilist.h"

static apilib_api_table api_table[17];

static void fill_apitable()
{
	api_table[0] = apitable_kernel32;
	api_table[1] = apitable_gdi32;
	api_table[2] = apitable_user32;
	api_table[3] = apitable_advapi32;
	api_table[4] = apitable_comdlg32;
	api_table[5] = apitable_shell32;
	api_table[6] = apitable_rpcrt4;
	api_table[7] = apitable_winspool;
	api_table[8] = apitable_shfolder;
	api_table[9] = apitable_winmm;
	api_table[10] = apitable_ws2_32;
	api_table[11] = apitable_ole32;
	api_table[12] = apitable_cryptui;
	api_table[13] = apitable_ntdll;
	api_table[14] = apitable_netapi32;
	api_table[15] = apitable_unicows;
	//last entry is null terminator
}

extern "C"
__declspec(dllexport)
const apilib_api_table* get_api_table()
{
	fill_apitable();
	return api_table;
}

static BOOL init()
{
	return common_init() && init_kernel32() && init_gdi32() && init_user32() 
		&& init_advapi32() && init_comdlg32() && init_shell32() && init_rpcrt4() 
		&& init_winspool() && init_shfolder() && init_winmm() && init_ws2_32()
		&& init_ole32() && init_cryptui() && init_ntdll() && init_netapi32()
		&& init_unicows();
}

static void uninit()
{
	uninit_kernel32();
	uninit_shell32();
}

static void detach()
{
	detach_kernel32();
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, BOOL load_static)
{
	switch (reason) 
	{
	case DLL_PROCESS_ATTACH:
		if (!init())
			return FALSE;
		break;
	case DLL_PROCESS_DETACH:
		uninit();
		break;
	case DLL_THREAD_DETACH:
		detach();
		break;
	}
	return TRUE;
}
