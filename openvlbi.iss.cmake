#define MyAppName "OpenVLBI"
#define TargetName "vlbi_server_json"

[Setup]
AppId={{38D7ECEF-D2FE-464C-A8BB-2271CCC6EE3E}
AppName={#MyAppName}
AppVersion=@VLBI_VERSION_STRING@
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName=Ilia Platone
UninstallDisplayIcon={app}\{#TargetName}.exe
WizardStyle=modern
Compression=lzma2
SolidCompression=yes
OutputDir="./"
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename={#TargetName}_setup
SetupIconFile=icon.ico

[Files]
Source: "../bin/{#TargetName}64/*"; DestDir: "{app}"; Check: Is64BitInstallMode ; Flags: solidbreak recursesubdirs
Source: "../bin/{#TargetName}32/*"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: solidbreak recursesubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#TargetName}.exe"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#TargetName}.exe"

[Code]
function VersionInstalled(const ProductID: string): String;
var
  UninstallExe: String;
  UninstallRegistry: String;
begin
    // Create the correct registry location name, which is based on the AppId
    UninstallRegistry := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\' + ProductID + '_is1');
    // Check whether an extry exists
    RegQueryStringValue(HKLM, UninstallRegistry, 'UninstallString', UninstallExe);
    Result := UninstallExe;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  UninstallExe: String;
begin
  if (CurStep = ssInstall) then // Install step has started
        begin
        UninstallExe := VersionInstalled('{#SetupSetting("AppId")}');
        if (not (UninstallExe = '')) then
        begin
          MsgBox('Setup will now remove the previous version.', mbInformation, MB_OK);
          Exec(RemoveQuotes(UninstallExe), ' /SILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
          sleep(1000);    //Give enough time for the install screen to be repainted before continuing
        end;
  end;
end;
