!ifndef UPGRADEDLL_FUNC_INCLUDED
!define UPGRADEDLL_FUNC_INCLUDED
 
; Function - Upgrade DLL File
; Written by Opher Shachar on 2004/01/14
; Revised on 2004/01/15: Supports registering *.TLB, *.OLB files with 'regtlib.exe'
;     Needs 'GetFileExt' (http://nsis.sourceforge.net/wiki/Get_extention_of_file)
; Revised 2007/02/06: fix directory as source, skip '.' and '..' entries
;     $R6 now is VOLATILE
; Revised 2009/02 by Xeno86: tempdir = localdir, no tlb register, erase source 
;     file if no upgrade
; Based on macro by Joost Verburg
; --------------------------------------
;
; Parameters:
; $R4 = DESTFILE    - Location of the DLL file that should be upgraded (on user's system)
;    OR DESTDIR     - Target directory for upgraded DLLs
; $R6 = REGISTER    - if you want to upgrade a DLL that has to be registered.
;       0 = Don't register, anything else = Register  -- note: VOLATILE
; $R7 = LOCALFILE   - Location of the new DLL file (on the user's system)
;    OR LOCALDIR    - Source directory of newer DLLs
; Note: If you want to support Win9x, you can only use short filenames (8.3).
;
; Example of usage for single file:
; $R4 = "$SYSDIR\dllname.dll"
; $R6 = "1" (= Register DLL)
; $R7 = "Source_dir\dllname.dll"
; Call UpgradeDLL_Func
;
; Example of usage with File command:
; SetOutPath "$PLUGINSDIR\SysFiles"
; File /r "sys_files_to_be_installed\*.*" ...
; ...
; File "some_other_sys_file.dll"
; $R4 = "$SYSDIR"               -- note no '\', MUST be an existing directory
; $R6 = "1" (= Register DLL)
; $R7 = "$PLUGINSDIR\SysFiles"  -- note no '\'
; Call UpgradeDLL_Func
; RMDir /r "$PLUGINSDIR\SysFiles"
;
; Example of usage for whole bunch of external files:
; $R4 = "$SYSDIR"       -- note no '\', MUST be an existing directory
; $R6 = "1" (= Register DLL)
; $R7 = "Source_dir"    -- note no '\'
; Call UpgradeDLL_Func
 
Function UpgradeDLL_Func
 
  Push $R0
  Push $R1
  Push $R2
  Push $R3
 
  SetOverwrite try
 
  ;------------------------
  ;Check single file
 
  IfFileExists "$R7\*.*" +3
    Call :upgradedll.doit
    Goto upgradedll.end
 
  ;------------------------
  ;Enumerate directory
 
  Push $0
  Push $1
  Push $2
  StrCpy $1 $R7
  StrCpy $2 $R4
  FindFirst $0 $R0 "$1\*.*"
 
  loop:
  IfErrors upgradedll.nomore
    StrCpy $R7 "$1\$R0"
    StrCpy $R4 "$2\$R0"
    StrCmp $R0 "." +3
    StrCmp $R0 ".." +2
      Call :upgradedll.doit
    FindNext $0 $R0
    Goto loop
 
  upgradedll.nomore:
  FindClose $0
  StrCpy $R7 $1
  StrCpy $R4 $2
  Pop $2
  Pop $1
  Pop $0
  Goto upgradedll.end
 
  ;------------------------
  ;Do we want to register file?
 
  upgradedll.doit:
  IntCmp $R6 0 upgradedll.moveon
 
  ;------------------------
  ;File already installed? No. Just copy
 
  upgradedll.moveon:
  IfFileExists $R4 0 upgradedll.copy
 
  ;------------------------
  ;Check file and version
 
  ClearErrors
    GetDLLVersion $R7 $R0 $R1
    GetDLLVersion $R4 $R2 $R3
  IfErrors upgradedll.upgrade
 
  IntCmpU $R0 $R2 0 upgradedll.dontupgrade upgradedll.upgrade
  IntCmpU $R1 $R3 upgradedll.upgrade upgradedll.dontupgrade upgradedll.upgrade
 
  ;------------------------
  ;Let's upgrade the DLL!
 
  upgradedll.upgrade:
  IntCmp $R6 1 0 +2 +2
    UnRegDLL $R4   ;Unregister the DLL
 
  ;------------------------
  ;Try to copy the DLL directly
 
  upgradedll.copy:
  ClearErrors
    Rename $R7 $R4
  IfErrors 0 upgradedll.noreboot
 
  ;------------------------
  ;DLL is in use. Copy it to a temp file and Rename it on reboot.
 
  Rename /REBOOTOK $R7 $R4
 
  ;------------------------
  ;Register the DLL on reboot
 
  IntCmp $R6 0 upgradedll.done
 
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\RunOnce" \
    "Register $R4" '"$SYSDIR\regsvr32.exe" /s "$R4"'
  Goto upgradedll.done
 
  ;------------------------
  ;Register the DLL
 
  upgradedll.noreboot:
  IntCmp $R6 0 upgradedll.done 
    RegDLL $R4
  Goto upgradedll.done
 
  ;------------------------
  ;No upgrade required - delete file
  
  upgradedll.dontupgrade:
  Delete $R7
  
  ;------------------------
  ;Done
 
  upgradedll.done:
  Return
 
  ;------------------------
  ;End
 
  upgradedll.end:
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
 
  ;------------------------
  ;Restore settings
 
  SetOverwrite lastused
 
FunctionEnd
 
!endif
