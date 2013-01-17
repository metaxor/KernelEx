#include <windows.h>
#include "init.h"

extern BOOL WINAPI DllMain(
        HINSTANCE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpReserved
);

BOOL WINAPI _DllMainCRTStartup(
        HINSTANCE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpReserved
        )
{
	BOOL ret;
	
    if (dwReason == DLL_PROCESS_ATTACH)
        __init();

	ret = DllMain(hDllHandle, dwReason, lpReserved);

    if (dwReason == DLL_PROCESS_DETACH)
        __exit();

    return ret;
}
