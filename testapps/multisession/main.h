#include <windows.h>
#include "function.h"

#define NDBG

extern HWND DialogWnd;

extern HDESK Desktop1;
extern HDESK Desktop0;

extern HWINSTA WindowStation1;

BOOL UnloadEverything(BOOL bForce);
void SwitchToSession1(void);
void RevertToSession0(void);
//void RevertToSession0(void);
BOOL CreateMultiSessionWindow();

VOID DEBUG(LPCSTR Message, ...);