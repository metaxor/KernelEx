# Microsoft Developer Studio Project File - Name="KernelEx Base Shared" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=KernelEx Base Shared - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kexbases.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kexbases.mak" CFG="KernelEx Base Shared - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KernelEx Base Shared - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "KernelEx Base Shared - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KernelEx Base Shared - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNELEXBASESHARED_EXPORTS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "." /I "../../common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D _WIN32_WINNT=0x0500 /YX /FD /TP /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /i "../../common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib version.lib ../../common/KernelEx.lib ../../kexcrt/kexcrt.lib libc.lib /nologo /dll /map /machine:I386 /nodefaultlib /out:"../../output-i386/Release/kexbases.dll" /ignore:4092 /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "KernelEx Base Shared - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNELEXBASESHARED_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "." /I "../../common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D _WIN32_WINNT=0x0500 /YX /FD /TP /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /i "../../common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib version.lib ../../common/KernelEx.lib ../../kexcrt/kexcrt.lib libc.lib shlwapi.lib /nologo /dll /map /debug /machine:I386 /nodefaultlib /out:"../../output-i386/Debug/kexbases.dll" /ignore:4092 /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "KernelEx Base Shared - Win32 Release"
# Name "KernelEx Base Shared - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "kernel32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Kernel32\_kernel32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\_kernel32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\Kernel32\_kernel32_stubs.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\critsect.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\directory.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\file.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\GetConsoleWindow.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\GetModuleHandleEx.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\inter.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\IsProcessorFeaturePresent.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\jobs.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\KEXVersion.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_casemap.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_collation.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_fold.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_sortkey.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_unicode.h
# End Source File
# Begin Source File

SOURCE=.\Kernel32\locale_wctype.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\mem.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\pointer.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\port.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\process.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\RtlCaptureContext.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\session.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\thread.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\time.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\uilang.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\unikernel32.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\version.c
# End Source File
# Begin Source File

SOURCE=.\Kernel32\widecharconv.c
# End Source File
# End Group
# Begin Group "user32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\User32\_user32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\User32\_user32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\User32\_user32_stubs.c
# End Source File
# Begin Source File

SOURCE=.\User32\desktop.c
# End Source File
# Begin Source File

SOURCE=.\User32\desktop.h
# End Source File
# Begin Source File

SOURCE=.\User32\dialog.c
# End Source File
# Begin Source File

SOURCE=.\User32\display.c
# End Source File
# Begin Source File

SOURCE=.\User32\exit.c
# End Source File
# Begin Source File

SOURCE=.\User32\GetMouseMovePointsEx.c
# End Source File
# Begin Source File

SOURCE=.\User32\input.c
# End Source File
# Begin Source File

SOURCE=.\User32\LockWorkStation.c
# End Source File
# Begin Source File

SOURCE=.\User32\lstr.c
# End Source File
# Begin Source File

SOURCE=.\User32\menu.c
# End Source File
# Begin Source File

SOURCE=.\User32\message.c
# End Source File
# Begin Source File

SOURCE=.\User32\SystemParametersInfo_fix.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_conv.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_layer.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_layer.h
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_macro.h
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_proc.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_stuff.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_thunk.c
# End Source File
# Begin Source File

SOURCE=.\User32\thuni_thunk.h
# End Source File
# Begin Source File

SOURCE=.\User32\uniuser32.c
# End Source File
# Begin Source File

SOURCE=.\User32\window.c
# End Source File
# Begin Source File

SOURCE=.\User32\winsta.c
# End Source File
# End Group
# Begin Group "gdi32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Gdi32\_gdi32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\_gdi32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\Gdi32\_gdi32_stubs.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\EnumFont.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\FontResourceEx.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\GdiObjects.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\GdiObjects.h
# End Source File
# Begin Source File

SOURCE=.\Gdi32\GetGlyphOutlineA_fix.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\GetTextExtentPoint32_fix.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\MaskBlt.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\Orhpans.cpp
# End Source File
# Begin Source File

SOURCE=.\Gdi32\Orhpans.h
# End Source File
# Begin Source File

SOURCE=.\Gdi32\TextOut.c
# End Source File
# Begin Source File

SOURCE=.\Gdi32\unigdi32.c
# End Source File
# End Group
# Begin Group "advapi32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Advapi32\_advapi32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\_advapi32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\Advapi32\_advapi32_stubs.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\lsa.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\OpenSCManager_stub.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\reg.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\RtlGenRandom.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\security.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\security.h
# End Source File
# Begin Source File

SOURCE=.\Advapi32\TraceMessage.c
# End Source File
# Begin Source File

SOURCE=.\Advapi32\uniadvapi32.c
# End Source File
# End Group
# Begin Group "comdlg32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\comdlg32\_comdlg32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\comdlg32\_comdlg32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\comdlg32\PrintDlgEx.c
# End Source File
# End Group
# Begin Group "version"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\version\_version_apilist.c
# End Source File
# Begin Source File

SOURCE=.\version\_version_apilist.h
# End Source File
# Begin Source File

SOURCE=.\version\universion.c
# End Source File
# End Group
# Begin Group "shell32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\shell32\_shell32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\shell32\_shell32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\shell32\_shell32_stubs.c
# End Source File
# Begin Source File

SOURCE=.\shell32\CommandLineToArgvW.c
# End Source File
# Begin Source File

SOURCE=.\shell32\IsUserAnAdmin.c
# End Source File
# Begin Source File

SOURCE=.\shell32\shexec.c
# End Source File
# Begin Source File

SOURCE=.\shell32\shexit.c
# End Source File
# End Group
# Begin Group "comctl32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\comctl32\_comctl32_apilist.c
# End Source File
# Begin Source File

SOURCE=.\comctl32\_comctl32_apilist.h
# End Source File
# Begin Source File

SOURCE=.\comctl32\newclassreg.c
# End Source File
# Begin Source File

SOURCE=.\comctl32\syslink.c
# End Source File
# End Group
# Begin Group "ntdll"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ntdll\_ntdll_apilist.c
# End Source File
# Begin Source File

SOURCE=.\ntdll\_ntdll_apilist.h
# End Source File
# Begin Source File

SOURCE=.\ntdll\_ntdll_stubs.c
# End Source File
# Begin Source File

SOURCE=.\ntdll\ntprocess.c
# End Source File
# Begin Source File

SOURCE=.\ntdll\ntthread.c
# End Source File
# Begin Source File

SOURCE=.\ntdll\sys.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\common\common.c
# End Source File
# Begin Source File

SOURCE=.\dirlist

!IF  "$(CFG)" == "KernelEx Base Shared - Win32 Release"

# Begin Custom Build
WkspDir=.
ProjDir=.
InputPath=.\dirlist

"&" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(WkspDir)\util\prep\Release\prep.exe" goto error 
	"$(WkspDir)\util\prep\Release\prep.exe" "$(ProjDir)" 
	goto quit 
	:error 
	echo Error - compile PREP (Release) project first 
	echo 1 | choice /C:1 /N >NUL 
	:quit 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "KernelEx Base Shared - Win32 Debug"

# Begin Custom Build
WkspDir=.
ProjDir=.
InputPath=.\dirlist

"&" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(WkspDir)\util\prep\Release\prep.exe" goto error 
	"$(WkspDir)\util\prep\Release\prep.exe" "$(ProjDir)" 
	goto quit 
	:error 
	echo Error - compile PREP (Release) project first 
	echo 1 | choice /C:1 /N >NUL 
	:quit 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kexbases.def

!IF  "$(CFG)" == "KernelEx Base Shared - Win32 Release"

# Begin Custom Build
OutDir=.\Release
WkspDir=.
InputPath=.\kexbases.def

"$(OutDir)\k32ord.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /nologo /c /TC /DK32ORD_IMPLIB >"%TEMP%\resp1455.tmp" 
	echo /Fo"$(OutDir)\k32ord.obj" >>"%TEMP%\resp1455.tmp" 
	echo "$(WkspDir)\common\k32ord.h" >>"%TEMP%\resp1455.tmp" 
	cl @"%TEMP%\resp1455.tmp" 
	del "%TEMP%\resp1455.tmp" >NUL 
	echo /DLL /NOENTRY /NOLOGO /IGNORE:4070 /MACHINE:IX86 >"%TEMP%\resp1456.tmp" 
	echo /DEF:"$(WkspDir)\common\k32ord.def" >>"%TEMP%\resp1456.tmp" 
	echo /OUT:"$(OutDir)\k32ord.dll" >>"%TEMP%\resp1456.tmp" 
	echo /IMPLIB:"$(OutDir)\k32ord.lib" >>"%TEMP%\resp1456.tmp" 
	echo "$(OutDir)\k32ord.obj" >>"%TEMP%\resp1456.tmp" 
	link @"%TEMP%\resp1456.tmp" 
	del "%TEMP%\resp1456.tmp" >NUL 
	del "$(OutDir)\k32ord.exp" >NUL 
	del "$(OutDir)\k32ord.obj" >NUL 
	del "$(OutDir)\k32ord.dll" >NUL 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "KernelEx Base Shared - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
WkspDir=.
InputPath=.\kexbases.def

"$(OutDir)\k32ord.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /nologo /c /TC /DK32ORD_IMPLIB >"%TEMP%\resp1455.tmp" 
	echo /Fo"$(OutDir)\k32ord.obj" >>"%TEMP%\resp1455.tmp" 
	echo "$(WkspDir)\common\k32ord.h" >>"%TEMP%\resp1455.tmp" 
	cl @"%TEMP%\resp1455.tmp" 
	del "%TEMP%\resp1455.tmp" >NUL 
	echo /DLL /NOENTRY /NOLOGO /IGNORE:4070 /MACHINE:IX86 >"%TEMP%\resp1456.tmp" 
	echo /DEF:"$(WkspDir)\common\k32ord.def" >>"%TEMP%\resp1456.tmp" 
	echo /OUT:"$(OutDir)\k32ord.dll" >>"%TEMP%\resp1456.tmp" 
	echo /IMPLIB:"$(OutDir)\k32ord.lib" >>"%TEMP%\resp1456.tmp" 
	echo "$(OutDir)\k32ord.obj" >>"%TEMP%\resp1456.tmp" 
	link @"%TEMP%\resp1456.tmp" 
	del "%TEMP%\resp1456.tmp" >NUL 
	del "$(OutDir)\k32ord.exp" >NUL 
	del "$(OutDir)\k32ord.obj" >NUL 
	del "$(OutDir)\k32ord.dll" >NUL 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\common\common.h
# End Source File
# Begin Source File

SOURCE=.\User32\hwnd9x.h
# End Source File
# Begin Source File

SOURCE=..\..\common\k32ord.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\common\k32ord.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\kexbases.rc
# End Source File
# End Group
# End Target
# End Project
