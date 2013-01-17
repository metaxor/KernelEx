/*
 * ctype.h
 *
 * This assumes ISO 8859-1, being a reasonable superset of ASCII.
 */

#ifndef _CTYPE_H
#define _CTYPE_H

#if defined( _MSC_VER ) && !defined( __cplusplus )
# define inline __inline
#endif

/*
 * This relies on the following definitions:
 *
 * cntrl = !print
 * alpha = upper|lower
 * graph = punct|alpha|digit
 * blank = '\t' || ' ' (per POSIX requirement)
 */
enum {
	__ctype_upper = (1 << 0),
	__ctype_lower = (1 << 1),
	__ctype_digit = (1 << 2),
	__ctype_xdigit = (1 << 3),
	__ctype_space = (1 << 4),
	__ctype_print = (1 << 5),
	__ctype_punct = (1 << 6),
	__ctype_cntrl = (1 << 7),
};

extern const unsigned char __ctypes[];

static inline int __ctype_isalnum(int __c)
{
	return __ctypes[__c + 1] &
	    (__ctype_upper | __ctype_lower | __ctype_digit);
}

static inline int __ctype_isalpha(int __c)
{
	return __ctypes[__c + 1] & (__ctype_upper | __ctype_lower);
}

static inline int __ctype_isascii(int __c)
{
	return !(__c & ~0x7f);
}

static inline int __ctype_isblank(int __c)
{
	return (__c == '\t') || (__c == ' ');
}

static inline int __ctype_iscntrl(int __c)
{
	return __ctypes[__c + 1] & __ctype_cntrl;
}

static inline int __ctype_isdigit(int __c)
{
	return ((unsigned)__c - '0') <= 9;
}

static inline int __ctype_isgraph(int __c)
{
	return __ctypes[__c + 1] &
	    (__ctype_upper | __ctype_lower | __ctype_digit | __ctype_punct);
}

static inline int __ctype_islower(int __c)
{
	return __ctypes[__c + 1] & __ctype_lower;
}

static inline int __ctype_isprint(int __c)
{
	return __ctypes[__c + 1] & __ctype_print;
}

static inline int __ctype_ispunct(int __c)
{
	return __ctypes[__c + 1] & __ctype_punct;
}

static inline int __ctype_isspace(int __c)
{
	return __ctypes[__c + 1] & __ctype_space;
}

static inline int __ctype_isupper(int __c)
{
	return __ctypes[__c + 1] & __ctype_upper;
}

static inline int __ctype_isxdigit(int __c)
{
	return __ctypes[__c + 1] & __ctype_xdigit;
}

/* Note: this is decimal, not hex, to avoid accidental promotion to unsigned */
#define _toupper(__c) ((__c) & ~32)
#define _tolower(__c) ((__c) | 32)

static inline int __ctype_toupper(int __c)
{
	return __ctype_islower(__c) ? _toupper(__c) : __c;
}

static inline int __ctype_tolower(int __c)
{
	return __ctype_isupper(__c) ? _tolower(__c) : __c;
}

# define __CTYPEFUNC(X) \
  extern int X(int);

__CTYPEFUNC(isalnum)
    __CTYPEFUNC(isalpha)
    __CTYPEFUNC(isascii)
    __CTYPEFUNC(isblank)
    __CTYPEFUNC(iscntrl)
    __CTYPEFUNC(isdigit)
    __CTYPEFUNC(isgraph)
    __CTYPEFUNC(islower)
    __CTYPEFUNC(isprint)
    __CTYPEFUNC(ispunct)
    __CTYPEFUNC(isspace)
    __CTYPEFUNC(isupper)
    __CTYPEFUNC(isxdigit)
    __CTYPEFUNC(toupper)
    __CTYPEFUNC(tolower)
#endif				/* _CTYPE_H */
