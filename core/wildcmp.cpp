//public domain wildcard match function by Jack Handy
#include "wildcmp.h"

int wildcmp(const char *wild, const char *string) {
    const char *cp = 0, *mp = 0;
    while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}
 
	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {	
			wild++;
			string++;
		} else { 
			if (!cp || !mp) 
				return 0; 
			wild = mp; 
			string = cp++; 
		}
    }
 
	while (*wild == '*') {
        wild++;
    }
 
	return !*wild;
}
