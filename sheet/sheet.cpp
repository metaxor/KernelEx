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
#include <shlwapi.h>
#include "sheet.h"
#include "server.h"
#include "resource.h"
#include "KexLinkage.h"

extern "C" int snprintf(char*, size_t, const char*, ...);

struct CTips
{
	char* _TIP_DEFAULT;
	char* _TIP_DISABLE;
	char* _TIP_COMPAT;
	char* _TIP_SYSTEM;
	char* _TIP_NOINHERIT;
	char* _TIP_OVERRIDE;
	char* _TIP_HOOK;

	CTips()
	{
		_TIP_DEFAULT = _TIP_DISABLE = _TIP_COMPAT = _TIP_SYSTEM
				= _TIP_NOINHERIT = _TIP_OVERRIDE = _TIP_HOOK = NULL;
		loaded = false;
	}

	~CTips()
	{
		if (loaded)
		{
			free(_TIP_DEFAULT);
			free(_TIP_DISABLE);
			free(_TIP_COMPAT);
			free(_TIP_SYSTEM);
			free(_TIP_NOINHERIT);
			free(_TIP_OVERRIDE);
			free(_TIP_HOOK);
		}
	}

	void load_tips()
	{
		if (loaded)
			return;
		loaded = true;
		_TIP_DEFAULT = load_string(TIP_DEFAULT);
		_TIP_DISABLE = load_string(TIP_DISABLE);
		_TIP_COMPAT = load_string(TIP_COMPAT);
		_TIP_SYSTEM = load_string(TIP_SYSTEM);
		_TIP_NOINHERIT = load_string(TIP_NOINHERIT);
		_TIP_OVERRIDE = load_string(TIP_OVERRIDE);
		_TIP_HOOK = load_string(TIP_HOOK);
	}

private:
	bool loaded;

	char* load_string(int sID)
	{
		char buf[512];
		LoadString(g_hModule, sID, buf, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';
		return strdup(buf);
	}
} tips;



KexShlExt::KexShlExt()
{
	m_cRef = 1;
	InterlockedIncrement(&g_LockCount);
}


KexShlExt::~KexShlExt()
{
	InterlockedDecrement(&g_LockCount);
}


STDMETHODIMP KexShlExt::QueryInterface(REFIID riid,LPVOID *ppv)
{
	if (riid == IID_IUnknown)
		*ppv = static_cast<IUnknown*>(static_cast<IShellExtInit*>(this));
	else if	(riid == IID_IShellExtInit)
		*ppv = static_cast<IShellExtInit*>(this);
	else if (riid == IID_IShellPropSheetExt)
		*ppv = static_cast<IShellPropSheetExt*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}


STDMETHODIMP_(ULONG) KexShlExt::AddRef() 
{	
	return InterlockedIncrement(&m_cRef);
}


STDMETHODIMP_(ULONG) KexShlExt::Release() 
{
	if (InterlockedDecrement(&m_cRef) == 0)
	{
		delete this;
		return 0;
	}
	return m_cRef;
}


bool KexShlExt::IsPEModule(const char* path)
{
	IMAGE_DOS_HEADER MZh;
	IMAGE_NT_HEADERS PEh;
	HANDLE f;
	bool result = false;
	DWORD bytes_read;

	f = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (f == INVALID_HANDLE_VALUE)
		return false;

	if (!ReadFile(f, &MZh, sizeof(MZh), &bytes_read, NULL) || bytes_read != sizeof(MZh))
		goto __end;

	if (MZh.e_magic != IMAGE_DOS_SIGNATURE)
		goto __end;

	if (SetFilePointer(f, MZh.e_lfanew, NULL, FILE_BEGIN) == 0xffffffff)
		goto __end;

	if (!ReadFile(f, &PEh, sizeof(PEh), &bytes_read, NULL) || bytes_read != sizeof(PEh))
		goto __end;

	if ((PEh.Signature != IMAGE_NT_SIGNATURE) 
			|| (PEh.FileHeader.Machine != IMAGE_FILE_MACHINE_I386) 
			|| (PEh.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC))
		goto __end;

	result = true;

__end:
	CloseHandle(f);

	return result;
}

bool KexShlExt::ResolveShortcut(const char* shortcutPath, char* filePath)
{
	HRESULT result;
	IShellLink* shellLink;
	IPersistFile* persistFile; 
	char path[MAX_PATH]; 
	WCHAR tmp[MAX_PATH];

	CoInitialize(NULL);

	result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
							IID_IShellLink, (void**) &shellLink); 

	if (FAILED(result))
	{
		CoUninitialize();
		return false;
	}

	result = shellLink->QueryInterface(IID_IPersistFile, (void**) &persistFile);
	if (SUCCEEDED(result))
	{
		MultiByteToWideChar(CP_ACP, 0, shortcutPath, -1, tmp, MAX_PATH);

		result = persistFile->Load(tmp, STGM_READ); 
		if (SUCCEEDED(result)) 
		{
			result = shellLink->Resolve(NULL, SLR_UPDATE); 
			if (SUCCEEDED(result)) 
			{
				result = shellLink->GetPath(path, MAX_PATH, NULL, SLGP_RAWPATH);
				if (SUCCEEDED(result))
					lstrcpyn(filePath, path, MAX_PATH); 
			} 
		} 

		persistFile->Release();
	}

	shellLink->Release();

	CoUninitialize();

	return SUCCEEDED(result);
}

STDMETHODIMP KexShlExt::Initialize(LPCITEMIDLIST pidlFolder, 
									LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg;
	HDROP hdrop;
	HRESULT result = S_OK;

	if (KexLinkage::instance.confs.empty())
		return E_FAIL;
	
	InitCommonControls();

	if (FAILED(pDataObj->GetData(&etc, &stg)))
		return E_INVALIDARG;

	// Get an HDROP handle.
	hdrop = (HDROP) GlobalLock(stg.hGlobal);

	if (!hdrop)
	{
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	ms = new ModuleSetting;
	if (!ms)
	{
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_OUTOFMEMORY;
	}

	// Determine how many files are involved in this operation.
	UINT numFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

	if (numFiles != 1)
		result = E_FAIL;
	else
	{
		// Get the filename.
		if (!DragQueryFile(hdrop, 0, ms->file, MAX_PATH) || PathIsDirectory(ms->file))
			result = E_FAIL;
		else
		{
			char* ext = PathFindExtension(ms->file);
			if (!lstrcmpi(ext, ".lnk"))
			{
				if (!ResolveShortcut(ms->file, ms->file))
					result = E_FAIL;
			}
			
			if (!IsPEModule(ms->file))
				result = E_FAIL;
		}
	}

	// Release resources.
	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

	strupr(ms->file);

	return result;
}


STDMETHODIMP KexShlExt::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hPage;

	memset(&psp, 0, sizeof(psp));
	psp.dwSize = sizeof(psp);
	psp.dwFlags = PSP_USEREFPARENT | PSP_USECALLBACK;
	psp.hInstance = g_hModule;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE);
	psp.pfnDlgProc = DlgProc;
	psp.pfnCallback = CallbackProc;
	psp.lParam = (LPARAM) ms;
	psp.pcRefParent = (UINT*) &g_LockCount;

	hPage = CreatePropertySheetPage(&psp);
	if (hPage)
	{
		if (!lpfnAddPageProc(hPage, lParam))
			DestroyPropertySheetPage(hPage);
	}

	return S_OK;
}


STDMETHODIMP KexShlExt::ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM)
{
	return E_NOTIMPL;
}


BOOL CALLBACK KexShlExt::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg) 
	{
	case WM_INITDIALOG:
		{
			PROPSHEETPAGE* ppsp = (PROPSHEETPAGE*) lParam;
			OnInitDialog(hwnd, (ModuleSetting*) ppsp->lParam);
		}
		break;

	case WM_SIZE:
		{
			WORD h = HIWORD(lParam);
			WORD w = LOWORD(lParam);

			RECT r;
			POINT p;

			GetWindowRect(GetDlgItem(hwnd, IDC_GADVAN), &r);
			p.x = r.left;
			p.y = r.top;
			ScreenToClient(hwnd, &p);
			MoveWindow(GetDlgItem(hwnd, IDC_GADVAN), p.x, p.y, w - 2 * p.x, r.bottom - r.top, TRUE);

			GetWindowRect(GetDlgItem(hwnd, IDC_GCOMPAT), &r);
			p.x = r.left;
			p.y = r.top;
			ScreenToClient(hwnd, &p);
			MoveWindow(GetDlgItem(hwnd, IDC_GCOMPAT), p.x, p.y, w - 2 * p.x, r.bottom - r.top, TRUE);

			//reposition horizontal spacer and version text
			MoveWindow(GetDlgItem(hwnd, IDC_HORIZ1), p.x, h - 14 - p.x, w -  2 * p.x, 1, TRUE);
			MoveWindow(GetDlgItem(hwnd, IDC_KEXVER), p.x, h - 12 - p.x, w - 2 * p.x, 12, TRUE);
			GetWindowRect(GetDlgItem(hwnd, IDC_TCOMPAT), &r);
			MoveWindow(GetDlgItem(hwnd, IDC_TCOMPAT), p.x, p.x, w - 2 * p.x, r.bottom - r.top, TRUE);
		}
		break;

	case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*) lParam;
			if (phdr->code == PSN_APPLY)
				OnApply(hwnd);
		}
		break;
	
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_DEFAULT:
				if (!IsDlgButtonChecked(hwnd, IDC_DEFAULT)) break;
				EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_HOOK), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_NOINHERIT), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_OVERRIDE), FALSE);
				PropSheet_Changed(GetParent(hwnd), hwnd);
				break;
			case IDC_DISABLE:
				if (!IsDlgButtonChecked(hwnd, IDC_DISABLE)) break;
				EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_HOOK), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_NOINHERIT), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_OVERRIDE), TRUE);
				PropSheet_Changed(GetParent(hwnd), hwnd);
				break;
			case IDC_COMPAT:
				if (!IsDlgButtonChecked(hwnd, IDC_COMPAT)) break;
				EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_HOOK), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_NOINHERIT), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_OVERRIDE), TRUE);
				PropSheet_Changed(GetParent(hwnd), hwnd);
				break;
			case IDC_SYSTEM:
			case IDC_HOOK:
			case IDC_NOINHERIT:
			case IDC_OVERRIDE:
				PropSheet_Changed(GetParent(hwnd), hwnd);
				break;
			}
		}
		break;
	}

	return FALSE;
}


void KexShlExt::OnInitDialog(HWND hwnd, ModuleSetting* ms)
{
	SetWindowLong(hwnd, GWL_USERDATA, (LONG) ms);
	vector<KexLinkage::conf>::const_iterator it;
	for (it = KexLinkage::instance.confs.begin() ; 
			it != KexLinkage::instance.confs.end() ; it++)	
		SendMessage(GetDlgItem(hwnd, IDC_SYSTEM), CB_ADDSTRING, 
				0, (LPARAM) (const char*) it->desc);

	bool default_index_valid = KexLinkage::instance.default_index >= 0 
			&& KexLinkage::instance.default_index < KexLinkage::instance.confs.size();
	if (default_index_valid)
		SendMessage(GetDlgItem(hwnd, IDC_SYSTEM), CB_SETCURSEL,
				KexLinkage::instance.default_index, 0);
	else
		SendMessage(GetDlgItem(hwnd, IDC_SYSTEM), CB_SETCURSEL, 0, 0);

	KexLinkage::instance.m_kexGetModuleSettings(ms->file, ms->conf, &ms->flags);
	
	for (int i = 0 ; i < KexLinkage::instance.confs.size() ; i++)
		if (!strcmp(ms->conf, KexLinkage::instance.confs[i].name))
		{
			CheckDlgButton(hwnd, IDC_COMPAT, BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), TRUE);
			SendMessage(GetDlgItem(hwnd, IDC_SYSTEM), CB_SETCURSEL, i, 0);
			break;
		}
	if (!(ms->flags & KRF_VALID_FLAG))
	{
		CheckDlgButton(hwnd, IDC_DEFAULT, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_HOOK), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_NOINHERIT), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_OVERRIDE), FALSE);
	}
	if (ms->flags & KRF_KEX_DISABLE)
	{
		CheckDlgButton(hwnd, IDC_DISABLE, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_SYSTEM), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_HOOK), FALSE);
	}
	if (ms->flags & KRF_HOOK_APIS)
		CheckDlgButton(hwnd, IDC_HOOK, BST_CHECKED);
	if (ms->flags & KRF_NO_INHERIT)
		CheckDlgButton(hwnd, IDC_NOINHERIT, BST_CHECKED);
	if (ms->flags & KRF_OVERRIDE_PROC_MOD)
		CheckDlgButton(hwnd, IDC_OVERRIDE, BST_CHECKED);

	{
		//show what are the default compatibility settings
		char bufi[512];
		char bufo[512];
		char bufs[512];
		
		SendMessage(GetDlgItem(hwnd, IDC_DEFAULT), WM_GETTEXT, 
				(WPARAM) sizeof(bufi), (LPARAM) bufi);
		LoadString(g_hModule, KexLinkage::instance.disable_extensions ?
				IDS_DISABLED : IDS_ENABLED, bufs, sizeof(bufs));
		snprintf(bufo, sizeof(bufo), bufi, bufs);
		SendMessage(GetDlgItem(hwnd, IDC_DEFAULT), WM_SETTEXT, 
				(WPARAM) 0, (LPARAM) bufo);
	}

	DWORD caps = KexLinkage::instance.m_kexGetCoreCaps();
	{
		//set KernelEx version
		unsigned long ver = KexLinkage::instance.m_kexGetKEXVersion();
		char ver_s[32];
		snprintf(ver_s, sizeof(ver_s), "KernelEx Core v%d.%d.%d %s", ver>>24,
				(ver>>16) & 0xff, ver & 0xffff, caps & KCC_DEBUG ? "DEBUG" : "");
		SendMessage(GetDlgItem(hwnd, IDC_KEXVER), WM_SETTEXT, 0, (LPARAM) ver_s);
	}

	ShowWindow(GetDlgItem(hwnd, IDC_HOOK), caps & KCC_APIHOOK ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hwnd, IDC_OVERRIDE), caps & KCC_DEBUG ? SW_SHOW : SW_HIDE);

	if (!(caps & KCC_DEBUG))
	{
		RECT r;
		HWND h = GetDlgItem(hwnd, IDC_GADVAN);
		GetWindowRect(h, &r);
		r.bottom -= 20; //space between IDC_HOOK and element above
		if (!(caps & KCC_APIHOOK))
			r.bottom -= 20;
		SetWindowPos(h, NULL, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE);
	}

	tips.load_tips();

	HWND hwndTip = CreateTooltipWindow(hwnd);
	CreateTooltip(hwndTip, hwnd, IDC_DEFAULT, tips._TIP_DEFAULT);
	CreateTooltip(hwndTip, hwnd, IDC_DISABLE, tips._TIP_DISABLE);
	CreateTooltip(hwndTip, hwnd, IDC_COMPAT, tips._TIP_COMPAT);
	CreateTooltip(hwndTip, hwnd, IDC_SYSTEM, tips._TIP_SYSTEM);
	CreateTooltip(hwndTip, hwnd, IDC_NOINHERIT, tips._TIP_NOINHERIT);
	CreateTooltip(hwndTip, hwnd, IDC_HOOK, tips._TIP_HOOK);
	CreateTooltip(hwndTip, hwnd, IDC_OVERRIDE, tips._TIP_OVERRIDE);
}


void KexShlExt::OnApply(HWND hwnd)
{
	ModuleSetting* ms = (ModuleSetting*) GetWindowLong(hwnd, GWL_USERDATA);
	DWORD flags = 0;
	const char* conf = "";
	if (!IsDlgButtonChecked(hwnd, IDC_DEFAULT))
		flags |= KRF_VALID_FLAG;
	if (IsDlgButtonChecked(hwnd, IDC_DISABLE))
		flags |= KRF_KEX_DISABLE;
	if (IsDlgButtonChecked(hwnd, IDC_COMPAT))
		conf = KexLinkage::instance.confs[SendMessage(
				GetDlgItem(hwnd, IDC_SYSTEM), CB_GETCURSEL, 0, 0)].name;
	if (IsDlgButtonChecked(hwnd, IDC_HOOK))
		flags |= KRF_HOOK_APIS;
	if (IsDlgButtonChecked(hwnd, IDC_NOINHERIT))
		flags |= KRF_NO_INHERIT;
	if (IsDlgButtonChecked(hwnd, IDC_OVERRIDE))
		flags |= KRF_OVERRIDE_PROC_MOD;

	if (flags != ms->flags || strcmp(conf, ms->conf) != 0)
	{
		if (flags & KRF_VALID_FLAG)
			KexLinkage::instance.m_kexSetModuleSettings(ms->file, conf, flags);
		else
			KexLinkage::instance.m_kexResetModuleSettings(ms->file);
	}
}


UINT CALLBACK KexShlExt::CallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	if (uMsg == PSPCB_RELEASE)
		delete (ModuleSetting*) ppsp->lParam;

	return 1;   // used for PSPCB_CREATE - let the page be created
}

HWND KexShlExt::CreateTooltipWindow(HWND parent)
{
	HWND hwndTip;
	hwndTip = CreateWindow(TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			parent, NULL, g_hModule, NULL);
	SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 300);
	SendMessage(hwndTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELONG(32767, 0));
	return hwndTip;
}

bool KexShlExt::CreateTooltip(HWND hwndTip, HWND hDlg, int toolID, char* pText)
{
	if (!hwndTip || !toolID || !hDlg || !pText)
		return false;

	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);
	if (!hwndTool)
		return false;

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR) hwndTool;
	toolInfo.lpszText = pText;
	toolInfo.hinst = g_hModule;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) &toolInfo);

	return true;
}
