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

#ifndef _SHEET_H
#define _SHEET_H

#include <shlobj.h>

struct ModuleSetting
{
	char file[MAX_PATH];
	char conf[256];
	DWORD flags;
};

class KexShlExt : public IShellExtInit, 
				  public IShellPropSheetExt
{
public:
	// Constructor
	KexShlExt();

	// Destructor
	~KexShlExt();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid,LPVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef(); 	
	STDMETHODIMP_(ULONG) Release(); 

	// IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IShellPropSheetExt
    STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
    STDMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM);

protected:
	ModuleSetting* ms;
	
	bool IsPEModule(const char* path);
	bool ResolveShortcut(const char* shortcutPath, char* filePath);
	static BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT CALLBACK CallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);
	static void OnInitDialog(HWND hwnd, ModuleSetting* ms);
	static void OnApply(HWND hwnd);
	static HWND CreateTooltipWindow(HWND parent);
	static bool CreateTooltip(HWND hwndTip, HWND hDlg, int toolID, char* pText);

private:
	long m_cRef;
};

#endif // _SHEET_H
