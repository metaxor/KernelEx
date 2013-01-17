#include "main.h"

PROCESS_INFORMATION pi;
STARTUPINFO si;

HDESK Desktop1 = NULL;
HDESK Desktop0 = NULL;
HWINSTA WindowStation1 = NULL;

#ifdef NDBG
VOID DEBUG(LPCSTR Message, ...)
{
    LPSTR buffer;
    va_list args = NULL;

    va_start(args, Message);

    //sprintf(buffer, format, args);
    FormatMessage(FORMAT_MESSAGE_FROM_STRING |
				  FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  Message, 
                  0,
                  0,
                  (LPSTR)&buffer, 
                  0, 
                  &args);

    va_end(args);

    MessageBox(NULL, buffer, NULL, 0);
}
#else
VOID DEBUG(LPCSTR Message, ...)
{
}
#endif

/*
	Closes all process from Session1 space
*/
BOOL CALLBACK EnumWindowsProc(HWND hwnd,
    LPARAM lParam
)
{
	DWORD dwResult = -1;
	DWORD dwProcessId = 0;
	DWORD dwThreadId = 0;
	HANDLE hProcess;

	dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);
	if(dwProcessId == GetCurrentProcessId() || dwProcessId == pi.dwProcessId)
		return TRUE;

	if(lParam == 0)
	{
		SendMessageTimeout(hwnd, WM_QUERYENDSESSION, 0, ENDSESSION_LOGOFF, SMTO_NORMAL, 5000, &dwResult);
        if(dwResult == FALSE || GetLastError() == 0)
            return FALSE;
		SendMessageTimeout(hwnd, WM_ENDSESSION, TRUE, ENDSESSION_LOGOFF, SMTO_NORMAL, 1000, &dwResult);
		if(dwProcessId)
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
		if(hProcess)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
	else if(lParam == 1)
	{
		if(dwProcessId)
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
		if(hProcess)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
	return TRUE;
}

BOOL UnloadEverything(BOOL bForce)
{
    int i;
    SwitchDesktop(Desktop1);
    if(!EnumDesktopWindows(Desktop1, EnumWindowsProc, bForce))
    {
        SwitchDesktop(Desktop0);
        MessageBoxW(NULL, L"Some applications on Session1 doesn't want to close.", L"Warning", MB_ICONWARNING);
        return FALSE;
    }

    for(i=0;i<=255;i++)
        EnumDesktopWindows(Desktop1, EnumWindowsProc, bForce);

    TerminateProcess(pi.hProcess, 1);

    CloseHandle(pi.hProcess);

    Sleep(250);

	if(Desktop0)
	{
		SwitchDesktop(Desktop0);
		SetThreadDesktop(Desktop0);
	}

	if(Desktop1)
		CloseDesktop(Desktop1);

	ExitProcess(0);

    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
	int ret;
	TCHAR buffer[255];
	TCHAR *ExplorerString = "C:\\WINDOWS\\TASKMAN.EXE";
	DWORD ThreadId;

    DEBUG("#1");

    WindowStation1 = GetProcessWindowStation();

	DEBUG("#2");

	// Create personnals spaces
	ThreadId = GetCurrentThreadId();

    Desktop0 = GetThreadDesktop(ThreadId);//OpenDesktop("Default", 0, FALSE, GENERIC_ALL);
	DEBUG("#3");
    Desktop1 = CreateDesktop("Session1", NULL, NULL, 0, GENERIC_ALL, NULL);

    wsprintf(buffer, "Desktop1 = 0x%X", Desktop1);
	DEBUG(buffer);
	if(Desktop1 == NULL)
	{
		DEBUG("#5");
		wsprintf(buffer, "Error while creating the desktop: (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
		return 0;
	}

	DEBUG("#6");
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.lpDesktop = "Session1";
	si.cb = sizeof(STARTUPINFO);

    DEBUG("#7");
	ret = CreateProcess(ExplorerString, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // Let's put the shell inside
	//if(!ret)
	//{
	//	UnloadEverything(TRUE);
	//	return 0;
	//}
	CloseHandle(pi.hThread);

    DEBUG("#8");
	if(!CreateMultiSessionWindow())
	{
		UnloadEverything(TRUE);
		return 0;
	}
	UnloadEverything(TRUE);

	return 1;
}
