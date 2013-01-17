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

#include <malloc.h>
#include "shelllink.h"
#include "server.h"


// Constructor
CShellLink::CShellLink(IUnknown *prevUnk)
{
    InterlockedIncrement(&g_LockCount);
    m_cRef = 1;
   	m_Unknown = prevUnk;
	m_ShellLinkA = NULL;
	m_PersistFile = NULL;
	m_PersistStream = NULL;
	m_ShellExtInit = NULL;
	m_ContextMenu = NULL;
	m_ContextMenu2 = NULL;
	m_DropTarget = NULL;
	m_ExtractIconA = NULL;
}


// Destructor
CShellLink::~CShellLink()
{
    InterlockedDecrement(&g_LockCount);
	if (m_ShellLinkA)
		m_ShellLinkA->Release();
	if (m_PersistFile)
		m_PersistFile->Release();
    if (m_PersistStream)
		m_PersistStream->Release();
    if (m_ShellExtInit)
		m_ShellExtInit->Release();
	if (m_ContextMenu)
		m_ContextMenu->Release();
	if (m_ContextMenu2)
		m_ContextMenu2->Release();
	if (m_DropTarget)
		m_DropTarget->Release();
	if (m_ExtractIconA)
		m_ExtractIconA->Release();
	m_Unknown->Release();
}


// IUnknown
HRESULT STDMETHODCALLTYPE CShellLink::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	HRESULT hr = S_OK;

	if (riid == IID_IUnknown)
	{
		if (!m_ShellLinkA)
			hr = m_Unknown->QueryInterface(IID_IShellLinkA, (void**) &m_ShellLinkA);
		*ppvObject = static_cast<IUnknown*>(static_cast<IShellLinkA*>(this));
	}
	else if (riid == IID_IShellLinkA)
	{
		if (!m_ShellLinkA)
			hr = m_Unknown->QueryInterface(IID_IShellLinkA, (void**) &m_ShellLinkA);
		*ppvObject = static_cast<IShellLinkA*>(this);
	}
	else if (riid == IID_IShellLinkW)
	{
		if (!m_ShellLinkA)
			hr = m_Unknown->QueryInterface(IID_IShellLinkA, (void**) &m_ShellLinkA);
		*ppvObject = static_cast<IShellLinkW*>(this);
	}
	else if (riid == IID_IPersistFile)
	{
		if (!m_PersistFile)
			hr = m_Unknown->QueryInterface(IID_IPersistFile, (void**) &m_PersistFile);
		*ppvObject = static_cast<IPersistFile*>(this);
	}
	else if (riid == IID_IPersistStream)
	{
		if (!m_PersistStream)
			hr = m_Unknown->QueryInterface(IID_IPersistStream, (void**) &m_PersistStream);
		*ppvObject = static_cast<IPersistStream*>(this);
	}
	else if (riid == IID_IShellExtInit)
	{
		if (!m_ShellExtInit)
			hr = m_Unknown->QueryInterface(IID_IShellExtInit, (void**) &m_ShellExtInit);
		*ppvObject = static_cast<IShellExtInit*>(this);
	}
	else if (riid == IID_IContextMenu)
	{
		if (!m_ContextMenu)
			hr = m_Unknown->QueryInterface(IID_IContextMenu, (void**) &m_ContextMenu);
		*ppvObject = static_cast<IContextMenu*>(this);
	}
	else if (riid == IID_IContextMenu2)
	{
		if (!m_ContextMenu2)
			hr = m_Unknown->QueryInterface(IID_IContextMenu2, (void**) &m_ContextMenu2);
		*ppvObject = static_cast<IContextMenu2*>(this);
	}
	else if (riid == IID_IDropTarget)
	{
		if (!m_DropTarget)
			hr = m_Unknown->QueryInterface(IID_IDropTarget, (void**) &m_DropTarget);
		*ppvObject = static_cast<IDropTarget*>(this);
	}
	else if (riid == IID_IExtractIconA)
	{
		if (!m_ExtractIconA)
			hr = m_Unknown->QueryInterface(IID_IExtractIconA, (void**) &m_ExtractIconA);
		*ppvObject = static_cast<IExtractIconA*>(this);
	}
	else if (riid == IID_IExtractIconW)
	{
		if (!m_ExtractIconA)
			hr = m_Unknown->QueryInterface(IID_IExtractIconA, (void**) &m_ExtractIconA);
		*ppvObject = static_cast<IExtractIconW*>(this);
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	if (SUCCEEDED(hr))
		AddRef();
	else
		*ppvObject = NULL;

	return hr;
}

ULONG STDMETHODCALLTYPE CShellLink::AddRef( void)
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE CShellLink::Release( void)
{
	if (InterlockedDecrement(&m_cRef) == 0)
	{
		delete this;
		return 0;
	}
	return m_cRef;
}


// IShellLinkA
HRESULT STDMETHODCALLTYPE CShellLink::GetPath( 
	/* [size_is][out] */ LPSTR pszFile,
	/* [in] */ int cch,
	/* [full][out][in] */ WIN32_FIND_DATAA *pfd,
	/* [in] */ DWORD fFlags)
{
	return m_ShellLinkA->GetPath(pszFile, cch, pfd, fFlags);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetIDList( 
	/* [out] */ LPITEMIDLIST *ppidl)
{
	return m_ShellLinkA->GetIDList(ppidl);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetIDList( 
	/* [in] */ LPCITEMIDLIST pidl)
{
	return m_ShellLinkA->SetIDList(pidl);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetDescription( 
	/* [size_is][out] */ LPSTR pszName,
	/* [in] */ int cch)
{
	return m_ShellLinkA->GetDescription(pszName, cch);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetDescription( 
	/* [in] */ LPCSTR pszName)
{
	return m_ShellLinkA->SetDescription(pszName);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetWorkingDirectory( 
	/* [size_is][out] */ LPSTR pszDir,
	/* [in] */ int cch)
{
	return m_ShellLinkA->GetWorkingDirectory(pszDir, cch);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetWorkingDirectory( 
	/* [in] */ LPCSTR pszDir)
{
	return m_ShellLinkA->SetWorkingDirectory(pszDir);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetArguments( 
	/* [size_is][out] */ LPSTR pszArgs,
	/* [in] */ int cch)
{
	return m_ShellLinkA->GetArguments(pszArgs, cch);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetArguments( 
	/* [in] */ LPCSTR pszArgs)
{
	return m_ShellLinkA->SetArguments(pszArgs);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetHotkey( 
	/* [out] */ WORD *pwHotkey)
{
	return m_ShellLinkA->GetHotkey(pwHotkey);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetHotkey( 
	/* [in] */ WORD wHotkey)
{
	return m_ShellLinkA->SetHotkey(wHotkey);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetShowCmd( 
	/* [out] */ int *piShowCmd)
{
	return m_ShellLinkA->GetShowCmd(piShowCmd);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetShowCmd( 
	/* [in] */ int iShowCmd)
{
	return m_ShellLinkA->SetShowCmd(iShowCmd);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetIconLocation( 
	/* [size_is][out] */ LPSTR pszIconPath,
	/* [in] */ int cch,
	/* [out] */ int *piIcon)
{
	return m_ShellLinkA->GetIconLocation(pszIconPath, cch, piIcon);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetIconLocation( 
	/* [in] */ LPCSTR pszIconPath,
	/* [in] */ int iIcon)
{
	return m_ShellLinkA->SetIconLocation(pszIconPath, iIcon);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetRelativePath( 
	/* [in] */ LPCSTR pszPathRel,
	/* [in] */ DWORD dwReserved)
{
	return m_ShellLinkA->SetRelativePath(pszPathRel, dwReserved);
}

HRESULT STDMETHODCALLTYPE CShellLink::Resolve( 
	/* [in] */ HWND hwnd,
	/* [in] */ DWORD fFlags)
{
	return m_ShellLinkA->Resolve(hwnd, fFlags);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetPath( 
	/* [in] */ LPCSTR pszFile)
{
	return m_ShellLinkA->SetPath(pszFile);
}


// IShellLinkW
HRESULT STDMETHODCALLTYPE CShellLink::GetPath( 
	/* [size_is][out] */ LPWSTR pszFile,
	/* [in] */ int cch,
	/* [full][out][in] */ WIN32_FIND_DATAW *pfd,
	/* [in] */ DWORD fFlags)
{
	HRESULT hr;
	LPSTR pszFileA;
	WIN32_FIND_DATAA fdA;

	pszFileA = (LPSTR) alloca(cch);
	hr = m_ShellLinkA->GetPath(pszFileA, cch, &fdA, fFlags);
	if (FAILED(hr))
		return hr;
	MultiByteToWideChar(CP_ACP, 0, pszFileA, -1, pszFile, cch);

	if (pfd)
	{
		pfd->dwFileAttributes = fdA.dwFileAttributes;
		pfd->ftCreationTime = fdA.ftCreationTime;
		pfd->ftLastAccessTime = fdA.ftLastAccessTime;
		pfd->ftLastWriteTime = fdA.ftLastWriteTime;
		pfd->nFileSizeHigh = fdA.nFileSizeHigh;
		pfd->nFileSizeLow = fdA.nFileSizeLow;
		pfd->dwReserved0 = fdA.dwReserved0;
		pfd->dwReserved1 = fdA.dwReserved1;
		MultiByteToWideChar(CP_ACP, 0, fdA.cFileName, -1, pfd->cFileName, sizeof(pfd->cFileName));
		MultiByteToWideChar(CP_ACP, 0, fdA.cAlternateFileName, -1, pfd->cAlternateFileName, sizeof(pfd->cAlternateFileName));
	}

	return hr;
}

//HRESULT STDMETHODCALLTYPE CShellLink::GetIDList( 
//	/* [out] */ LPITEMIDLIST *ppidl)
//{
//}
//
//HRESULT STDMETHODCALLTYPE CShellLink::SetIDList( 
//	/* [in] */ LPCITEMIDLIST pidl)
//{
//}

HRESULT STDMETHODCALLTYPE CShellLink::GetDescription( 
	/* [size_is][out] */ LPWSTR pszName,
	int cch)
{
	HRESULT hr;
	LPSTR pszNameA;

	pszNameA = (LPSTR) alloca(cch);
	hr = m_ShellLinkA->GetDescription(pszNameA, cch);
	if (SUCCEEDED(hr))
		MultiByteToWideChar(CP_ACP, 0, pszNameA, -1, pszName, cch);
	return hr;
}

HRESULT STDMETHODCALLTYPE CShellLink::SetDescription( 
	/* [in] */ LPCWSTR pszName)
{
	LPSTR pszNameA;
	int lenA;
	
	lenA = (lstrlenW(pszName) + 1) * 2;
	pszNameA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszName, -1, pszNameA, lenA, NULL, NULL);
	return m_ShellLinkA->SetDescription(pszNameA);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetWorkingDirectory( 
	/* [size_is][out] */ LPWSTR pszDir,
	int cch)
{
	HRESULT hr;
	LPSTR pszDirA;

	pszDirA = (LPSTR) alloca(cch);
	hr = m_ShellLinkA->GetWorkingDirectory(pszDirA, cch);
	if (SUCCEEDED(hr))
		MultiByteToWideChar(CP_ACP, 0, pszDirA, -1, pszDir, cch);
	return hr;
}

HRESULT STDMETHODCALLTYPE CShellLink::SetWorkingDirectory( 
	/* [in] */ LPCWSTR pszDir)
{
	LPSTR pszDirA;
	int lenA;
	
	lenA = (lstrlenW(pszDir) + 1) * 2;
	pszDirA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszDir, -1, pszDirA, lenA, NULL, NULL);
	return m_ShellLinkA->SetWorkingDirectory(pszDirA);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetArguments( 
	/* [size_is][out] */ LPWSTR pszArgs,
	int cch)
{
	HRESULT hr;
	LPSTR pszArgsA;

	pszArgsA = (LPSTR) alloca(cch);
	hr = m_ShellLinkA->GetArguments(pszArgsA, cch);
	if (SUCCEEDED(hr))
		MultiByteToWideChar(CP_ACP, 0, pszArgsA, -1, pszArgs, cch);
	return hr;
}

HRESULT STDMETHODCALLTYPE CShellLink::SetArguments( 
	/* [in] */ LPCWSTR pszArgs)
{
	LPSTR pszArgsA;
	int lenA;
	
	lenA = (lstrlenW(pszArgs) + 1) * 2;
	pszArgsA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszArgs, -1, pszArgsA, lenA, NULL, NULL);
	return m_ShellLinkA->SetArguments(pszArgsA);
}

//HRESULT STDMETHODCALLTYPE CShellLink::GetHotkey( 
//	/* [out] */ WORD *pwHotkey)
//{
//}
//
//HRESULT STDMETHODCALLTYPE CShellLink::SetHotkey( 
//	/* [in] */ WORD wHotkey)
//{
//}
//
//HRESULT STDMETHODCALLTYPE CShellLink::GetShowCmd( 
//	/* [out] */ int *piShowCmd)
//{
//}
//
//HRESULT STDMETHODCALLTYPE CShellLink::SetShowCmd( 
//	/* [in] */ int iShowCmd)
//{
//}

HRESULT STDMETHODCALLTYPE CShellLink::GetIconLocation( 
	/* [size_is][out] */ LPWSTR pszIconPath,
	/* [in] */ int cch,
	/* [out] */ int *piIcon)
{
	HRESULT hr;
	LPSTR pszIconPathA;

	pszIconPathA = (LPSTR) alloca(cch);
	hr = m_ShellLinkA->GetIconLocation(pszIconPathA, cch, piIcon);
	if (SUCCEEDED(hr))
		MultiByteToWideChar(CP_ACP, 0, pszIconPathA, -1, pszIconPath, cch);
	return hr;
}

HRESULT STDMETHODCALLTYPE CShellLink::SetIconLocation( 
	/* [in] */ LPCWSTR pszIconPath,
	/* [in] */ int iIcon)
{
	LPSTR pszIconPathA;
	int lenA;
	
	lenA = (lstrlenW(pszIconPath) + 1) * 2;
	pszIconPathA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszIconPath, -1, pszIconPathA, lenA, NULL, NULL);
	return m_ShellLinkA->SetIconLocation(pszIconPathA, iIcon);
}

HRESULT STDMETHODCALLTYPE CShellLink::SetRelativePath( 
	/* [in] */ LPCWSTR pszPathRel,
	/* [in] */ DWORD dwReserved)
{
	LPSTR pszPathRelA;
	int lenA;
	
	lenA = (lstrlenW(pszPathRel) + 1) * 2;
	pszPathRelA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszPathRel, -1, pszPathRelA, lenA, NULL, NULL);
	return m_ShellLinkA->SetRelativePath(pszPathRelA, dwReserved);
}

//HRESULT STDMETHODCALLTYPE CShellLink::Resolve( 
//	/* [in] */ HWND hwnd,
//	/* [in] */ DWORD fFlags)
//{
//}

HRESULT STDMETHODCALLTYPE CShellLink::SetPath( 
	/* [in] */ LPCWSTR pszFile)
{
	LPSTR pszFileA;
	int lenA;
	
	lenA = (lstrlenW(pszFile) + 1) * 2;
	pszFileA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszFile, -1, pszFileA, lenA, NULL, NULL);
	return m_ShellLinkA->SetPath(pszFileA);
}


// IPersist
HRESULT STDMETHODCALLTYPE CShellLink::GetClassID(
/* [out] */ CLSID *pClassID)
{
	if (m_PersistFile)
		return m_PersistFile->GetClassID(pClassID);
	return m_PersistStream->GetClassID(pClassID);
}


// IPersistFile
HRESULT STDMETHODCALLTYPE CShellLink::IsDirty( void)
{
	return m_PersistFile->IsDirty();
}

HRESULT STDMETHODCALLTYPE CShellLink::Load( 
	/* [in] */ LPCOLESTR pszFileName,
	/* [in] */ DWORD dwMode)
{
	return m_PersistFile->Load(pszFileName, dwMode);
}

HRESULT STDMETHODCALLTYPE CShellLink::Save( 
	/* [unique][in] */ LPCOLESTR pszFileName,
	/* [in] */ BOOL fRemember)
{
	return m_PersistFile->Save(pszFileName, fRemember);
}

HRESULT STDMETHODCALLTYPE CShellLink::SaveCompleted( 
	/* [unique][in] */ LPCOLESTR pszFileName)
{
	return m_PersistFile->SaveCompleted(pszFileName);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetCurFile( 
	/* [out] */ LPOLESTR *ppszFileName)
{
	return m_PersistFile->GetCurFile(ppszFileName);
}


// IPersistStream
//HRESULT STDMETHODCALLTYPE CShellLink::IsDirty( void)
//{
//}

HRESULT STDMETHODCALLTYPE CShellLink::Load( 
	/* [unique][in] */ IStream *pStm)
{
	return m_PersistStream->Load(pStm);
}

HRESULT STDMETHODCALLTYPE CShellLink::Save( 
	/* [unique][in] */ IStream *pStm,
	/* [in] */ BOOL fClearDirty)
{
	return m_PersistStream->Save(pStm, fClearDirty);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetSizeMax( 
	/* [out] */ ULARGE_INTEGER *pcbSize)
{
	return m_PersistStream->GetSizeMax(pcbSize);
}


// IShellExtInit
HRESULT STDMETHODCALLTYPE CShellLink::Initialize(
	/* [in] */ LPCITEMIDLIST pidlFolder,
	/* [in] */ IDataObject *pdtobj,
	/* [in] */ HKEY hkeyProgID)
{
	return m_ShellExtInit->Initialize(pidlFolder, pdtobj, hkeyProgID);
}


// IContextMenu
HRESULT STDMETHODCALLTYPE CShellLink::QueryContextMenu(
	HMENU hmenu,
	UINT indexMenu,
	UINT idCmdFirst,
	UINT idCmdLast,
	UINT uFlags)
{
	return m_ContextMenu->QueryContextMenu(hmenu, indexMenu, idCmdFirst, idCmdLast, uFlags);
}

HRESULT STDMETHODCALLTYPE CShellLink::InvokeCommand(
	LPCMINVOKECOMMANDINFO lpici)
{
	return m_ContextMenu->InvokeCommand(lpici);
}

HRESULT STDMETHODCALLTYPE CShellLink::GetCommandString(
	UINT_PTR idCmd,
	UINT uType,
	UINT *pwReserved,
	LPSTR pszName,
	UINT cchMax)
{
	return m_ContextMenu->GetCommandString(idCmd, uType, pwReserved, pszName, cchMax);
}


// IContextMenu2
HRESULT STDMETHODCALLTYPE CShellLink::HandleMenuMsg(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
    return m_ContextMenu2->HandleMenuMsg(uMsg, wParam, lParam);
}


// IDropTarget
HRESULT STDMETHODCALLTYPE CShellLink::DragEnter( 
	/* [unique][in] */ IDataObject *pDataObj,
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD *pdwEffect)
{
    return m_DropTarget->DragEnter(pDataObj, grfKeyState, pt, pdwEffect);
}

HRESULT STDMETHODCALLTYPE CShellLink::DragOver( 
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD *pdwEffect)
{
    return m_DropTarget->DragOver(grfKeyState, pt, pdwEffect);
}

HRESULT STDMETHODCALLTYPE CShellLink::DragLeave( void)
{
    return m_DropTarget->DragLeave();
}

HRESULT STDMETHODCALLTYPE CShellLink::Drop( 
	/* [unique][in] */ IDataObject *pDataObj,
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD *pdwEffect)
{
    return m_DropTarget->Drop(pDataObj, grfKeyState, pt, pdwEffect);
}


// IExtractIconA
HRESULT STDMETHODCALLTYPE CShellLink::GetIconLocation(
	UINT uFlags,
	LPSTR szIconFile,
	UINT cchMax,
	int *piIndex,
	UINT *pwFlags)
{
    return m_ExtractIconA->GetIconLocation(uFlags, szIconFile, cchMax, piIndex, pwFlags);
}

HRESULT STDMETHODCALLTYPE CShellLink::Extract(
	LPCSTR pszFile,
	UINT nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIconSize)
{
    return m_ExtractIconA->Extract(pszFile, nIconIndex, phiconLarge, phiconSmall, nIconSize);
}


// IExtractIconW
HRESULT STDMETHODCALLTYPE CShellLink::GetIconLocation(
	UINT uFlags,
	LPWSTR szIconFile,
	UINT cchMax,
	int *piIndex,
	UINT *pwFlags)
{
   	HRESULT hr;
	LPSTR szIconFileA;

	szIconFileA = (LPSTR) alloca(cchMax);
	hr = m_ExtractIconA->GetIconLocation(uFlags, szIconFileA, cchMax, piIndex, pwFlags);
	if (SUCCEEDED(hr))
		MultiByteToWideChar(CP_ACP, 0, szIconFileA, -1, szIconFile, cchMax);
	return hr;
}

HRESULT STDMETHODCALLTYPE CShellLink::Extract(
	LPCWSTR pszFile,
	UINT nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIconSize)
{
	LPSTR pszFileA;
	int lenA;
	
	lenA = (lstrlenW(pszFile) + 1) * 2;
	pszFileA = (LPSTR) alloca(lenA);
	WideCharToMultiByte(CP_ACP, 0, pszFile, -1, pszFileA, lenA, NULL, NULL);
	return m_ExtractIconA->Extract(pszFileA, nIconIndex, phiconLarge, phiconSmall, nIconSize);
}
