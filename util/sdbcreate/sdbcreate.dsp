# Microsoft Developer Studio Project File - Name="sdbcreate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sdbcreate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdbcreate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdbcreate.mak" CFG="sdbcreate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdbcreate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sdbcreate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdbcreate - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /FI"../../common/msvc_quirks.h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shlwapi.lib kernel32.lib shell32.lib /nologo /subsystem:console /machine:I386 /out:"../../output-i386/Release/sdbcreate.exe"
# Begin Special Build Tool
ProjDir=.
TargetPath=\Projects\KernelEx\output-i386\Release\sdbcreate.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=$(TargetPath) $(ProjDir)\sdbdb\kexsdb.ini $(ProjDir)\sdbdb\KernelEx.sdb $(ProjDir)\sdbdb\kexsdb.reg
# End Special Build Tool

!ELSEIF  "$(CFG)" == "sdbcreate - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /FI"../../common/msvc_quirks.h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shlwapi.lib kernel32.lib shell32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../output-i386/Debug/sdbcreate.exe" /pdbtype:sept
# Begin Special Build Tool
ProjDir=.
TargetPath=\Projects\KernelEx\output-i386\Debug\sdbcreate.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=$(TargetPath) $(ProjDir)\sdbdb\kexsdb.ini $(ProjDir)\sdbdb\KernelEx.sdb $(ProjDir)\sdbdb\kexsdb.reg
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "sdbcreate - Win32 Release"
# Name "sdbcreate - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sdbcreate.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\sdbapi.h

!IF  "$(CFG)" == "sdbcreate - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Release
ProjDir=.
InputPath=.\sdbapi.h

"$(OutDir)\sdbapi.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /nologo /c /TC /DSDBAPI_IMPLIB >"%TEMP%\resp1455.tmp" 
	echo /Fo"$(OutDir)\sdbapi.obj" >>"%TEMP%\resp1455.tmp" 
	echo "$(ProjDir)\sdbapi.h" >>"%TEMP%\resp1455.tmp" 
	cl @"%TEMP%\resp1455.tmp" 
	del "%TEMP%\resp1455.tmp" >NUL 
	echo /DLL /NOENTRY /NOLOGO /IGNORE:4070 /MACHINE:IX86 >"%TEMP%\resp1456.tmp" 
	echo /DEF:"$(ProjDir)\sdbapi.def" >>"%TEMP%\resp1456.tmp" 
	echo /OUT:"$(OutDir)\sdbapi.dll" >>"%TEMP%\resp1456.tmp" 
	echo /IMPLIB:"$(OutDir)\sdbapi.lib" >>"%TEMP%\resp1456.tmp" 
	echo "$(OutDir)\sdbapi.obj" >>"%TEMP%\resp1456.tmp" 
	link @"%TEMP%\resp1456.tmp" 
	del "%TEMP%\resp1456.tmp" >NUL 
	del "$(OutDir)\sdbapi.exp" >NUL 
	del "$(OutDir)\sdbapi.obj" >NUL 
	del "$(OutDir)\sdbapi.dll" >NUL 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "sdbcreate - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
ProjDir=.
InputPath=.\sdbapi.h

"$(OutDir)\sdbapi.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /nologo /c /TC /DSDBAPI_IMPLIB >"%TEMP%\resp1455.tmp" 
	echo /Fo"$(OutDir)\sdbapi.obj" >>"%TEMP%\resp1455.tmp" 
	echo "$(ProjDir)\sdbapi.h" >>"%TEMP%\resp1455.tmp" 
	cl @"%TEMP%\resp1455.tmp" 
	del "%TEMP%\resp1455.tmp" >NUL 
	echo /DLL /NOENTRY /NOLOGO /IGNORE:4070 /MACHINE:IX86 >"%TEMP%\resp1456.tmp" 
	echo /DEF:"$(ProjDir)\sdbapi.def" >>"%TEMP%\resp1456.tmp" 
	echo /OUT:"$(OutDir)\sdbapi.dll" >>"%TEMP%\resp1456.tmp" 
	echo /IMPLIB:"$(OutDir)\sdbapi.lib" >>"%TEMP%\resp1456.tmp" 
	echo "$(OutDir)\sdbapi.obj" >>"%TEMP%\resp1456.tmp" 
	link @"%TEMP%\resp1456.tmp" 
	del "%TEMP%\resp1456.tmp" >NUL 
	del "$(OutDir)\sdbapi.exp" >NUL 
	del "$(OutDir)\sdbapi.obj" >NUL 
	del "$(OutDir)\sdbapi.dll" >NUL 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wstring.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
