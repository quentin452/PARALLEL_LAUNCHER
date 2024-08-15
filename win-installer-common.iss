#define MyAppName "Parallel Launcher"
#define MyAppVersion "7.7.0"
#define MyAppPublisher "Matt Pharoah"
#define MyAppURL "https://parallel-launcher.ca"
#define MyAppExeName "parallel-launcher.exe"

#define RegistryId "parallel_launcher"

[InstallDelete]
Type: files; Name: "{app}\styles\qwindowsvistastyle.dll"
Type: files; Name: "{app}\qrc_resources.cpp"
Type: files; Name: "{app}\parallel-launcher-sdl-proxy.exe"
Type: files; Name: "{app}\mount-sd-card.exe"
Type: files; Name: "{app}\virtdisk.dll"
Type: files; Name: "{app}\translations\parallel-launcher.ar.qm"
Type: files; Name: "{app}\translations\parallel-launcher.de.qm"
Type: files; Name: "{app}\translations\parallel-launcher.en_gb.qm"
Type: files; Name: "{app}\translations\parallel-launcher.en_us.qm"
Type: files; Name: "{app}\translations\parallel-launcher.es.qm"
Type: files; Name: "{app}\translations\parallel-launcher.fr_ca.qm"
Type: files; Name: "{app}\translations\parallel-launcher.fr.qm"
Type: files; Name: "{app}\translations\parallel-launcher.it.qm"
Type: files; Name: "{app}\translations\parallel-launcher.pt.qm"

[Setup]
AppId=2C94867F-0911-4237-B447-CE6E30AD7F55
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\parallel-launcher
DisableProgramGroupPage=yes
LicenseFile={#SourcePath}\LICENSE
PrivilegesRequired=admin
OutputDir={#SourcePath}\{#TargetArch}
OutputBaseFilename=parallel-launcher_setup_{#TargetArch}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ChangesAssociations=yes

[Registry]
Root: HKCR; Subkey: ".z64"; ValueName: ""; ValueType: string; ValueData: "{#RegistryId}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".n64"; ValueName: ""; ValueType: string; ValueData: "{#RegistryId}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".v64"; ValueName: ""; ValueType: string; ValueData: "{#RegistryId}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".bps"; ValueName: ""; ValueType: string; ValueData: "{#RegistryId}"; Flags: uninsdeletevalue

Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".z64"; ValueData: ""
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".n64"; ValueData: ""
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".v64"; ValueData: ""
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".bps"; ValueData: ""

Root: HKCR; Subkey: "rhdc"; ValueName: ""; ValueType: string; ValueData: "URL:rhdc Protocol"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "rhdc"; ValueName: "URL Protocol"; ValueType: string; ValueData: ""; Flags: uninsdeletevalue
Root: HKCR; Subkey: "rhdc\shell\open\command"; ValueName: ""; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekey

Root: HKCR; Subkey: "{#RegistryId}"; ValueName: ""; ValueType: string; ValueData: "N64 ROM"; Flags: uninsdeletekey
Root: HKCR; Subkey: "{#RegistryId}\shell\open\command"; ValueName: ""; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekey

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkedonce

[Files]
Source: "{#SourcePath}\release\*"; DestDir: "{app}"; Excludes: "qrc_resources.cpp,qwindowsvistastyle.dll,virtdisk.dll,mount-sd-card.exe"; Flags: ignoreversion recursesubdirs
Source: "{#SourcePath}\winfonts\NotoSans-Regular.ttf"; DestDir: "{fonts}"; FontInstall: "Noto Sans"; Flags: uninsneveruninstall onlyifdoesntexist
Source: "{#SourcePath}\winfonts\NotoSans-Bold.ttf"; DestDir: "{fonts}"; FontInstall: "Noto Sans Bold"; Flags: uninsneveruninstall onlyifdoesntexist
Source: "{#SourcePath}\winfonts\NotoSans-Italic.ttf"; DestDir: "{fonts}"; FontInstall: "Noto Sans Italic"; Flags: uninsneveruninstall onlyifdoesntexist
Source: "{#SourcePath}\winfonts\NotoSans-BoldItalic.ttf"; DestDir: "{fonts}"; FontInstall: "Noto Sans Bold Italic"; Flags: uninsneveruninstall onlyifdoesntexist
Source: "{#SourcePath}\winfonts\NotoMono-Regular.ttf"; DestDir: "{fonts}"; FontInstall: "Noto Mono"; Flags: uninsneveruninstall onlyifdoesntexist

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall
