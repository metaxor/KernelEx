#include "main.h"
#include "resource.h"
#include <stdio.h>

HWND DialogWnd = NULL;
BOOL bRestartDialog = TRUE;
HANDLE hToken;
BOOL bSwitchToSession1 = FALSE;
BOOL bRevertToSession0 = FALSE;
LPWSTR String1;

INT_PTR CALLBACK DialogProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    int Result;
	switch(uMsg)
	{
		case WM_INITDIALOG:
			DialogWnd = hwndDlg;
            SetDlgItemTextW(hwndDlg, IDC_SESSIONNAME, String1);
		break;
		case WM_CLOSE:
            Result = MessageBoxW(NULL, L"Do you wish to force applications to close on Session1 ?", L"Question", MB_YESNO);
            if(Result == IDYES)
                Result = UnloadEverything(TRUE);
            else
                Result = UnloadEverything(FALSE);
            SetThreadDesktop(Desktop0);
			if(Result)
                EndDialog(hwndDlg, 1);

			return 0;
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				// Switch to the second windows station
				case IDC_SWITCHTOSESSION1:
					bSwitchToSession1 = TRUE;
					bRestartDialog = TRUE;
					EndDialog(hwndDlg, 1);
					
					return 1;
				// Revert back to the first windows station
				case IDC_REVERTTOSESSION0:
                    bRevertToSession0 = TRUE;
					bRestartDialog = TRUE;
					EndDialog(hwndDlg, 1);

					return 1;

				case IDC_CLOSE:
					SendMessage(hwndDlg, WM_CLOSE, 0, 0);

			}
			break;
		case WM_QUERYENDSESSION:
			SendMessage(hwndDlg, WM_CLOSE, 0, 0);
        break;

        default:
			return 0;
	}
	return 0;
}

BOOL CreateMultiSessionWindow()
{
	while(bRestartDialog)
	{
		bRestartDialog = FALSE;
		if(DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_MULTISESSION), NULL, DialogProc) == -1)
			return FALSE;
		DialogWnd = NULL;
        //DEBUG("#8");
		if(bSwitchToSession1)
		{
			bSwitchToSession1 = FALSE;
			SwitchToSession1();
		}
		else if(bRevertToSession0)
		{
			bRevertToSession0 = FALSE;
			RevertToSession0();
		}
	}
	return TRUE;
}
