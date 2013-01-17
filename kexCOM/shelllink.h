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

#ifndef __SHELLLINK_H
#define __SHELLLINK_H

#include <shlobj.h>

class CShellLink : public IShellLinkA, 
                   public IShellLinkW,
                   public IPersistFile, 
                   public IPersistStream,
                   public IShellExtInit,
                   public IContextMenu2,
                   public IDropTarget,
                   public IExtractIconA,
                   public IExtractIconW
{
public:

    // Constructor
    CShellLink(IUnknown *prevUnk);


    // Destructor
    ~CShellLink();


    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject);
    
    ULONG STDMETHODCALLTYPE AddRef( void);
    
    ULONG STDMETHODCALLTYPE Release( void);


    // IShellLinkA
    HRESULT STDMETHODCALLTYPE GetPath( 
        /* [size_is][out] */ LPSTR pszFile,
        /* [in] */ int cch,
        /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
        /* [in] */ DWORD fFlags);
    
    HRESULT STDMETHODCALLTYPE GetIDList( 
        /* [out] */ LPITEMIDLIST *ppidl);
    
    HRESULT STDMETHODCALLTYPE SetIDList( 
        /* [in] */ LPCITEMIDLIST pidl);
    
    HRESULT STDMETHODCALLTYPE GetDescription( 
        /* [size_is][out] */ LPSTR pszName,
        /* [in] */ int cch);
    
    HRESULT STDMETHODCALLTYPE SetDescription( 
        /* [in] */ LPCSTR pszName);
    
    HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
        /* [size_is][out] */ LPSTR pszDir,
        /* [in] */ int cch);
    
    HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
        /* [in] */ LPCSTR pszDir);
    
    HRESULT STDMETHODCALLTYPE GetArguments( 
        /* [size_is][out] */ LPSTR pszArgs,
        /* [in] */ int cch);
    
    HRESULT STDMETHODCALLTYPE SetArguments( 
        /* [in] */ LPCSTR pszArgs);
    
    HRESULT STDMETHODCALLTYPE GetHotkey( 
        /* [out] */ WORD *pwHotkey);
    
    HRESULT STDMETHODCALLTYPE SetHotkey( 
        /* [in] */ WORD wHotkey);
    
    HRESULT STDMETHODCALLTYPE GetShowCmd( 
        /* [out] */ int *piShowCmd);
    
    HRESULT STDMETHODCALLTYPE SetShowCmd( 
        /* [in] */ int iShowCmd);
    
    HRESULT STDMETHODCALLTYPE GetIconLocation( 
        /* [size_is][out] */ LPSTR pszIconPath,
        /* [in] */ int cch,
        /* [out] */ int *piIcon);
    
    HRESULT STDMETHODCALLTYPE SetIconLocation( 
        /* [in] */ LPCSTR pszIconPath,
        /* [in] */ int iIcon);
    
    HRESULT STDMETHODCALLTYPE SetRelativePath( 
        /* [in] */ LPCSTR pszPathRel,
        /* [in] */ DWORD dwReserved);
    
    HRESULT STDMETHODCALLTYPE Resolve( 
        /* [in] */ HWND hwnd,
        /* [in] */ DWORD fFlags);
    
    HRESULT STDMETHODCALLTYPE SetPath( 
        /* [in] */ LPCSTR pszFile);


    // IShellLinkW
    HRESULT STDMETHODCALLTYPE GetPath( 
        /* [size_is][out] */ LPWSTR pszFile,
        /* [in] */ int cch,
        /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
        /* [in] */ DWORD fFlags);
    
//    HRESULT STDMETHODCALLTYPE GetIDList( 
//        /* [out] */ LPITEMIDLIST *ppidl);
//    
//    HRESULT STDMETHODCALLTYPE SetIDList( 
//        /* [in] */ LPCITEMIDLIST pidl);
    
    HRESULT STDMETHODCALLTYPE GetDescription( 
        /* [size_is][out] */ LPWSTR pszName,
        int cch);
    
    HRESULT STDMETHODCALLTYPE SetDescription( 
        /* [in] */ LPCWSTR pszName);
    
    HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
        /* [size_is][out] */ LPWSTR pszDir,
        int cch);
    
    HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
        /* [in] */ LPCWSTR pszDir);
    
    HRESULT STDMETHODCALLTYPE GetArguments( 
        /* [size_is][out] */ LPWSTR pszArgs,
        int cch);
    
    HRESULT STDMETHODCALLTYPE SetArguments( 
        /* [in] */ LPCWSTR pszArgs);
    
//    HRESULT STDMETHODCALLTYPE GetHotkey( 
//        /* [out] */ WORD *pwHotkey);
//    
//    HRESULT STDMETHODCALLTYPE SetHotkey( 
//        /* [in] */ WORD wHotkey);
//    
//    HRESULT STDMETHODCALLTYPE GetShowCmd( 
//        /* [out] */ int *piShowCmd);
//    
//    HRESULT STDMETHODCALLTYPE SetShowCmd( 
//        /* [in] */ int iShowCmd);
    
    HRESULT STDMETHODCALLTYPE GetIconLocation( 
        /* [size_is][out] */ LPWSTR pszIconPath,
        /* [in] */ int cch,
        /* [out] */ int *piIcon);
    
    HRESULT STDMETHODCALLTYPE SetIconLocation( 
        /* [in] */ LPCWSTR pszIconPath,
        /* [in] */ int iIcon);
    
    HRESULT STDMETHODCALLTYPE SetRelativePath( 
        /* [in] */ LPCWSTR pszPathRel,
        /* [in] */ DWORD dwReserved);
    
//    HRESULT STDMETHODCALLTYPE Resolve( 
//        /* [in] */ HWND hwnd,
//        /* [in] */ DWORD fFlags);
    
    HRESULT STDMETHODCALLTYPE SetPath( 
        /* [in] */ LPCWSTR pszFile);


    // IPersist
    HRESULT STDMETHODCALLTYPE GetClassID(
    /* [out] */ CLSID *pClassID);


    // IPersistFile
    HRESULT STDMETHODCALLTYPE IsDirty( void);
    
    HRESULT STDMETHODCALLTYPE Load( 
        /* [in] */ LPCOLESTR pszFileName,
        /* [in] */ DWORD dwMode);
    
    HRESULT STDMETHODCALLTYPE Save( 
        /* [unique][in] */ LPCOLESTR pszFileName,
        /* [in] */ BOOL fRemember);
    
    HRESULT STDMETHODCALLTYPE SaveCompleted( 
        /* [unique][in] */ LPCOLESTR pszFileName);
    
    HRESULT STDMETHODCALLTYPE GetCurFile( 
        /* [out] */ LPOLESTR *ppszFileName);


    // IPersistStream
//    HRESULT STDMETHODCALLTYPE IsDirty( void);
    
    HRESULT STDMETHODCALLTYPE Load( 
        /* [unique][in] */ IStream *pStm);
    
    HRESULT STDMETHODCALLTYPE Save( 
        /* [unique][in] */ IStream *pStm,
        /* [in] */ BOOL fClearDirty);
    
    HRESULT STDMETHODCALLTYPE GetSizeMax( 
        /* [out] */ ULARGE_INTEGER *pcbSize);


    // IShellExtInit
    HRESULT STDMETHODCALLTYPE Initialize(
        /* [in] */ LPCITEMIDLIST pidlFolder,
        /* [in] */ IDataObject *pdtobj,
        /* [in] */ HKEY hkeyProgID);


    // IContextMenu
    HRESULT STDMETHODCALLTYPE QueryContextMenu(
        HMENU hmenu,
        UINT indexMenu,
        UINT idCmdFirst,
        UINT idCmdLast,
        UINT uFlags);

    HRESULT STDMETHODCALLTYPE InvokeCommand(
        LPCMINVOKECOMMANDINFO lpici);

    HRESULT STDMETHODCALLTYPE GetCommandString(
        UINT_PTR idCmd,
        UINT uType,
        UINT *pwReserved,
        LPSTR pszName,
        UINT cchMax);


    // IContextMenu2
    HRESULT STDMETHODCALLTYPE HandleMenuMsg(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam);


    // IDropTarget
    HRESULT STDMETHODCALLTYPE DragEnter( 
        /* [unique][in] */ IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD *pdwEffect);
    
    HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD *pdwEffect);
    
    HRESULT STDMETHODCALLTYPE DragLeave( void);
    
    HRESULT STDMETHODCALLTYPE Drop( 
        /* [unique][in] */ IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD *pdwEffect);


    // IExtractIconA
    HRESULT STDMETHODCALLTYPE GetIconLocation(
        UINT uFlags,
        LPSTR szIconFile,
        UINT cchMax,
        int *piIndex,
        UINT *pwFlags);

    HRESULT STDMETHODCALLTYPE Extract(
        LPCSTR pszFile,
        UINT nIconIndex,
        HICON *phiconLarge,
        HICON *phiconSmall,
        UINT nIconSize);


    // IExtractIconW
    HRESULT STDMETHODCALLTYPE GetIconLocation(
        UINT uFlags,
        LPWSTR szIconFile,
        UINT cchMax,
        int *piIndex,
        UINT *pwFlags);

    HRESULT STDMETHODCALLTYPE Extract(
        LPCWSTR pszFile,
        UINT nIconIndex,
        HICON *phiconLarge,
        HICON *phiconSmall,
        UINT nIconSize);


private:
	long m_cRef;
    IUnknown *m_Unknown;
    IShellLinkA *m_ShellLinkA; 
//    IShellLinkW *m_ShellLinkW; 
    IPersistFile *m_PersistFile; 
    IPersistStream *m_PersistStream; 
    IShellExtInit *m_ShellExtInit; 
    IContextMenu *m_ContextMenu; 
    IContextMenu2 *m_ContextMenu2;
    IDropTarget *m_DropTarget;
    IExtractIconA *m_ExtractIconA;
//    IExtractIconW *m_ExtractIconW;
};

#endif
