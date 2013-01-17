#include <windows.h>
#include <stdio.h>

typedef int (* initproc_t)();

int main()
{
	HMODULE h;
	initproc_t init;
	initproc_t uninit;
	DWORD starttime;
	int i;
	
	h = LoadLibrary("KernelEx.dll");
	if (h)
	{
		printf("KernelEx successfully loaded @ 0x%08x\n", h);
	}
	else
	{
		printf("Load error\n");
	}
	
	init = (initproc_t) GetProcAddress(h, "kexInit");
	uninit = (initproc_t) GetProcAddress(h, "kexUninit");
	
	starttime = GetTickCount();
	i = init();
	if (i)
		printf("init completed (%d) in %d ms\n", i, GetTickCount() - starttime);
	else
		printf("init failure\n");
		
	printf("Press ENTER.\n");
	getchar();
	
	i = uninit();
	printf("uninit complete (%d)\n", i);
	
	FreeLibrary(h);
	return 0;
}
