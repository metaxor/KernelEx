#include <windows.h>
#include <stdio.h>

typedef PROC (*al_register_t)(const char*, const char*, const char*, PROC);
typedef int (*init_once_t)(void);
typedef int (WINAPI *MessageBox_t)(HWND, LPCWSTR, LPCWSTR, UINT);

int main()
{
	MessageBox_t msgbox;
	HMODULE hUser32 = LoadLibrary("user32.dll");
	msgbox = (MessageBox_t) GetProcAddress(hUser32, "MessageBoxW");

	al_register_t al_register;
	init_once_t init_once;	
	HMODULE hApiHookDll = LoadLibrary("ApiLog.dll");
	init_once = (init_once_t) GetProcAddress(hApiHookDll, "kexApiHook_initonce");
	al_register = (al_register_t) GetProcAddress(hApiHookDll, "kexApiHook_register");
	
	if (!init_once || !al_register)
	{
		puts("Failed to load api hook dll");
		return 1;
	}
	if (!init_once())
	{
		puts("Failed to init api hook dll");
		return 1;
	}
	
	msgbox = (MessageBox_t) al_register("SOURCE", "TARGET", "TestApi", (PROC) msgbox);
	msgbox(NULL, L"This is a test message", L"Caption", MB_OK | MB_ICONINFORMATION);
	
	FreeLibrary(hUser32);
	FreeLibrary(hApiHookDll);
	return 0;
}
