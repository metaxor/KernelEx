# Makefile for GNU C Compiler (GCC)

CC  = gcc
CXX = g++
RCC = windres
OBJ = msimg32.o
RES = msimgme.o
LIBS = -nostdlib -lkernel32 -lgdi32
LDFLAGS = -s -shared -Wl,--kill-at -e _DllMain@12 -Wl,--enable-stdcall-fixup
BIN = ..\..\output-i386\Release\msimg32.dll
CFLAGS = -O2 -Wall
CXXFLAGS = $(CFLAGS)

.SUFFIXES: .rc

all : $(BIN)

.PHONY : clean
clean : 
	-@if exist *.o del *.o
	-@if exist *.po del *.po
	-@if exist *.a del *.a

realclean : clean
	-@if exist $(BIN) del $(BIN)

$(BIN) : $(OBJ) $(RES)
	$(CXX) $(LDFLAGS) -o $(BIN) msimg32.def $(OBJ) $(RES) $(LIBS)

.c.o : 
	$(CC) $(CFLAGS) -c -MMD -MF $*.po -o $@ $<

.cpp.o : 
	$(CXX) $(CXXFLAGS) -c -MMD -MF $*.po -o $@ $<

.rc.o :
	$(RCC) $< $@

.def.a :
	dlltool --def $< -l $@

-include $(OBJ:.o=.po)
