OBJ = abort.o assert.o atoi.o atol.o atoll.o ctypes.o memccpy.o memchr.o memcmp.o memcpy.o memmem.o memmove.o memory.o memory-cpp.o memrchr.o memset.o memswap.o printf.o snprintf.o sprintf.o sscanf.o strcat.o strchr.o strcmp.o strcmpi.o strcpy.o strdup.o strlen.o strncat.o strncmp.o strncpy.o strnicmp.o strnlen.o strntoimax.o strntoumax.o strpbrk.o strrchr.o strsep.o strstr.o strtok.o strtok_r.o strtol.o strtoll.o strtoul.o strtoull.o strtoumax.o strupr.o strxspn.o vsnprintf.o vsprintf.o vsscanf.o _vsnprintf.o write.o exit.o \
ctype/isalnum.o ctype/isalpha.o ctype/isascii.o ctype/isblank.o ctype/iscntrl.o ctype/isdigit.o ctype/isgraph.o ctype/islower.o ctype/isprint.o ctype/ispunct.o ctype/isspace.o ctype/isupper.o ctype/isxdigit.o ctype/tolower.o ctype/toupper.o \
gcc/init.o gcc/dllcrt0.o gcc/pseudo-reloc.o gcc/pseudo-reloc-list.o gcc/purecall.o

CC = gcc
CXX = g++
CFLAGS = -O2 -s -I. -D__NO_CTYPE_INLINES -D__NO_ISOCEXT

BIN = libkexcrt.a

all : $(BIN)

clean :
	@del *.o >NUL
	@del ctype\*.o >NUL
	@del gcc\*.o >NUL

$(BIN) : $(OBJ)
	ar qcs $@ $(OBJ)

.c.o : 
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o : 
	$(CXX) $(CFLAGS) -c -o $@ $<
