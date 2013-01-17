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

#include <windows.h>
#include "opensavefile.h"

static UINT WM_HELPMSGSTRING;
static UINT WM_FILEOKSTRING;
static UINT WM_SHAREVISTRING;

//this procedure adapts unicode hook proc, both old and new
static UINT_PTR CALLBACK OFNHookProcWAdapter(
	LPOFNHOOKPROC pfnhook, LPOPENFILENAMEW lpofnW, HWND hdlg, UINT uiMsg, 
	WPARAM wParam, LPARAM lParam
)
{
	switch(uiMsg)
	{
	case WM_INITDIALOG:
		return pfnhook(hdlg,uiMsg,wParam,(LPARAM)lpofnW);
	case WM_NOTIFY:
	{
		if (!lParam) return 0;
		LPOFNOTIFYA notif = (LPOFNOTIFYA)lParam;
		switch (notif->hdr.code)
		{
		case CDN_FILEOK:
		case CDN_FOLDERCHANGE:
		case CDN_HELP:
		case CDN_INCLUDEITEM:
		case CDN_INITDONE:
		case CDN_SELCHANGE:
		case CDN_SHAREVIOLATION:
		case CDN_TYPECHANGE:
		{
			if (notif->hdr.code == CDN_FILEOK || notif->hdr.code == CDN_SHAREVIOLATION)
			{
				//pszFile is updated
				if (notif->lpOFN->Flags & (OFN_EXPLORER|OFN_ALLOWMULTISELECT))
				{
					//many files could be selected, convert \0-separated list
					MultiByteToWideChar(CP_ACP, 0, notif->lpOFN->lpstrFile,
							strlenAA(notif->lpOFN->lpstrFile), 
							lpofnW->lpstrFile, lpofnW->nMaxFile);
				}
				else
				{
					MultiByteToWideChar(CP_ACP, 0, notif->lpOFN->lpstrFile,
							-1,lpofnW->lpstrFile,lpofnW->nMaxFile);
				}
			}
			lpofnW->nFilterIndex = notif->lpOFN->nFilterIndex;
			lpofnW->Flags = notif->lpOFN->Flags;
			((LPOFNOTIFYW)notif)->lpOFN = lpofnW;
		}
		default:
			return pfnhook(hdlg,uiMsg,wParam,lParam);
		}
	}
	//old style hooks messages
	default:
		if (uiMsg == WM_HELPMSGSTRING || uiMsg == WM_FILEOKSTRING || uiMsg == WM_SHAREVISTRING)
		{
			LPOPENFILENAMEA lpofnA = (LPOPENFILENAMEA)lParam;
			lpofnW->nFilterIndex = lpofnA->nFilterIndex;
			lpofnW->Flags = lpofnA->Flags;
			if (uiMsg == WM_FILEOKSTRING || uiMsg == WM_SHAREVISTRING)
			{				
				//pszFile is updated
				if (lpofnW->Flags & (OFN_EXPLORER|OFN_ALLOWMULTISELECT))
				{
					//many files could be selected, convert \0-separated list
					MultiByteToWideChar(CP_ACP, 0, lpofnA->lpstrFile, 
							strlenAA(lpofnA->lpstrFile), 
							lpofnW->lpstrFile, lpofnW->nMaxFile);
				}
				else
				{
					MultiByteToWideChar(CP_ACP, 0, lpofnA->lpstrFile, -1, 
							lpofnW->lpstrFile,lpofnW->nMaxFile);
				}
			}
			lParam = (LPARAM)lpofnW;
		}
		return pfnhook(hdlg,uiMsg,wParam,lParam);
	}
}


//this procedure handles unicode CDM_ messages. makes sense only to Explorer-style dlgs
static INT_PTR CALLBACK OpenSaveDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DLGPROC DlgProc = (DLGPROC)GetProp(hWnd,OLDDLGPROCPROP);
	if (!DlgProc) return 0;
	
	switch(Msg)
	{
		case CDM_GETSPEC:
		case CDM_GETFILEPATH:
		case CDM_GETFOLDERPATH:
		{			
			LPSTR bufA;
			ABUFFER_ALLOC(bufA,wParam);
			LRESULT ret = DlgProc(hWnd,Msg,wParam,(LPARAM)bufA);
			ABUFFER_toW(bufA,lParam,wParam);
			BUFFER_FREE(bufA);
			return ret;
		}
		case CDM_SETCONTROLTEXT:
		case CDM_SETDEFEXT:
		{
			LPSTR strA;
			STACK_WtoA(lParam,strA);
			return DlgProc(hWnd,Msg,wParam,(LPARAM)strA);
		}		
		default:
			return DlgProc(hWnd,Msg,wParam,lParam);
	}
}

static void CALLBACK WinCreateEvent( 
	HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject,
	LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime
)
{
	//catch "Open"(Save) button creation, since on dialog window creation no dlgproc set yet
	if (GetDlgCtrlID(hwnd) == IDOK)
	{
		hwnd = GetParent(hwnd);
		//mark dialog created as unicode. really, everything should be marked unicode
		//with thunilay but then dialog may choke on unicode common controls
		if (GetClassWord(hwnd,GCW_ATOM) == DIALOG_ATOM 
				&& GetProp(hwnd,OLDDLGPROCPROP) == NULL)
		{
			SetWindowUnicode(hwnd,TRUE);			
			HANDLE h = (HANDLE)SetWindowLongA(hwnd,DWL_DLGPROC,(LONG)OpenSaveDlgProcW);
			SetProp(hwnd,OLDDLGPROCPROP,h);
		}
	}
}

static BOOL WINAPI GetOpenOrSaveFileNameW(LPOPENFILENAMEW lpofn, BOOL IsSave)
{
	BOOL ret;
	OPENFILENAMEA ofnA;
	memcpy(&ofnA,lpofn,sizeof(OPENFILENAME));
	//File filters
	STACK_WWtoAA(lpofn->lpstrFilter,ofnA.lpstrFilter);
	//Custom filter [NOBODY USES LOL]
	ofnA.lpstrCustomFilter = NULL;
	if (lpofn->lpstrCustomFilter && lpofn->nMaxCustFilter)
	{		
		ofnA.nMaxCustFilter = lpofn->nMaxCustFilter * 2;
		ofnA.lpstrCustomFilter = StrAllocA(ofnA.nMaxCustFilter);
		WideCharToMultiByte(CP_ACP, 0, lpofn->lpstrCustomFilter, -1,
				ofnA.lpstrCustomFilter, ofnA.nMaxCustFilter, NULL, NULL);
	}
	//File path buffer - nMaxFile is byte count
	if (lpofn->lpstrFile)
	{
		ofnA.nMaxFile = lpofn->nMaxFile * 2;
		ofnA.lpstrFile = StrAllocA(ofnA.nMaxFile);
		WideCharToMultiByte(CP_ACP, 0, lpofn->lpstrFile, -1,
				ofnA.lpstrFile, ofnA.nMaxFile, "_", NULL);
	}
	//File title buffer - same
	if (lpofn->lpstrFileTitle)
	{
		ofnA.nMaxFileTitle = lpofn->nMaxFileTitle * 2;
		ofnA.lpstrFileTitle = StrAllocA(ofnA.nMaxFileTitle);
	}
	//Misc stuff
	STACK_WtoA(lpofn->lpstrInitialDir, ofnA.lpstrInitialDir);
	STACK_WtoA(lpofn->lpstrTitle, ofnA.lpstrTitle);
	STACK_WtoA(lpofn->lpstrDefExt, ofnA.lpstrDefExt);
	if (ofnA.Flags & OFN_ENABLETEMPLATE)
		STACK_WtoA(lpofn->lpTemplateName, ofnA.lpTemplateName);
	//Hook: build hook thunk on stack
	if (ofnA.Flags & OFN_ENABLEHOOK)
	{
		POFN_THUNK thunk;
		WM_HELPMSGSTRING = RegisterWindowMessage(HELPMSGSTRING);
		WM_FILEOKSTRING = RegisterWindowMessage(FILEOKSTRING);
		WM_SHAREVISTRING = RegisterWindowMessage(SHAREVISTRING);
		ALLOC_OFNTHUNK(thunk, lpofn, lpofn->lpfnHook, OFNHookProcWAdapter);
		ofnA.lpfnHook = (LPOFNHOOKPROC)thunk;
	}
	//Set up eventhook to make dialog unicode
	HWINEVENTHOOK eventhook = SetWinEventHook(EVENT_OBJECT_CREATE, 
		EVENT_OBJECT_CREATE, GetModuleHandle("USER32.DLL"),
		(WINEVENTPROC)WinCreateEvent, GetCurrentProcessId(), 
		GetCurrentThreadId(), WINEVENT_INCONTEXT);

	if (IsSave)
		ret = GetSaveFileNameA_fix(&ofnA);
	else
		ret = GetOpenFileNameA_fix(&ofnA);

	UnhookWinEvent(eventhook);

	if (ret)
	{
		//copy stuff back
		lpofn->nFilterIndex = ofnA.nFilterIndex;
		lpofn->Flags = ofnA.Flags;
		//translate buffers back
		if (ofnA.lpstrCustomFilter)
			MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrCustomFilter, -1,
					lpofn->lpstrCustomFilter, lpofn->nMaxCustFilter);
		if (ofnA.lpstrFile)
		{
			if (ofnA.Flags & (OFN_EXPLORER|OFN_ALLOWMULTISELECT))
			{
				//many files could be selected, convert \0-separated list
				MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrFile,
						strlenAA(ofnA.lpstrFile), lpofn->lpstrFile, lpofn->nMaxFile);
			}
			else
			{
				MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrFile, -1,
						lpofn->lpstrFile, lpofn->nMaxFile);
			}
			//translate buffer offsets
			lpofn->nFileOffset = MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrFile,
					ofnA.nFileOffset, NULL, 0);
			lpofn->nFileExtension = MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrFile,
					ofnA.nFileExtension, NULL, 0);
		}
		if (ofnA.lpstrFileTitle)
			MultiByteToWideChar(CP_ACP, 0, ofnA.lpstrFileTitle, -1, 
					lpofn->lpstrFileTitle, lpofn->nMaxFileTitle);			
	}
	else //function failed
	{
		//buffer too small? lpstrFile contains proper size
		if ( CommDlgExtendedError() == FNERR_BUFFERTOOSMALL )
			*(WORD*)lpofn->lpstrFile = *(WORD*)ofnA.lpstrFile;
	}
	//free buffers
	StrFree(ofnA.lpstrCustomFilter);
	StrFree(ofnA.lpstrFile);
	StrFree(ofnA.lpstrFileTitle);
	
	return ret;
}

/* MAKE_EXPORT GetSaveFileNameW_new=GetSaveFileNameW */
BOOL WINAPI GetSaveFileNameW_new(LPOPENFILENAMEW lpofn)
{
	return GetOpenOrSaveFileNameW(lpofn,TRUE);
}

/* MAKE_EXPORT GetOpenFileNameW_new=GetOpenFileNameW */
BOOL WINAPI GetOpenFileNameW_new(LPOPENFILENAMEW lpofn)
{
	return GetOpenOrSaveFileNameW(lpofn,FALSE);
}
