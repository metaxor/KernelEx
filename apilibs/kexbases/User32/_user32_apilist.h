/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86, Ley0k
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

#ifndef _USER32_APILIST_H
#define _USER32_APILIST_H

#include "kexcoresdk.h"
#include "main.h"

#define MAX_HARD_ERRORS		128

/* There is no GetAltTabInfoA or RealGetWindowClassA in 9x */
#ifdef GetAltTabInfo
#undef GetAltTabInfo
#endif
EXTERN_C BOOL WINAPI GetAltTabInfo(HWND,int,PVOID,LPSTR,UINT);
#ifdef RealGetWindowClass
#undef RealGetWindowClass
#endif
EXTERN_C UINT WINAPI RealGetWindowClass(HWND  hwnd, LPTSTR pszType, UINT  cchType);


typedef BOOL (WINAPI *IsHungThread_t)(DWORD ThreadID);
extern IsHungThread_t IsHungThread_pfn;
typedef BOOL (WINAPI *DrawCaptionTempA_t)(HWND, HDC, const RECT*, HFONT, HICON, LPCSTR, UINT);
extern DrawCaptionTempA_t DrawCaptionTempA_pfn;
typedef int (WINAPI *GetMouseMovePoints_t)(UINT,LPMOUSEMOVEPOINT,LPMOUSEMOVEPOINT,int,DWORD);
extern GetMouseMovePoints_t GetMouseMovePoints_pfn;

typedef struct _HARDERRORDATA
{
	LPCSTR lpHardErrorTitle;
	LPCSTR lpHardErrorMessage;
	DWORD dwProcessId;
	HANDLE hEvent;
	DWORD Result;
	ULONG uType;
} HARDERRORDATA, *PHARDERRORDATA;

extern PPDB98 Msg32Process;
extern PPDB98 MprProcess;
extern DWORD gpidMpr;

extern PTDB98 pHardErrorThread;
extern DWORD HardErrorThreadId;

extern PTDB98 pShutdownThread;

extern PHARDERRORDATA *pHardErrorData;

extern BOOL fShutdown;

BOOL WINAPI Win32RaiseHardError(LPCSTR lpszMessage, LPCSTR lpszTitle, UINT uMsgType, BOOL fWait);

BOOL init_user32();
BOOL thread_user32_init(PTDB98 Thread);
BOOL process_user32_init(PPDB98 Process);
VOID thread_user_uninit(PTDB98 Thread);
VOID process_user_uninit(PPDB98 Process);
extern const apilib_api_table apitable_user32;

/*** AUTOGENERATED APILIST DECLARATIONS BEGIN ***/
BOOL WINAPI SystemParametersInfoA_fix(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
STUB InitSharedTable_stub;
STUB PrintWindow_stub;
STUB RegisterServicesProcess_stub;
STUB SetLayeredWindowAttributes_stub;
BOOL WINAPI CloseDesktop_new(HDESK hDesktop);
HDESK WINAPI CreateDesktopA_new(LPCSTR lpszDesktop, LPCSTR lpszDevice, LPDEVMODE pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa);
HDESK WINAPI CreateDesktopExA_new(LPCSTR lpszDesktop, LPCSTR lpszDevice, LPDEVMODEA pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa, ULONG ulHeapSize, PVOID pvoid);
BOOL WINAPI EnumDesktopsA_new(HWINSTA hwinsta, DESKTOPENUMPROCA lpEnumFunc, LPARAM lParam);
BOOL WINAPI EnumDesktopWindows_new(HDESK hDesktop, WNDENUMPROC lpfn, LPARAM lParam);
HWND WINAPI GetDesktopWindow_new(VOID);
HDESK WINAPI GetInputDesktop_new(VOID);
HDESK WINAPI GetThreadDesktop_new(DWORD dwThreadId);
HDESK WINAPI OpenDesktopA_new(LPSTR lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
HDESK WINAPI OpenInputDesktop_new(DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
BOOL WINAPI SetThreadDesktop_new(HDESK hDesktop);
BOOL WINAPI SwitchDesktop_new(HDESK hDesktop);
HWND WINAPI CreateDialogIndirectParamA_fix(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit);
HWND WINAPI CreateDialogParamA_fix(HINSTANCE hInstance, LPCTSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND WINAPI GetNextDlgTabItem_fix(HWND hDlg, HWND hCtl, BOOL bPrevious);
LONG WINAPI ChangeDisplaySettingsExA_fix(LPCSTR lpszDeviceName, LPDEVMODE lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam);
LONG WINAPI ChangeDisplaySettingsA_fix(LPDEVMODE lpDevMode, DWORD dwflags);
BOOL WINAPI EnumDisplaySettingsA_fix(LPCSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode);
BOOL WINAPI EnumDisplaySettingsExA_fix(LPCSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode, DWORD dwFlags);
BOOL WINAPI ExitWindowsEx_fix(UINT uFlags, DWORD dwReserved);
SHORT WINAPI GetAsyncKeyState_nothunk(int vKey);
BOOL WINAPI GetCursorPos_nothunk(LPPOINT lpPoint);
int WINAPI GetMouseMovePointsEx_98(UINT size, LPMOUSEMOVEPOINT ptin, LPMOUSEMOVEPOINT ptout, int count, DWORD res);
UINT WINAPI MapVirtualKeyA_new(UINT uCode, UINT uMapType);
UINT WINAPI MapVirtualKeyExA_new(UINT uCode, UINT uMapType, HKL dwhkl);
LRESULT WINAPI DefRawInputProc_new(PVOID paRawInput, INT nInput, UINT cbSizeHeader);
UINT WINAPI GetRawInputBuffer_new(PVOID pData, PUINT pcbSize, UINT cbSizeHeader);
UINT WINAPI GetRawInputData_new(PVOID hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);
UINT WINAPI GetRawInputDeviceList_new(PVOID pRawInputDeviceList, PUINT puiNumDevices, UINT cbSize);
UINT WINAPI GetRawInputDeviceInfo_new(HANDLE hDevice, UINT uiCommand, LPVOID pData, PUINT pcbSize);
UINT WINAPI GetRegisteredRawInputDevices_new(PVOID pRawInputDevices, PUINT puiNumDevices, UINT cbSize);
BOOL WINAPI RegisterRawInputDevices_new(PVOID pRawInputDevices, UINT uiNumDevices, UINT cbSize);
BOOL WINAPI SetCursorPos_nothunk(int X, int Y);
LPWSTR WINAPI CharNextExW_new(WORD codepage, LPCWSTR ptr, DWORD flags);
LPWSTR WINAPI CharNextW_new(LPCWSTR x);
LPSTR WINAPI CharPrevExW_new(WORD codepage, LPCWSTR start, LPCWSTR ptr, DWORD flags);
LPWSTR WINAPI CharPrevW_new(LPCWSTR start,LPCWSTR x);
BOOL WINAPI CharToOemBuffW_new(LPCWSTR s, LPSTR d, DWORD len);
BOOL WINAPI CharToOemW_new(LPCWSTR s, LPSTR d);
BOOL WINAPI OemToCharBuffW_new(LPCSTR s, LPWSTR d, DWORD len);
BOOL WINAPI OemToCharW_new(LPCSTR s, LPWSTR d);
LPWSTR WINAPI CharLowerW_new(LPWSTR x);
LPWSTR WINAPI CharUpperW_new(LPWSTR x);
DWORD WINAPI CharLowerBuffW_new(LPWSTR str, DWORD len);
DWORD WINAPI CharUpperBuffW_new(LPWSTR str, DWORD len);
BOOL WINAPI IsCharLowerW_new(WCHAR x);
BOOL WINAPI IsCharUpperW_new(WCHAR x);
BOOL WINAPI IsCharAlphaNumericW_new(WCHAR x);
BOOL WINAPI IsCharAlphaW_new(WCHAR x);
HMENU WINAPI LoadMenuA_fix(HINSTANCE hInstance, LPCSTR lpMenuName);
HMENU WINAPI LoadMenuW_new(HINSTANCE hInstance, LPCWSTR lpMenuNameW);
BOOL WINAPI InsertMenuW_new(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItemW);
BOOL WINAPI AppendMenuW_new(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem);
BOOL WINAPI ChangeMenuW_new(HMENU hMenu, UINT cmd, LPCWSTR lpNewItemW, UINT uIDNewItem, UINT uFlags);
BOOL WINAPI ModifyMenuW_new(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItemW);
LRESULT WINAPI SendMessageA_fix(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LONG WINAPI GetWindowLongA_NEW(HWND hWnd, int nIndex);
BOOL WINAPI IsWindowUnicode_NEW(HWND hWnd);
LONG WINAPI SetWindowLongA_NEW(HWND hWnd, int nIndex, LONG dwNewLong);
LONG WINAPI GetWindowLongW_NEW(HWND hWnd, int nIndex);
LONG WINAPI SetWindowLongW_NEW(HWND hWnd, int nIndex, LONG dwNewLong);
DWORD WINAPI GetClassLongW_NEW(HWND hWnd, int nIndex);
DWORD WINAPI SetClassLongW_NEW(HWND hWnd, int nIndex, LONG dwNewLong);
ATOM WINAPI RegisterClassExW_NEW(CONST WNDCLASSEXW *lpwcx);
ATOM WINAPI RegisterClassW_NEW(CONST WNDCLASSW *lpwcx);
ATOM WINAPI GetClassInfoExW_NEW(HINSTANCE hinst, LPCWSTR lpszClass, WNDCLASSEXW *lpwcx);
ATOM WINAPI GetClassInfoW_NEW(HINSTANCE hinst, LPCWSTR lpszClass, WNDCLASSW *wc);
HWND WINAPI CreateWindowExW_NEW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowW_NEW(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateMDIWindowW_NEW(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, LPARAM lParam);
HWND WINAPI CreateDialogParamW_NEW(HINSTANCE hInstance, LPCTSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND WINAPI CreateDialogIndirectParamW_NEW(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit);
INT_PTR WINAPI DialogBoxParamW_NEW(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
INT_PTR WINAPI DialogBoxIndirectParamW_NEW(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
LRESULT WINAPI SendMessageW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefWindowProcW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI SetWindowTextW_NEW(HWND hWnd, LPCWSTR lpString);
BOOL WINAPI SetDlgItemTextW_NEW(HWND hDlg, int nIDDlgItem, LPCWSTR lpString);
LRESULT WINAPI SendDlgItemMessageW_NEW(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam);
int WINAPI GetWindowTextW_NEW(HWND hWnd, LPWSTR lpString, int nMaxCount);
int WINAPI GetDlgItemTextW_NEW(HWND hDlg, int nIDDlgItem, LPWSTR lpString, int nMaxCount);
int WINAPI GetWindowTextLengthW_NEW(HWND hWnd);
BOOL WINAPI UnregisterClassW_NEW(LPCWSTR lpClassName, HINSTANCE hInstance);
BOOL WINAPI SendMessageCallbackW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, SENDASYNCPROC lpCallBack, DWORD dwData);
LRESULT WINAPI SendMessageTimeoutW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, LPDWORD lpdwResult);
LRESULT WINAPI CallWindowProcA_fix(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI CallWindowProcW_new(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefDlgProcW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefMDIChildProcW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI PostMessageW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI SendNotifyMessageW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI PostThreadMessageW_NEW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefFrameProcW_NEW(HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI GetMessageA_NEW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
BOOL WINAPI PeekMessageA_NEW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
BOOL WINAPI GetMessageW_NEW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
BOOL WINAPI PeekMessageW_NEW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
BOOL WINAPI GetLastInputInfo_NEW(PLASTINPUTINFO plii);
int WINAPI ToUnicodeEx_new(UINT wVirtKey, UINT wScanCode, const PBYTE lpKeyState, LPWSTR pwszBuff, int cchBuff, UINT wFlags, HKL dwhkl);
int WINAPI ToUnicode_new(UINT wVirtKey, UINT wScanCode, const PBYTE lpKeyState, LPWSTR pwszBuff, int cchBuff, UINT wFlags);
HDESK WINAPI CreateDesktopExW_new(LPWSTR lpszDesktop, LPWSTR lpszDevice, LPDEVMODEW pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa, ULONG ulHeapSize, PVOID pvoid);
HDESK WINAPI CreateDesktopW_new(LPWSTR lpszDesktopW, LPWSTR lpszDeviceW, LPDEVMODEW pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa);
HWINSTA WINAPI CreateWindowStationW_new(LPWSTR lpwinstaW, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa);
BOOL WINAPI DrawCaptionTempW_new(HWND hwnd, HDC hdc, const RECT *rect, HFONT hFont, HICON hIcon, LPCWSTR strW, UINT uFlags);
BOOL WINAPI EnumDesktopsW_new(HWINSTA hwinsta, DESKTOPENUMPROCW lpEnumFunc, LPARAM lParam);
BOOL WINAPI EnumWindowStationsW_new(WINSTAENUMPROCW lpEnumFunc, LPARAM lParam);
BOOL WINAPI GetUserObjectInformationW_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded);
int WINAPI LoadStringW_new(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int nBufferMax);
HDESK WINAPI OpenDesktopW_new(LPWSTR lpszDesktopW, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
HWINSTA WINAPI OpenWindowStationW_new(LPWSTR lpszWinStaW, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
UINT WINAPI RealGetWindowClassW_new(HWND hwnd, LPWSTR pszTypeW, UINT cchType);
BOOL WINAPI AllowSetForegroundWindow_98(DWORD procid);
BOOL WINAPI AnyPopup_nothunk(VOID);
HWND WINAPI CreateMDIWindowA_fix(LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, LPARAM lParam);
HWND WINAPI CreateWindowExA_fix(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
void __stdcall DisableProcessWindowsGhosting_new(void);
BOOL WINAPI EnableWindow_fix(HWND hWnd, BOOL bEnable);
BOOL WINAPI EnumChildWindows_nothunk(HWND hWndParent, WNDENUMPROC lpEnumFunc, LPARAM lParam);
BOOL WINAPI EnumThreadWindows_nothunk(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam);
BOOL WINAPI EnumWindows_nothunk(WNDENUMPROC lpEnumFunc, LPARAM lParam);
HWND WINAPI GetParent_nothunk(HWND hWnd);
BOOL WINAPI LockSetForegroundWindow_98(UINT lockcode);
HWND APIENTRY GetAncestor_fix(HWND hwnd, UINT gaFlags);
HWND APIENTRY GetShellWindow_new(VOID);
int APIENTRY InternalGetWindowText_new(HWND hWnd, LPWSTR lpString, int nMaxCount);
BOOL WINAPI IsHungAppWindow_new(HWND hWnd);
BOOL WINAPI IsWindowVisible_fix(HWND hWnd);
BOOL WINAPI SetForegroundWindow_fix(HWND hWnd);
BOOL WINAPI SetWindowPos_fix(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL WINAPI ShowWindow_fix(HWND hWnd, int nCmdShow);
BOOL WINAPI ShowWindowAsync_fix(HWND hWnd, int nCmdShow);
HWND WINAPI SetParent_nothunk(HWND hWndChild, HWND hWndNewParent);
BOOL WINAPI UpdateLayeredWindow_new(HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
BOOL WINAPI CloseWindowStation_new(HWINSTA hWinSta);
HWINSTA WINAPI CreateWindowStationA_new(LPCSTR lpwinsta, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa);
BOOL WINAPI EnumWindowStationsA_new(WINSTAENUMPROCA lpEnumFunc, LPARAM lParam);
HWINSTA WINAPI GetProcessWindowStation_new(void);
BOOL WINAPI GetUserObjectInformationA_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded);
BOOL WINAPI GetUserObjectSecurity_new(HANDLE hObj, PSECURITY_INFORMATION pSIRequested, PSECURITY_DESCRIPTOR pSD, DWORD nLength, LPDWORD lpnLengthNeeded);
BOOL WINAPI LockWorkStation_new(void);
BOOL WINAPI LockWindowStation_new(HWINSTA hWinSta);
HWINSTA WINAPI OpenWindowStationA_new(LPSTR lpszWinSta, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
BOOL WINAPI RegisterLogonProcess_new(DWORD dwProcessId, BOOL fUnknown);
BOOL WINAPI SetLogonNotifyWindow_new(HWINSTA hWinSta, HWND hWnd);
BOOL WINAPI SetProcessWindowStation_new(HWINSTA hWinSta);
BOOL WINAPI SetUserObjectInformationA_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength);
BOOL WINAPI SetUserObjectSecurity_new(HANDLE hObj, PSECURITY_INFORMATION pSIRequested, PSECURITY_DESCRIPTOR pSD);
BOOL WINAPI UnlockWindowStation_new(HWINSTA hWinSta);
/*** AUTOGENERATED APILIST DECLARATIONS END ***/

#endif
