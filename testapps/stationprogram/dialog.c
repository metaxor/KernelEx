#include "main.h"
#include "resource.h"
#include <stdio.h>

HWND DialogWnd = NULL;

INT_PTR CALLBACK DialogProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
            SetWindowPos(hwndDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			DialogWnd = hwndDlg;
			break;
		}
		case WM_CLOSE:
		{
			ShowWindow(hwndDlg, SW_HIDE);
			UnloadEverything();
			EndDialog(hwndDlg, 1);

			return 0;
			break;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_RUNPROGRAM:
                {
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					TCHAR buffer[255];
                    TCHAR buf[255];
                    LPVOID buffer2;
					ZeroMemory(&si, sizeof(si));
					ZeroMemory(&pi, sizeof(pi));

					GetDlgItemText(hwndDlg, IDT_PROGRAMTORUN, buffer, 255);

                    if(!CreateProcessInDesktop(Session->WindowStation,
                                                Session->Desktop,
                                                NULL,
                                                buffer,
                                                NULL,
                                                NULL,
                                                FALSE,
                                                0,
                                                NULL,
                                                NULL,
                                                &si,
                                                &pi))
                    {
                        ErrorCodeToString(GetLastError(), &buffer2);
                        wsprintf(buf, "Failed to create process, error %d : %s", GetLastError(), buffer2);
                        MessageBox(hwndDlg, buf, NULL, MB_ICONWARNING);
                    }

					/*si.lpDesktop = L"WinSta1\\Default";
					CreateProcessW(NULL,
									buffer,
									NULL,
									NULL,
									FALSE,
									0,
									NULL,
									NULL,
									&si,
									&pi);*/

					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);

					return 1;
					break;
                }

                case IDC_REVERTDESKTOP:
                    EndDialog(hwndDlg, 3);
                    break;
                case IDC_SWITCHDESKTOP:
                    EndDialog(hwndDlg, 2);
                    break;

				case IDC_ENDWINSTA1:
					SendMessage(hwndDlg, WM_CLOSE, 0, 0);
					break;
			}
			break;
		}
		case WM_QUERYENDSESSION:
		{
			SendMessage(hwndDlg, WM_CLOSE, 0, 0);

			return FALSE;
		}
	}
	return 0;
}

BOOL CreateMultiSessionWindow()
{
    INT_PTR Result;
    BOOL retval;
    TCHAR buffer[255];

    Result = DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_STATIONPROGRAM), NULL, DialogProc);

	if(Result == -1)
		return FALSE;
    else if(Result == 2)
    {
        retval = SetProcessWindowStation(Session->WindowStation);
        if(!retval)
        {
            wsprintf(buffer, TEXT("Setting process to window station 0x%X failed"), Session->WindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }

        retval = SetThreadDesktop(Session->Desktop);
        if(!retval)
        {
            SetProcessWindowStation(Session->InteractiveWindowStation);
            wsprintf(buffer, TEXT("Setting thread to desktop 0x%X on window station 0x%X failed"), Session->Desktop, Session->WindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }

        retval = SwitchDesktop(Session->Desktop);
        if(!retval)
        {
            SetProcessWindowStation(Session->InteractiveWindowStation);
            SetThreadDesktop(Session->DefaultDesktop);
            wsprintf(buffer, TEXT("Setting thread to desktop 0x%X on window station 0x%X failed"), Session->Desktop, Session->WindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }
        goto cont;
    }
    else if(Result == 3)
    {
        retval = SetProcessWindowStation(Session->InteractiveWindowStation);
        if(!retval)
        {
            wsprintf(buffer, TEXT("Setting process to window station 0x%X failed"), Session->InteractiveWindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }

        retval = SetThreadDesktop(Session->DefaultDesktop);
        if(!retval)
        {
            SetProcessWindowStation(Session->WindowStation);
            wsprintf(buffer, TEXT("Setting thread to desktop 0x%X on window station 0x%X failed"), Session->DefaultDesktop, Session->InteractiveWindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }

        retval = SwitchDesktop(Session->DefaultDesktop);
        if(!retval)
        {
            SetProcessWindowStation(Session->WindowStation);
            SetThreadDesktop(Session->Desktop);
            wsprintf(buffer, TEXT("Setting thread to desktop 0x%X on window station 0x%X failed"), Session->DefaultDesktop, Session->InteractiveWindowStation);
            MessageBox(NULL, buffer, NULL, 0);
            goto cont;
        }
        goto cont;
    }

	return TRUE;
cont:
    return CreateMultiSessionWindow();
}
