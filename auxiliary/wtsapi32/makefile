# Makefile for GNU C Compiler (GCC)

CC  = gcc
CXX = g++
RCC = windres
OBJ = wtsapi32.o
RES = 
LIBS = ..\winsta\winsta.a ..\..\common\KernelEx.lib
DEF = wtsapi32.def
LDFLAGS = -s -shared -Wl,--kill-at
BIN = ..\..\output-i386\Release\wtsapi32.dll
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

$(BIN) : $(OBJ) $(RES) ..\winsta\winsta.a
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(RES) $(LIBS)  $(DEF)

.c.o : 
	$(CC) $(CFLAGS) -c -MMD -MF $*.po -o $@ $<

.cpp.o : 
	$(CXX) $(CXXFLAGS) -c -MMD -MF $*.po -o $@ $<

.rc.o :
	$(RCC) $< $@

.def.a :
	dlltool --def $< -l $@

-include $(OBJ:.o=.po)
