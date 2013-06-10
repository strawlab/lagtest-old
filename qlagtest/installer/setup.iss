; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=lagtest
AppVersion=1.0
DefaultDirName={pf}\lagtest
DefaultGroupName=lagtest
UninstallDisplayIcon={app}\lagtest.exe
OutputDir=..\installer
SourceDir=C:\Users\pasieka\Projects\qlagtest\bin

[Files]
Source: "lagtest.exe"; DestDir: "{app}"
Source: "*.dll"; DestDir: "{app}"
Source: "flash.exe"; DestDir: "{app}"
Source: "firmware.hex"; DestDir: "{app}"
Source: "tools\*"; DestDir: "{app}\tools"
;Source: "readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\lagtest"; Filename: "{app}\lagtest.exe"

[Run]
Filename: "{app}\flash.exe"; Parameters: "{app}\tools\avrdude.exe {app}\firmware.hex"; Description: "Flash Arduino with newest lagtest firmware"; Flags: postinstall
Filename: "{app}\lagtest.exe"; Description: "Start Lagtest"; Flags: postinstall

