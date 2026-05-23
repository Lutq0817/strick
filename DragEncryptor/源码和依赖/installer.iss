[Setup]
AppName=DragEncryptor
AppVersion=1.0
AppPublisher=Lutq0817
DefaultDirName={pf}\DragEncryptor
DefaultGroupName=DragEncryptor
UninstallDisplayIcon={app}\DragEncryptor.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
OutputBaseFilename=DragEncryptor_Setup

[Files]
Source: "DragEncryptor.exe"; DestDir: "{app}"
Source: "libcrypto-4-x64.dll"; DestDir: "{app}"
Source: "libssl-4-x64.dll"; DestDir: "{app}"
Source: "libgcc_s_seh-1.dll"; DestDir: "{app}"
Source: "libstdc++-6.dll"; DestDir: "{app}"
Source: "libwinpthread-1.dll"; DestDir: "{app}"

[Icons]
Name: "{group}\DragEncryptor"; Filename: "{app}\DragEncryptor.exe"
Name: "{commondesktop}\DragEncryptor"; Filename: "{app}\DragEncryptor.exe"

[Run]
Filename: "{app}\DragEncryptor.exe"; Description: "?? DragEncryptor"; Flags: postinstall nowait skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\encryptor.key"
