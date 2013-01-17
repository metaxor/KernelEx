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

#include <windows.h>
#include "factory.h"
#include "server.h"
#include "sheet.h"
#include "KexLinkage.h"

CFactory::CFactory() 
{
	m_cRef = 1;
	InterlockedIncrement(&g_LockCount);
}

CFactory::~CFactory()
{
	InterlockedDecrement(&g_LockCount);
}

STDMETHODIMP CFactory::QueryInterface(const IID& iid, void** ppv)
{
	if (iid==IID_IUnknown)
		*ppv = static_cast<IUnknown*>(this);
	else if (iid==IID_IClassFactory)
		*ppv = static_cast<IClassFactory*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CFactory::AddRef() 
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CFactory::Release() 
{
	if (InterlockedDecrement(&m_cRef) == 0)
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

STDMETHODIMP CFactory::CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv)
{
	HRESULT hr;
	
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	if (!KexLinkage::instance.IsReady())
		return E_NOINTERFACE;
	
	KexShlExt* pShlExt = new KexShlExt;
	if (pShlExt == NULL)
		return E_OUTOFMEMORY;
	
	hr = pShlExt->QueryInterface(iid, ppv);
	pShlExt->Release();
	return hr;
}

STDMETHODIMP CFactory::LockServer(BOOL bLock)
{
	if (bLock)
		InterlockedIncrement(&g_LockCount); 
	else
		InterlockedDecrement(&g_LockCount);
	return S_OK;
}
