#include <windows.h>

void __exit(void);

void __cdecl exit(int v)
{
	__exit();
	ExitProcess(v);
}
