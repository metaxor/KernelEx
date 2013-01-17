#include <windows.h>
#include "function.h"

extern HWND DialogWnd;

VOID DEBUG(const char *string);
void UnloadEverything(void);
BOOL CreateMultiSessionWindow();

typedef struct _SESSION
{
	HWINSTA WindowStation;
	ACCESS_MASK WindowStationAccessRights;
	HWINSTA InteractiveWindowStation;
	HDESK Desktop;
    HDESK DefaultDesktop;
	HANDLE hDebugPipe;
} SESSION, *PSESSION;

extern PSESSION Session;
