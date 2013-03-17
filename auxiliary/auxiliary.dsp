# Microsoft Developer Studio Project File - Name="auxiliary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=auxiliary - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "auxiliary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "auxiliary.mak" CFG="auxiliary - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "auxiliary - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f auxiliary.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "auxiliary.exe"
# PROP BASE Bsc_Name "auxiliary.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /nologo /f makefile.msv ide"
# PROP Rebuild_Opt "/a"
# PROP Target_File "auxiliary"
# PROP Bsc_Name ""
# PROP Target_Dir ""
# Begin Target

# Name "auxiliary - Win32 Release"

!IF  "$(CFG)" == "auxiliary - Win32 Release"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "msimg32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msimg32\msimg32.c
# End Source File
# Begin Source File

SOURCE=.\msimg32\msimgme.rc
# End Source File
# End Group
# Begin Group "pdh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdh\pdh.c
# End Source File
# End Group
# Begin Group "psapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\psapi\psapi.c
# End Source File
# End Group
# Begin Group "userenv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userenv\userenv.c
# End Source File
# End Group
# Begin Group "uxtheme"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\uxtheme\metric.c
# End Source File
# Begin Source File

SOURCE=.\uxtheme\uxtheme.c
# End Source File
# End Group
# Begin Group "wtsapi32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wtsapi32\wtsapi32.c
# End Source File
# Begin Source File

SOURCE=.\wtsapi32\wtsapi32.h
# End Source File
# End Group
# Begin Group "winsta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\winsta\winsta.c
# End Source File
# Begin Source File

SOURCE=.\winsta\winsta.h
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Definition Files"

# PROP Default_Filter ""
# Begin Group "uxtheme_kord"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\uxtheme\kord.def
# End Source File
# End Group
# Begin Group "pdh_kord"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdh\kord.def
# End Source File
# End Group
# Begin Source File

SOURCE=.\msimg32\msimg32.def
# End Source File
# Begin Source File

SOURCE=.\netapi32\netapi32.def
# End Source File
# Begin Source File

SOURCE=.\pdh\pdh.def
# End Source File
# Begin Source File

SOURCE=.\psapi\psapi.def
# End Source File
# Begin Source File

SOURCE=.\userenv\userenv.def
# End Source File
# Begin Source File

SOURCE=.\uxtheme\uxtheme.def
# End Source File
# Begin Source File

SOURCE=.\winsta\winsta.def
# End Source File
# Begin Source File

SOURCE=.\wtsapi32\wtsapi32.def
# End Source File
# End Group
# End Target
# End Project
