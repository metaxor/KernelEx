/*
 *  KernelEx
 *  Copyright (C) 2007, Xeno86
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

int WINAPI CommonUnimpStub(void);

#ifdef __GNUC__
#define UNIMPL_FUNC(name,params) \
	__asm__( ".text\n" \
			 ".globl _" #name "@0\n" \
			 "_" #name "_new@0:\n\t" \
			 "xor %eax, %eax\n\t" \
			 "movb $" #params ", %cl\n\t" \
			 "jmp _CommonUnimpStub@0\n\t" \
	)
#else
#define UNIMPL_FUNC(name,params) \
	int __declspec(naked) __stdcall name() \
	{ \
		__asm xor eax,eax \
		__asm mov cl, params \
		__asm jmp CommonUnimpStub \
	}
#endif

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			OSVERSIONINFO osv;
			osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osv);
			if (osv.dwMajorVersion < 5)
				return FALSE;
			DisableThreadLibraryCalls(hinstDLL);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}

UNIMPL_FUNC(PdhGetDllVersion, 1);
UNIMPL_FUNC(PdhOpenQueryA, 3);
UNIMPL_FUNC(PdhOpenQueryW, 3);
UNIMPL_FUNC(PdhAddCounterA, 4);
UNIMPL_FUNC(PdhAddCounterW, 4);
UNIMPL_FUNC(PdhRemoveCounter, 1);
UNIMPL_FUNC(PdhCollectQueryData, 1);
UNIMPL_FUNC(PdhCloseQuery, 1);
UNIMPL_FUNC(PdhGetFormattedCounterValue, 4);
UNIMPL_FUNC(PdhGetFormattedCounterArrayA, 5);
UNIMPL_FUNC(PdhGetFormattedCounterArrayW, 5);
UNIMPL_FUNC(PdhGetRawCounterValue, 3);
UNIMPL_FUNC(PdhGetRawCounterArrayA, 4);
UNIMPL_FUNC(PdhGetRawCounterArrayW, 4);
UNIMPL_FUNC(PdhCalculateCounterFromRawValue, 5);
UNIMPL_FUNC(PdhComputeCounterStatistics, 6);
UNIMPL_FUNC(PdhGetCounterInfoA, 4);
UNIMPL_FUNC(PdhGetCounterInfoW, 4);
UNIMPL_FUNC(PdhSetCounterScaleFactor, 2);
UNIMPL_FUNC(PdhConnectMachineA, 1);
UNIMPL_FUNC(PdhConnectMachineW, 1);
UNIMPL_FUNC(PdhEnumMachinesA, 3);
UNIMPL_FUNC(PdhEnumMachinesW, 3);
UNIMPL_FUNC(PdhEnumObjectsA, 6);
UNIMPL_FUNC(PdhEnumObjectsW, 6);
UNIMPL_FUNC(PdhEnumObjectItemsA, 9);
UNIMPL_FUNC(PdhEnumObjectItemsW, 9);
UNIMPL_FUNC(PdhMakeCounterPathA, 4);
UNIMPL_FUNC(PdhMakeCounterPathW, 4);
UNIMPL_FUNC(PdhParseCounterPathA, 4);
UNIMPL_FUNC(PdhParseCounterPathW, 4);
UNIMPL_FUNC(PdhParseInstanceNameA, 6);
UNIMPL_FUNC(PdhParseInstanceNameW, 6);
UNIMPL_FUNC(PdhValidatePathA, 1);
UNIMPL_FUNC(PdhValidatePathW, 1);
UNIMPL_FUNC(PdhGetDefaultPerfObjectA, 4);
UNIMPL_FUNC(PdhGetDefaultPerfObjectW, 4);
UNIMPL_FUNC(PdhGetDefaultPerfCounterA, 5);
UNIMPL_FUNC(PdhGetDefaultPerfCounterW, 5);
UNIMPL_FUNC(PdhBrowseCountersA, 1);
UNIMPL_FUNC(PdhBrowseCountersW, 1);
UNIMPL_FUNC(PdhExpandCounterPathA, 3);
UNIMPL_FUNC(PdhExpandCounterPathW, 3);
UNIMPL_FUNC(PdhLookupPerfNameByIndexA, 4);
UNIMPL_FUNC(PdhLookupPerfNameByIndexW, 4);
UNIMPL_FUNC(PdhLookupPerfIndexByNameA, 3);
UNIMPL_FUNC(PdhLookupPerfIndexByNameW, 3);
UNIMPL_FUNC(PdhOpenLogA, 7);
UNIMPL_FUNC(PdhOpenLogW, 7);
UNIMPL_FUNC(PdhUpdateLogA, 2);
UNIMPL_FUNC(PdhUpdateLogW, 2);
UNIMPL_FUNC(PdhGetLogFileSize, 2);
UNIMPL_FUNC(PdhCloseLog, 2);
UNIMPL_FUNC(PdhSelectDataSourceA, 4);
UNIMPL_FUNC(PdhSelectDataSourceW, 4);
UNIMPL_FUNC(PdhIsRealTimeQuery, 1);
UNIMPL_FUNC(PdhSetQueryTimeRange, 2);
UNIMPL_FUNC(PdhGetDataSourceTimeRangeA, 4);
UNIMPL_FUNC(PdhGetDataSourceTimeRangeW, 4);
UNIMPL_FUNC(PdhCollectQueryDataEx, 3);
UNIMPL_FUNC(PdhFormatFromRawValue, 6);
UNIMPL_FUNC(PdhGetCounterTimeBase, 2);
UNIMPL_FUNC(PdhEncodeWmiPathA, 5);
UNIMPL_FUNC(PdhEncodeWmiPathW, 5);
UNIMPL_FUNC(PdhDecodeWmiPathA, 5);
UNIMPL_FUNC(PdhDecodeWmiPathW, 5);
UNIMPL_FUNC(PdhReadRawLogRecord, 4);
UNIMPL_FUNC(PdhLogServiceCommandA, 4);
UNIMPL_FUNC(PdhLogServiceCommandW, 4);
UNIMPL_FUNC(PdhLogServiceControlA, 5);
UNIMPL_FUNC(PdhLogServiceControlW, 5);
