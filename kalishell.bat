@echo off
REM KaliShell Portable Launcher
REM This batch file sets up the environment and launches KaliShell

setlocal enabledelayedexpansion

REM Get the directory of this batch file
set "KALISHELL_DIR=%~dp0"
set "KALISHELL_DIR=%KALISHELL_DIR:~0,-1%"

REM Set environment variables
set "PATH=%KALISHELL_DIR%;%PATH%"
set "KALISHELL_HOME=%KALISHELL_DIR%"

REM Check if kalishell.exe exists
if not exist "%KALISHELL_DIR%\kalishell.exe" (
    echo [ERROR] kalishell.exe not found in %KALISHELL_DIR%
    echo Please run build_windows.bat first.
    pause
    exit /b 1
)

REM Create default directories if they don't exist
if not exist "%KALISHELL_DIR%\workspaces" mkdir "%KALISHELL_DIR%\workspaces"
if not exist "%KALISHELL_DIR%\config" mkdir "%KALISHELL_DIR%\config"

REM Launch KaliShell
echo Starting KaliShell...
echo.
"%KALISHELL_DIR%\kalishell.exe" %*

endlocal
