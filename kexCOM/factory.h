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

#ifndef __FACTORY_H
#define __FACTORY_H

#include <objbase.h>

class CFactory : public IClassFactory
{
public:
	// Constructor
	CFactory(IClassFactory* prevCF);
	// Destructor
	~CFactory();

	// IUnknown
	STDMETHODIMP QueryInterface(const IID& iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// Interface IClassFactory
	STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv);
	STDMETHODIMP LockServer(BOOL bLock);

private:
	long m_cRef;
	IClassFactory* m_prevCF;
};

#endif
