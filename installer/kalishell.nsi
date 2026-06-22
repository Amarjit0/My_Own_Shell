; Gupt NSIS Installer Script
; https://nsis.sourceforge.io/

!include "MUI2.nsh"

; General
Name "Gupt"
OutFile "Gupt-Installer.exe"
InstallDir "$PROGRAMFILES\Gupt"
InstallDirRegKey HKLM "Software\Gupt" "InstallDir"
RequestExecutionLevel admin

; Version info
VIProductVersion "0.1.0"
VIAddVersionKey "ProductName" "Gupt"
VIAddVersionKey "FileDescription" "Security Operating Environment"
VIAddVersionKey "LegalCopyright" "MIT License"
VIAddVersionKey "FileVersion" "0.1.0"

; Interface settings
!define MUI_ABORTWARNING

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Languages
!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "Gupt (Required)"
    SectionIn RO
    
    ; Set output path to installation directory
    SetOutPath $INSTDIR
    
    ; Add files
    File "build\gupt.exe"
    File "LICENSE"
    File "README.md"
    
    ; Create config directory
    CreateDirectory "$INSTDIR\config"
    SetOutPath "$INSTDIR\config"
    File "config\gupt.yaml"
    
    ; Create tools directory
    CreateDirectory "$INSTDIR\tools"
    CreateDirectory "$INSTDIR\tools\definitions"
    SetOutPath "$INSTDIR\tools\definitions"
    File "tools\definitions\*.yaml"
    
    ; Create scripts directory
    CreateDirectory "$INSTDIR\scripts"
    SetOutPath "$INSTDIR\scripts"
    File "scripts\*.lua"
    
    ; Create workspace directory
    CreateDirectory "$INSTDIR\workspaces"
    
    ; Store installation path
    WriteRegStr HKLM "Software\Gupt" "InstallDir" "$INSTDIR"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Add to PATH
    Environ::AddPath "$INSTDIR"
    
    ; Create Start Menu shortcuts
    CreateDirectory "$SMPROGRAMS\Gupt"
    CreateShortcut "$SMPROGRAMS\Gupt\Gupt.lnk" "$INSTDIR\gupt.exe"
    CreateShortcut "$SMPROGRAMS\Gupt\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Add to Add/Remove Programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "DisplayName" "Gupt"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "DisplayVersion" "0.1.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "Publisher" "Gupt"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt" "NoRepair" 1
    
SectionEnd

; Uninstaller section
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\gupt.exe"
    Delete "$INSTDIR\LICENSE"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\Uninstall.exe"
    
    ; Remove directories
    RMDir /r "$INSTDIR\config"
    RMDir /r "$INSTDIR\tools"
    RMDir /r "$INSTDIR\scripts"
    RMDir /r "$INSTDIR\workspaces"
    RMDir "$INSTDIR"
    
    ; Remove from PATH
    Environ::RemovePath "$INSTDIR"
    
    ; Remove Start Menu shortcuts
    RMDir /r "$SMPROGRAMS\Gupt"
    
    ; Remove registry keys
    DeleteRegKey HKLM "Software\Gupt"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gupt"
    
SectionEnd
