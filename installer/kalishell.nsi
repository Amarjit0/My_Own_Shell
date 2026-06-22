; KaliShell NSIS Installer Script
; https://nsis.sourceforge.io/

!include "MUI2.nsh"

; General
Name "KaliShell"
OutFile "KaliShell-Installer.exe"
InstallDir "$PROGRAMFILES\KaliShell"
InstallDirRegKey HKLM "Software\KaliShell" "InstallDir"
RequestExecutionLevel admin

; Version info
VIProductVersion "0.1.0"
VIAddVersionKey "ProductName" "KaliShell"
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
Section "KaliShell (Required)"
    SectionIn RO
    
    ; Set output path to installation directory
    SetOutPath $INSTDIR
    
    ; Add files
    File "build\kalishell.exe"
    File "LICENSE"
    File "README.md"
    
    ; Create config directory
    CreateDirectory "$INSTDIR\config"
    SetOutPath "$INSTDIR\config"
    File "config\kalishell.yaml"
    
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
    WriteRegStr HKLM "Software\KaliShell" "InstallDir" "$INSTDIR"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Add to PATH
    Environ::AddPath "$INSTDIR"
    
    ; Create Start Menu shortcuts
    CreateDirectory "$SMPROGRAMS\KaliShell"
    CreateShortcut "$SMPROGRAMS\KaliShell\KaliShell.lnk" "$INSTDIR\kalishell.exe"
    CreateShortcut "$SMPROGRAMS\KaliShell\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Add to Add/Remove Programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "DisplayName" "KaliShell"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "DisplayVersion" "0.1.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "Publisher" "KaliShell"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell" "NoRepair" 1
    
SectionEnd

; Uninstaller section
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\kalishell.exe"
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
    RMDir /r "$SMPROGRAMS\KaliShell"
    
    ; Remove registry keys
    DeleteRegKey HKLM "Software\KaliShell"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KaliShell"
    
SectionEnd
