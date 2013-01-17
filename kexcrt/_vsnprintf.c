#include <stdarg.h>
#include <stdio.h>

int _vsnprintf(char *buffer, size_t n, const char *format, va_list ap)
{
	return vsnprintf(buffer, n, format, ap);
}
