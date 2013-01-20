# Makefile for GNU C Compiler (GCC)

CC  = gcc
CXX = g++
RCC = windres
OBJ = uxtheme.o
RES = 
DEF = uxtheme.def
LIBS = kord.a -nostdlib -lkernel32
LDFLAGS = -s -shared -Wl,--enable-stdcall-fixup -e _DllMain@12
BIN = ..\..\output-i386\Release\uxtheme.dll
CFLAGS = -Os -Wall
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

$(BIN) : $(OBJ) $(RES) kord.a
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(RES) $(LIBS) $(DEF)

.c.o : 
	$(CC) $(CFLAGS) -c -MMD -MF $*.po -o $@ $<

.cpp.o : 
	$(CXX) $(CXXFLAGS) -c -MMD -MF $*.po -o $@ $<

.rc.o :
	$(RCC) $< $@

.def.a :
	dlltool --def $< -l $@

-include $(OBJ:.o=.po)
