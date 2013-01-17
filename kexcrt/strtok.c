/*
 * strtok.c
 */

char *strsep(char **stringp, const char *delim);

char *strtok(char *s, const char *delim)
{
	static char *holder;

	if (s)
		holder = s;

	do {
		s = strsep(&holder, delim);
	} while (s && !*s);

	return s;
}
