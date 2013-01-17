#include "main.h"

/*
	Switch to our personnal space,
	the user can do what he wants
	from his personnal space.
*/

void SwitchToSession1(void)
{
	int ret;
	TCHAR buffer[255];
	ZeroMemory(&buffer, sizeof(buffer));

	ret = SetThreadDesktop(Desktop1);
	if(!ret)
	{
		wsprintf(buffer, "Failed to set the thread's desktop (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
		return;
	}

	ret = SwitchDesktop(Desktop1);
	if(!ret)
	{
        SetThreadDesktop(Desktop0);
		wsprintf(buffer, "Switching to the desktop's Window Station has failed (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
	}
}

/*
	Let's go back to the default desktop
*/

void RevertToSession0(void)
{
	int ret;
	TCHAR buffer[255];
	ZeroMemory(&buffer, sizeof(buffer));

	ret = SetThreadDesktop(Desktop0);
	if(!ret)
	{
		wsprintf(buffer, "Failed to set the thread's desktop (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
		return;
	}

	ret = SwitchDesktop(Desktop0);
	if(!ret)
	{
        SetThreadDesktop(Desktop1);
		wsprintf(buffer, "Switching to the desktop's Window Station has failed (error %d)", GetLastError());
		MessageBox(NULL, buffer, "Error", MB_ICONWARNING);
	}
}
