# Microsoft Developer Studio Project File - Name="kexcrt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KEXCRT - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kexcrt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kexcrt.mak" CFG="KEXCRT - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kexcrt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /O2 /I "." /I "msvc" /D "_CTYPE_DISABLE_MACROS" /FD /Oi- /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Target

# Name "kexcrt - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_vsnprintf.c
# End Source File
# Begin Source File

SOURCE=.\abort.c
# End Source File
# Begin Source File

SOURCE=.\msvc\argcargv.c
# End Source File
# Begin Source File

SOURCE=.\assert.c
# End Source File
# Begin Source File

SOURCE=.\atoi.c
# End Source File
# Begin Source File

SOURCE=.\atol.c
# End Source File
# Begin Source File

SOURCE=.\atoll.c
# End Source File
# Begin Source File

SOURCE=.\msvc\concrt0.c
# End Source File
# Begin Source File

SOURCE=.\ctypes.c
# End Source File
# Begin Source File

SOURCE=.\msvc\dllcrt0.c
# End Source File
# Begin Source File

SOURCE=.\exit.c
# End Source File
# Begin Source File

SOURCE=.\msvc\init.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isalnum.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isalpha.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isascii.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isblank.c
# End Source File
# Begin Source File

SOURCE=.\ctype\iscntrl.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isdigit.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isgraph.c
# End Source File
# Begin Source File

SOURCE=.\ctype\islower.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isprint.c
# End Source File
# Begin Source File

SOURCE=.\ctype\ispunct.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isspace.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isupper.c
# End Source File
# Begin Source File

SOURCE=.\ctype\isxdigit.c
# End Source File
# Begin Source File

SOURCE=.\memccpy.c
# End Source File
# Begin Source File

SOURCE=.\memchr.c
# End Source File
# Begin Source File

SOURCE=.\memcmp.c
# End Source File
# Begin Source File

SOURCE=.\memcpy.c
# End Source File
# Begin Source File

SOURCE=.\memmem.c
# End Source File
# Begin Source File

SOURCE=.\memmove.c
# End Source File
# Begin Source File

SOURCE=".\memory-cpp.cpp"
# End Source File
# Begin Source File

SOURCE=.\memory.c
# End Source File
# Begin Source File

SOURCE=.\memrchr.c
# End Source File
# Begin Source File

SOURCE=.\memset.c
# End Source File
# Begin Source File

SOURCE=.\memswap.c
# End Source File
# Begin Source File

SOURCE=.\printf.c
# End Source File
# Begin Source File

SOURCE=.\msvc\purecall.c
# End Source File
# Begin Source File

SOURCE=.\snprintf.c
# End Source File
# Begin Source File

SOURCE=.\sprintf.c
# End Source File
# Begin Source File

SOURCE=.\sscanf.c
# End Source File
# Begin Source File

SOURCE=.\strcat.c
# End Source File
# Begin Source File

SOURCE=.\strchr.c
# End Source File
# Begin Source File

SOURCE=.\strcmp.c
# End Source File
# Begin Source File

SOURCE=.\strcmpi.c
# End Source File
# Begin Source File

SOURCE=.\strcpy.c
# End Source File
# Begin Source File

SOURCE=.\strdup.c
# End Source File
# Begin Source File

SOURCE=.\strlen.c
# End Source File
# Begin Source File

SOURCE=.\strncat.c
# End Source File
# Begin Source File

SOURCE=.\strncmp.c
# End Source File
# Begin Source File

SOURCE=.\strncpy.c
# End Source File
# Begin Source File

SOURCE=.\strnicmp.c
# End Source File
# Begin Source File

SOURCE=.\strnlen.c
# End Source File
# Begin Source File

SOURCE=.\strntoimax.c
# End Source File
# Begin Source File

SOURCE=.\strntoumax.c
# End Source File
# Begin Source File

SOURCE=.\strpbrk.c
# End Source File
# Begin Source File

SOURCE=.\strrchr.c
# End Source File
# Begin Source File

SOURCE=.\strsep.c
# End Source File
# Begin Source File

SOURCE=.\strstr.c
# End Source File
# Begin Source File

SOURCE=.\strtok.c
# End Source File
# Begin Source File

SOURCE=.\strtok_r.c
# End Source File
# Begin Source File

SOURCE=.\strtol.c
# End Source File
# Begin Source File

SOURCE=.\strtoll.c
# End Source File
# Begin Source File

SOURCE=.\strtoul.c
# End Source File
# Begin Source File

SOURCE=.\strtoull.c
# End Source File
# Begin Source File

SOURCE=.\strtoumax.c
# End Source File
# Begin Source File

SOURCE=.\strupr.c
# End Source File
# Begin Source File

SOURCE=.\strxspn.c
# End Source File
# Begin Source File

SOURCE=.\ctype\tolower.c
# End Source File
# Begin Source File

SOURCE=.\ctype\toupper.c
# End Source File
# Begin Source File

SOURCE=.\vsnprintf.c
# End Source File
# Begin Source File

SOURCE=.\vsprintf.c
# End Source File
# Begin Source File

SOURCE=.\vsscanf.c
# End Source File
# Begin Source File

SOURCE=.\msvc\wincrt0.c
# End Source File
# Begin Source File

SOURCE=.\write.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\msvc\argcargv.h
# End Source File
# Begin Source File

SOURCE=.\ctype\ctypefunc.h
# End Source File
# Begin Source File

SOURCE=.\ctypes.h
# End Source File
# Begin Source File

SOURCE=.\msvc\init.h
# End Source File
# Begin Source File

SOURCE=.\strxspn.h
# End Source File
# End Group
# End Target
# End Project
