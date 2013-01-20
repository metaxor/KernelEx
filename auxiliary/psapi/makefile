# Makefile for GNU C Compiler (GCC)

CC  = gcc
CXX = g++
RCC = windres
OBJ = psapi.o
RES = 
DEF = psapi.def
LIBS = -nostdlib -lkernel32 -L../../common -lkernelex
LDFLAGS = -s -shared -Wl,--enable-stdcall-fixup -e _DllMain@12
BIN = ..\..\output-i386\Release\psapi.dll
CFLAGS = -Os -Wall -I../../common
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
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(RES) $(DEF) $(LIBS)

.c.o : 
	$(CC) $(CFLAGS) -c -MMD -MF $*.po -o $@ $<

.cpp.o : 
	$(CXX) $(CXXFLAGS) -c -MMD -MF $*.po -o $@ $<

.rc.o :
	$(RCC) $< $@

.def.a :
	dlltool --def $< -l $@

-include $(OBJ:.o=.po)
