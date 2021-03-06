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

#ifndef _RPCRT4_APILIST_H
#define _RPCRT4_APILIST_H

#include "kexcoresdk.h"
#include <rpc.h>

BOOL init_rpcrt4();
extern const apilib_api_table apitable_rpcrt4;

/*** AUTOGENERATED APILIST DECLARATIONS BEGIN ***/
HRESULT WINAPI NdrDllGetClassObject_new(REFCLSID rclsid, REFIID riid, void **ppv, PVOID pVoidProxyFileList, const CLSID *pclsid, PVOID pVoidPSFactoryBuffer);
RPC_STATUS RPC_ENTRY RpcStringFreeW_new(IN OUT LPWSTR * String);
RPC_STATUS RPC_ENTRY UuidFromStringW_new(IN LPWSTR StringUuidW, OUT UUID * Uuid);
RPC_STATUS RPC_ENTRY UuidToStringW_new(IN UUID * Uuid, OUT LPWSTR * StringUuid);
/*** AUTOGENERATED APILIST DECLARATIONS END ***/

#endif
