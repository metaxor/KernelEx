#include <windows.h>

#define	STDIN_FILENO  0
#define	STDOUT_FILENO 1
#define	STDERR_FILENO 2

int __cdecl write(int fd, const void* buf, unsigned int size)
{
	DWORD written;
	HANDLE hFile;
	
	switch (fd)
	{
	case STDIN_FILENO:
		hFile = GetStdHandle(STD_INPUT_HANDLE);
		break;
	case STDOUT_FILENO:
		hFile = GetStdHandle(STD_OUTPUT_HANDLE);
		break;
	case STDERR_FILENO:
		hFile = GetStdHandle(STD_ERROR_HANDLE);
		break;
	default:
		hFile = (HANDLE) fd;
	}
	
	if (!WriteFile(hFile, buf, size, &written, NULL))
		return -1;
	return written;
	
}

int __cdecl _write(int fd, const void* buf, unsigned int size)
{
	return write(fd, buf, size);
}
