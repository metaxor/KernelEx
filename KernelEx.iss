#include "common/version.h"
#include ReadReg(HKEY_LOCAL_MACHINE,'Software\Sherlock Software\InnoTools\Downloader','ScriptPath','')

[Setup]
AlwaysRestart=yes
AppName="KernelEx"
AppVerName=KernelEx {#VERSION_STR}
AppVersion={#VERSION_NUMBER}
DefaultDirName={win}\KernelEx
DefaultGroupName="KernelEx"
DirExistsWarning=no
DisableDirPage=yes
LicenseFile=License.txt
;MinVersion=4.10,4.0
;OnlyBelowVersion=0,4.0
OutputBaseFilename="KernelEx"
OutputDir=output-i386\setup

[Files]
Source: "{win}\SYSBCKUP\KERNEL32.DLL"; DestDir: "{sys}"; Flags: external skipifsourcedoesntexist restartreplace allowunsafefiles
Source: "output-i386\Debug\KernelEx.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\kexbasen.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\kexbases.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\kexCOM.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\kexcontrol.exe"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\prep.exe"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\sdbcreate.exe"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\sheet.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Debug\verify.exe"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "apilibs\CORE.INI"; DestDir: "{app}"
Source: "apilibs\settings.reg"; DestDir: "{app}"
Source: "License.txt"; DestDir: "{app}"
Source: "Release Notes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "output-i386\Debug\VKrnlEx.vxd"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\msimg32.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\netapi32.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\pdh.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\psapi.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\userenv.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\uxtheme.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\winsta.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "output-i386\Release\wtsapi32.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace
Source: "util\sdbcreate\sdbdb\KernelEx.sdb"; DestDir: "{win}\AppPatch\Custom"
Source: "util\sdbcreate\sdbdb\kexsdb.i.reg"; DestDir: "{app}"

[Messages]
WelcomeLabel2=This wizard will guide you through the installation of [name].%n%nIt is recommended that yo uclose all other applications before starting Setup. This will make it possible to update relevant system files without having to reboot your computer.

[Icons]
Name: "{group}\Release Notes"; Filename: "{app}\Release Notes.txt"; IconFilename: "Release Notes"
Name: "{group}\Verify Installation"; Filename: "{app}\verify.exe"; IconFilename: "Verify Installation"
Name: "{group}\KernelEx Home Page"; Filename: "http://kernelex.sourceforge.net/"; IconFilename: "KernelEx Home Page"
Name: "{group}\KernelEx Wiki"; Filename: "{app}\http://kernelex.sourceforge.net/wiki/"; IconFilename: "KernelEx Wiki"
Name: "{group}\Uninstall"; Filename: "{app}\Uninstall.exe"; IconFilename: "Uninstall"

[Registry]
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunServicesOnce"; ValueType: string; ValueName: "KexNeedsReboot"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "MSIMG32"; ValueData: "MSIMG32.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "NETAPI32"; ValueData: "NETAPI32.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "PDH"; ValueData: "PDH.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "PSAPI"; ValueData: "PSAPI.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "USERENV"; ValueData: "USERENV.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "UXTHEME"; ValueData: "UXTHEME.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "WINSTA"; ValueData: "WINSTA.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx\KnownDLLs"; ValueType: string; ValueName: "WTSAPI32"; ValueData: "WTSAPI32.DLL"
Root: "HKLM"; Subkey: "Software\KernelEx"; ValueType: string; ValueName: "InstallDir"; ValueData: "{app}"
Root: "HKLM"; Subkey: "System\CurrentControlSet\Services\VxD\VKRNLEX"; ValueType: string; ValueName: "StaticVxD"; ValueData: "{app}\VKrnlEx.vxd"
Root: "HKLM"; Subkey: "System\CurrentControlSet\Control\MPRServices\KernelEx"; ValueType: string; ValueName: "DLLName"; ValueData: "{app}\KernelEx.dll"
Root: "HKLM"; Subkey: "System\CurrentControlSet\Control\MPRServices\KernelEx"; ValueType: string; ValueName: "EntryPoint"; ValueData: "_MprStart@4"
Root: "HKLM"; Subkey: "System\CurrentControlSet\Control\MPRServices\KernelEx"; ValueType: dword; ValueName: "StackSize"; ValueData: "4096"
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "KexVerify"; ValueData: "{app}\verify.exe"
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunOnce"; ValueType: string; ValueName: "Register KernelEx"; ValueData: """{sys}\regsvr32.exe"" /s ""{app}\KernelEx.dll"""
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunOnce"; ValueType: string; ValueName: "Register kexbasen"; ValueData: """{sys}\regsvr32.exe"" /s ""{app}\kexbasen.dll"""
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunOnce"; ValueType: string; ValueName: "Register kexbases"; ValueData: """{sys}\regsvr32.exe"" /s ""{app}\kexbases.dll"""
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunOnce"; ValueType: string; ValueName: "Register kexCom"; ValueData: """{sys}\regsvr32.exe"" /s ""{app}\kexcom.dll"""
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\RunOnce"; ValueType: string; ValueName: "Register Sheet"; ValueData: """{sys}\regsvr32.exe"" /s ""{app}\sheet.dll"""

[Run]
Filename: "{win}\regedit.exe"; Parameters: "/s ""{app}\settings.reg"""; Flags: waituntilterminated
Filename: "{win}\regedit.exe"; Parameters: "/s ""{app}\kexsdb.i.reg"""; Flags: waituntilterminated

[InstallDelete]
Type: files; Name: "{app}\settings.reg"
Type: files; Name: "{app}\kexsdb.i.reg"

[Dirs]
Name: "{win}\AppPatch\Custom"

[Code]
var
  EnableButton: TRadioButton;
  WarningText: TLabel;
  DefPageConfig: TWizardPage;

procedure PageDefConfig(); forward;

function InitializeSetup(): Boolean;
var
  DllKex: LongInt;
  DllKrnl32: LongInt;
  Err: Integer;
  kexVersion: LongInt;
  krnl32Version: LongInt;
  kupVersion: LongInt;
begin
  if RegValueExists(HKEY_LOCAL_MACHINE, 'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce', 'KexNeedsReboot') then
  begin
    MsgBox('Detected unfinished previous installation.'#13'You have to restart the system in order to complete it before you can proceed further.', mbError, MB_OK);
    result := false;
    exit;
  end;

  DllKex := LoadDLL('KernelEx.dll', Err);
  DllKrnl32 := LoadDLL('kernel32.dll', Err);

  if VarIsNull(DllKex) Or VarIsNull(DllKrnl32) then
  begin
    result := true;
    exit;
  end;

  CallDLLProc(DllKex, 'kexGetKEXVersion', 0, 0, kexVersion);

  //if CallDLLProc(DllKrnl32, 'KEXVersion', 0, 0, krnl32Version) Or CallDLLProc(DllKrnl32, 'KUPVersion', 0, 0, kupVersion) then
  //begin
  //  MsgBox(Format('Setup has detected previous version of KernelEx (%d) installed on this computer. Please uninstall it before continuing.', [kexVersion]), mbError, MB_OK);
  //  result := false;
  //  exit;
  //end;

  if kexVersion > {#VERSION_CODE} then
  begin
    MsgBox('Can''t downgrade. Please uninstall currently installed version of KernelEx before continuing.', mbError, MB_OK);
    result := false;
    exit;
  end;

  result := true;
end;

procedure InitializeWizard();
begin
  ITD_Init();
  PageDefConfig();
end;

function InitializeUninstall(): Boolean;
begin
  if RegValueExists(HKEY_LOCAL_MACHINE, 'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce', 'KexNeedsReboot') then
  begin
    MsgBox('Detected unfinished previous installation.'#13'You have to restart the system in order to complete it before you can proceed further.', mbError, MB_OK);
    result := false;
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  msgResult: Integer;
  versionStr: String;
begin
  if CurPageId = wpReady then
  begin

    //if FileExists(ExpandConstant('{sys}\unicows.dll')) then
    //begin
    //  GetVersionNumbersString(ExpandConstant('{sys}\unicows.dll'), versionStr);

    //  if CompareStr(versionStr, '1.1.3790.0') = 0 then
    //  begin
    //    ITD_ClearFiles();
    //    result := true;
    //    exit;
    //  end;
    //end;

    msgResult := MsgBox('Microsoft Layer for Unicode is required.'#13
      'It is strongly advised that you install'#13
      'Microsoft Layer for Unicode before continuing.'#13
      'If you choose to continue, you will need to connect'#13
      'to the internet before proceeding.'#13
      'Would you like to continue with the installation?', mbError, MB_YESNO Or MB_DEFBUTTON2);

    if msgResult = IDNO then
    begin
      ITD_ClearFiles();
      result := false;
      Abort;
    end
    else
    begin;
      ITD_AddFile('http://download.microsoft.com/download/b/7/5/b75eace3-00e2-4aa0-9a6f-0b6882c71642/unicows.exe', ExpandConstant('{tmp}\unicows.exe'));
      ITD_DownloadAfter(wpReady);
    end;
  end;

  result := true;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ErrorCode: Integer;
label
  _Error,
  _Skip;

begin
  if CurStep = ssInstall then
  begin
    CreateDir(ExpandConstant('{app}\MSLU'));
    if Not ShellExec('open', ExpandConstant('"{tmp}\unicows.exe"'),
          ExpandConstant('/t:{app}\MSLU'), '', SW_SHOW, ewWaitUntilTerminated, ErrorCode) then
    begin
      MsgBox('A fatal error occurred during the installation'#13'of the Microsoft Layer for Unicode.', mbError, MB_OK);
      goto _Error;
    end;

    if Not ErrorCode = 0 then goto _Error;

    DeleteFile(ExpandConstant('{tmp}\unicows.exe'));

    FileCopy(ExpandConstant('{app}\MSLU\unicows.dll'), ExpandConstant('{app}\mslu.tmp'), false);
    RestartReplace(ExpandConstant('{app}\mslu.tmp'), ExpandConstant('{sys}\unicows.dll'));

    goto _Skip;

_Error:
    RemoveDir(ExpandConstant('{app}'));
    Abort;
_Skip:
  
  end;
end;

procedure ToggleWarning(Sender: TObject);
begin
  if Sender = EnableButton then
  begin
    WarningText.Visible := false;
  end
  else
  begin
    WarningText.Visible := true;
  end;
end;

function PageLeaveDefConfig(Sender: TWizardPage): Boolean;
begin
  if EnableButton.Checked = true then
  begin
    RegWriteDWordValue(HKEY_LOCAL_MACHINE, 'Software\KernelEx', 'DisableExtensions', 0);
  end
  else
  begin
    RegWriteDWordValue(HKEY_LOCAL_MACHINE, 'Software\KernelEx', 'DisableExtensions', 1);
  end;

  result := true;
end;

procedure PageDefConfig();
var
  RadioButton: TRadioButton;
  StaticText: TLabel;
begin
  DefPageConfig := CreateCustomPage(wpInstalling, 'Default configuration', 'Choose default configuration of KernelEx.');
  DefPageConfig.OnNextButtonClick := @PageLeaveDefConfig;
  DefPageConfig.OnShouldSkipPage := @PageLeaveDefConfig;

  StaticText := TLabel.Create(DefPageConfig);
  StaticText.Parent := DefPageConfig.Surface;
  StaticText.WordWrap := true;
  StaticText.Width := 450;
  StaticText.Height := 40;
  StaticText.Caption := 'You can choose default KernelEx configuration here. This configuration will be used for all applications for which no other configuration is specified.';

  EnableButton := TRadioButton.Create(DefPageConfig);
  EnableButton.Parent := DefPageConfig.Surface;
  EnableButton.Top := 75;
  EnableButton.Width := 400;
  EnableButton.Caption := 'Enable KernelEx extensions for all applications (recommended)';
  EnableButton.Checked := true;
  EnableButton.OnClick := @ToggleWarning;

  RadioButton := TRadioButton.Create(DefPageConfig);
  RadioButton.Parent := DefPageConfig.Surface;
  RadioButton.Top := 105;
  RadioButton.Width := 250;
  RadioButton.Caption := 'Disable KernelEx extensions';
  RadioButton.Checked := false;
  RadioButton.OnClick := @ToggleWarning;

  WarningText := TLabel.Create(DefPageConfig);
  WarningText.Parent := DefPageConfig.Surface;
  WarningText.WordWrap := true;
  WarningText.Top := 175;
  WarningText.Width := 400;
  WarningText.Height := 40;
  WarningText.Caption := 'If you choose to disable KernelEx extensions, programs you run won''t get extended API functions by default. You will have to enable KernelEx extensions individually for applications which require newer Windows versions. This choice is better if you''re paranoid.';
  WarningText.Visible := false;
end;

