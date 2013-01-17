/*
 * printf.c
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define BUFFER_SIZE	16384

int printf(const char *format, ...)
{
	va_list ap;
	int rv;
	char buffer[BUFFER_SIZE];

	va_start(ap, format);
	rv = vsnprintf(buffer, BUFFER_SIZE, format, ap);
	va_end(ap);

	if (rv < 0)
		return rv;

	if (rv > BUFFER_SIZE - 1)
		rv = BUFFER_SIZE - 1;

	return write(1, buffer, rv);
}
