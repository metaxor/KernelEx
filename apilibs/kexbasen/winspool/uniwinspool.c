/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy
 *  Copyright (C) 2010, Xeno86
 *
 *  Parts adapted from WINE Project.
 *  Copyright 1996 John Harvey
 *  Copyright 1998 Andreas Mohr
 *  Copyright 1999 Klaas van Gend
 *  Copyright 1999, 2000 Huw D M Davies
 *  Copyright 2001 Marcus Meissner
 *  Copyright 2005-2009 Detlef Riekenberg
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
#include <winspool.h>

static const DWORD pi_sizeof[] = {0, sizeof(PRINTER_INFO_1), sizeof(PRINTER_INFO_2),
                                     sizeof(PRINTER_INFO_3), sizeof(PRINTER_INFO_4),
                                     sizeof(PRINTER_INFO_5)};

/***********************************************************
 * DEVMODEdupWtoA
 * Creates an ansi copy of supplied devmode
 */
static LPDEVMODEW DEVMODEdupAtoW(const DEVMODEA *dmA)
{
	LPDEVMODEW dmW;
	WORD size;

	if (!dmA) return NULL;
	size = dmA->dmSize + CCHDEVICENAME +
						((dmA->dmSize > FIELD_OFFSET(DEVMODEA, dmFormName)) ? CCHFORMNAME : 0);

	dmW = (LPDEVMODEW) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + dmA->dmDriverExtra);
	if (!dmW) return NULL;

	MultiByteToWideChar(CP_ACP, 0, (LPSTR)dmA->dmDeviceName, -1,
			dmW->dmDeviceName, CCHDEVICENAME);

	if (FIELD_OFFSET(DEVMODEA, dmFormName) >= dmA->dmSize) {
		memcpy(&dmW->dmSpecVersion, &dmA->dmSpecVersion,
				dmA->dmSize - FIELD_OFFSET(DEVMODEA, dmSpecVersion));
	}
	else
	{
		memcpy(&dmW->dmSpecVersion, &dmA->dmSpecVersion,
				FIELD_OFFSET(DEVMODEA, dmFormName) - FIELD_OFFSET(DEVMODEA, dmSpecVersion));
		MultiByteToWideChar(CP_ACP, 0, (LPSTR)dmA->dmFormName, -1,
				dmW->dmFormName, CCHFORMNAME);

		memcpy(&dmW->dmLogPixels, &dmA->dmLogPixels, dmA->dmSize - FIELD_OFFSET(DEVMODEA, dmLogPixels));
	}

	dmW->dmSize = size;
	memcpy((char *)dmW + dmW->dmSize, (const char *)dmA + dmA->dmSize, dmA->dmDriverExtra);
	return dmW;
}

static void convert_printerinfo_WtoA(LPBYTE outW, LPBYTE pPrintersA,
                                       DWORD level, DWORD outlen, DWORD numentries)
{
	DWORD id = 0;
	LPWSTR ptr;
	INT len;

	len = pi_sizeof[level] * numentries;
	ptr = (LPWSTR) (outW + len);
	/* first structures */
	outlen -= len;
	/* then text in unicode (we count in wchars from now on) */
	outlen /= 2;

	/* copy the numbers of all PRINTER_INFO_* first */
	memcpy(outW, pPrintersA, len);

	while (id < numentries)
	{
		switch (level)
		{
			case 1:
			{
				PRINTER_INFO_1W * piW = (PRINTER_INFO_1W *) outW;
				PRINTER_INFO_1A * piA = (PRINTER_INFO_1A *) pPrintersA;

				if (piA->pDescription) {
					piW->pDescription = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pDescription, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pName) {
					piW->pName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pComment) {
					piW->pComment = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pComment, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				break;
			}

			case 2:
			{
				PRINTER_INFO_2W * piW = (PRINTER_INFO_2W *) outW;
				PRINTER_INFO_2A * piA = (PRINTER_INFO_2A *) pPrintersA;
				LPDEVMODEW dmW;

				if (piA->pServerName) {
					piW->pServerName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pServerName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pPrinterName) {
					piW->pPrinterName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPrinterName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pShareName) {
					piW->pShareName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pShareName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pPortName) {
					piW->pPortName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPortName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pDriverName) {
					piW->pDriverName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pDriverName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pComment) {
					piW->pComment = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pComment, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pLocation) {
					piW->pLocation = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pLocation, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}

				dmW = DEVMODEdupAtoW(piA->pDevMode);
				if (dmW) {
					/* align DEVMODEA to a DWORD boundary */
					len = (2 - ( (DWORD_PTR) ptr & 1)) & 1;
					ptr += len;
					outlen -= len;

					piW->pDevMode = (LPDEVMODEW) ptr;
					len = dmW->dmSize + dmW->dmDriverExtra;
					memcpy(ptr, dmW, len);
					HeapFree(GetProcessHeap(), 0, dmW);

					ptr += len/2+1;
					outlen -= len/2+1;
				}

				if (piA->pSepFile) {
					piW->pSepFile = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pSepFile, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pPrintProcessor) {
					piW->pPrintProcessor = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPrintProcessor, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pDatatype) {
					piW->pDatatype = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pDatatype, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pParameters) {
					piW->pParameters = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pParameters, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pSecurityDescriptor) {
					piW->pSecurityDescriptor = NULL;
				}
				break;
			}

			case 4:
			{
				PRINTER_INFO_4W * piW = (PRINTER_INFO_4W *) outW;
				PRINTER_INFO_4A * piA = (PRINTER_INFO_4A *) pPrintersA;

				if (piA->pPrinterName) {
					piW->pPrinterName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPrinterName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pServerName) {
					piW->pServerName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pServerName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				break;
			}

			case 5:
			{
				PRINTER_INFO_5W * piW = (PRINTER_INFO_5W *) outW;
				PRINTER_INFO_5A * piA = (PRINTER_INFO_5A *) pPrintersA;

				if (piA->pPrinterName) {
					piW->pPrinterName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPrinterName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				if (piA->pPortName) {
					piW->pPortName = ptr;
					len = MultiByteToWideChar(CP_ACP, 0, piA->pPortName, -1,
							ptr, outlen);
					ptr += len;
					outlen -= len;
				}
				break;
			}
		}
		pPrintersA += pi_sizeof[level];
		outW += pi_sizeof[level];
		id++;
	}
}

//AddForm - not supported
FORWARD_TO_UNICOWS(AddJobW);
FORWARD_TO_UNICOWS(AddMonitorW);
FORWARD_TO_UNICOWS(AddPortW);
//AddPrinterConnection - not supported
FORWARD_TO_UNICOWS(AddPrinterDriverW);
FORWARD_TO_UNICOWS(AddPrinterW);
FORWARD_TO_UNICOWS(AddPrintProcessorW);
FORWARD_TO_UNICOWS(AddPrintProvidorW); //lol providor
FORWARD_TO_UNICOWS(AdvancedDocumentPropertiesW);
FORWARD_TO_UNICOWS(ConfigurePortW);
//DeleteForm - not supported
FORWARD_TO_UNICOWS(DeleteMonitorW);
FORWARD_TO_UNICOWS(DeletePortW);
//DeletePrinterConnection - not supported
FORWARD_TO_UNICOWS(DeletePrinterDriverW);
FORWARD_TO_UNICOWS(DeletePrintProcessorW);
FORWARD_TO_UNICOWS(DeletePrintProvidorW);
FORWARD_TO_UNICOWS(DeviceCapabilitiesW);
FORWARD_TO_UNICOWS(DocumentPropertiesW);
//EnumForms - not supported
//EnumJobsW - not implemented
//EnumMonitorsW - not implemented
//EnumPortsW - not implemented
//EnumPrinterDriversW - not implemented

/* MAKE_EXPORT EnumPrintersW_new=EnumPrintersW */
BOOL WINAPI EnumPrintersW_new(
  DWORD Flags,
  LPWSTR NameW,
  DWORD Level,
  LPBYTE pPrinterEnumW,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
)
{
	BOOL ret;
	DWORD needed;
	DWORD returned;
	LPBYTE pPrinterEnumA;

	ALLOC_WtoA(Name);
	pPrinterEnumA = (pPrinterEnumW && cbBuf) ? (LPBYTE) HeapAlloc(GetProcessHeap(), 0, cbBuf) : NULL;
	ret = EnumPrintersA(Flags, NameA, Level, pPrinterEnumA, cbBuf, &needed, &returned);
	if (Level <= 5)
		needed = (needed - pi_sizeof[Level]) * 2 + pi_sizeof[Level];
	if (pcbNeeded)
		*pcbNeeded = needed;
	if (pcReturned)
		*pcReturned = returned;
	if (ret)
	{
		if (cbBuf >= needed)
		{
			convert_printerinfo_WtoA(pPrinterEnumW, pPrinterEnumA, Level, needed, returned);
		}
		else
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			if (pcReturned)
				*pcReturned = 0;
			ret = FALSE;
		}
	}

	HeapFree(GetProcessHeap(), 0, pPrinterEnumA);
	return ret;
}

//EnumPrintProcessorDatatypesW - not implemented
//EnumPrintProcessorsW - not implemented
//GetForm - not supported
//GetJobW - not implemented
//GetPrinterDataW - not implemented
FORWARD_TO_UNICOWS(GetPrinterDriverDirectoryW);
//GetPrinterDriverW - not implemented
//GetPrinterW - not implemented
FORWARD_TO_UNICOWS(GetPrintProcessorDirectoryW);
FORWARD_TO_UNICOWS(OpenPrinterW);
FORWARD_TO_UNICOWS(ResetPrinterW);
//SetForm - not supported
FORWARD_TO_UNICOWS(SetJobW);
//SetPrinterDataW - not implemented
FORWARD_TO_UNICOWS(SetPrinterDataW); //lol lazy bastards
FORWARD_TO_UNICOWS(SetPrinterW);
FORWARD_TO_UNICOWS(StartDocPrinterW);
