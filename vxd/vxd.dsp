# Microsoft Developer Studio Project File - Name="VxD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=VxD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vxd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vxd.mak" CFG="VxD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VxD - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "VxD - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VxD - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /D "NDEBUG" /FD /GF /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vxdwraps.clb libc.lib /nologo /map /machine:I386 /nodefaultlib /out:"../output-i386/Release/VKrnlEx.vxd" /vxd /ignore:4078 /ignore:4039
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "VxD - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /D "_DEBUG" /FD /GF /Gs /c
# SUBTRACT CPP /Gy
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vxdwraps.clb libc.lib /nologo /incremental:no /map /debug /machine:I386 /nodefaultlib /out:"../output-i386/Debug/VKrnlEx.vxd" /vxd /ignore:4078 /ignore:4039
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "VxD - Win32 Release"
# Name "VxD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\debug.cpp

!IF  "$(CFG)" == "VxD - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VxD - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KernelEx.def
# End Source File
# Begin Source File

SOURCE=.\patch.cpp
# End Source File
# Begin Source File

SOURCE=.\patch_ifsmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\patch_kernel32.cpp
# End Source File
# Begin Source File

SOURCE=.\pemanip.cpp
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# Begin Source File

SOURCE=.\vxdmain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\interface.h
# End Source File
# Begin Source File

SOURCE=.\patch.h
# End Source File
# Begin Source File

SOURCE=.\patch_ifsmgr.h
# End Source File
# Begin Source File

SOURCE=.\patch_kernel32.h
# End Source File
# Begin Source File

SOURCE=.\pemanip.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\vxdmain.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
