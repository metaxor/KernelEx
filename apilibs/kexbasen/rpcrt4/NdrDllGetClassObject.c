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

//PITA to compile alltogether right
#define CINTERFACE
#include <windows.h>
#include <RpcProxy.h>
#ifdef LOAD_TLB_AS_64BIT
#undef LOAD_TLB_AS_64BIT
#endif
#ifdef LOAD_TLB_AS_32BIT
#undef LOAD_TLB_AS_32BIT
#endif
#include <ndrtypes.h>
#include "kexcoresdk.h"

static DWORD PageProtect(LPVOID lpAddress, DWORD flNewProtect)
{
	DWORD flOldProtect;
	VirtualProtect(lpAddress,32,flNewProtect,&flOldProtect);
	return flOldProtect;
}

static WORD PatchFormatString(PFORMAT_STRING pFormat)
{
	//this header is fixed (read rpctypes.h)
	NDR_DCOM_OI2_PROC_HEADER hdr2;
	memcpy(&hdr2,pFormat,sizeof(hdr2));
	if (hdr2.Oi2Flags.HasExtensions) //uses nt5 extensions?? cut them!
	{
		hdr2.Oi2Flags.HasExtensions = FALSE;
		PNDR_PROC_HEADER_EXTS exts = (PNDR_PROC_HEADER_EXTS)(pFormat + sizeof(hdr2));
		WORD cutsize = exts->Size;		
		DWORD oldproc = PageProtect((PVOID)pFormat,PAGE_READWRITE);		
		memcpy((PVOID)((DWORD)pFormat+(DWORD)cutsize),&hdr2,sizeof(hdr2));
		PageProtect((PVOID)pFormat,oldproc);		
		return cutsize;
	}
	return 0;
}

static void CheckStubInfo(const MIDL_SERVER_INFO* pServerInfo, ULONG DispatchCount )
{	
	if (pServerInfo && pServerInfo->pStubDesc && pServerInfo->pStubDesc->Version > NDR_VERSION_2_0)
	{
		//unconst and change ndr version to 2.0 as max 9x supports
		MIDL_STUB_DESC* pStubDesc = (MIDL_STUB_DESC*)pServerInfo->pStubDesc;
		DWORD oldprot = PageProtect((PVOID)&pStubDesc->Version,PAGE_READWRITE);
		pStubDesc->Version = NDR_VERSION_2_0;
		PageProtect((PVOID)&pStubDesc->Version,oldprot);
		//correct all dispatches (0,1,2 are provided by IUnknown_Proxy)
		for (ULONG i=3;i<DispatchCount;i++)
		{
			PFORMAT_STRING pFormat = pServerInfo->ProcString + pServerInfo->FmtStringOffset[i];
			WORD offset = PatchFormatString(pFormat);			
			if (offset) //shift offsets in entry table
			{
				DBGPRINTF(("PatchFormatString: entry %d uses extensions, size %d\n",i,offset));
				oldprot = PageProtect((PVOID)&pServerInfo->FmtStringOffset[i],PAGE_READWRITE);
				*(WORD*)&pServerInfo->FmtStringOffset[i] += offset;
				PageProtect((PVOID)&pServerInfo->FmtStringOffset[i],oldprot);
			}
		}
	}
}

/* MAKE_EXPORT NdrDllGetClassObject_new=NdrDllGetClassObject */
HRESULT WINAPI NdrDllGetClassObject_new( REFCLSID rclsid, REFIID riid, void **ppv, 
												PVOID pVoidProxyFileList, const CLSID *pclsid, 
												PVOID pVoidPSFactoryBuffer)
{	
	CStdPSFactoryBuffer *pPSFactoryBuffer = (CStdPSFactoryBuffer *)pVoidPSFactoryBuffer;
	const ProxyFileInfo **pProxyFileList = (const ProxyFileInfo **)pVoidProxyFileList;
	const ProxyFileInfo **pProxyFileListWalker = pProxyFileList;
	while (pProxyFileListWalker && *pProxyFileListWalker)
	{
		const PCInterfaceStubVtblList *pstubVtblList = (*pProxyFileListWalker)->pStubVtblList;
		while (pstubVtblList && *pstubVtblList)
		{
			CheckStubInfo((*pstubVtblList)->header.pServerInfo,(*pstubVtblList)->header.DispatchTableCount);
			pstubVtblList++;
		}
		pProxyFileListWalker++;
	}
	return NdrDllGetClassObject(rclsid,riid,ppv,pProxyFileList,pclsid,pPSFactoryBuffer);
}
