@echo off
REM Gupt Portable Launcher
REM This batch file sets up the environment and launches Gupt

setlocal enabledelayedexpansion

REM Get the directory of this batch file
set "GUPT_DIR=%~dp0"
set "GUPT_DIR=%GUPT_DIR:~0,-1%"

REM Set environment variables
set "PATH=%GUPT_DIR%;%PATH%"
set "GUPT_HOME=%GUPT_DIR%"

REM Check if gupt.exe exists
if not exist "%GUPT_DIR%\gupt.exe" (
    echo [ERROR] gupt.exe not found in %GUPT_DIR%
    echo Please run build_windows.bat first.
    pause
    exit /b 1
)

REM Create default directories if they don't exist
if not exist "%GUPT_DIR%\workspaces" mkdir "%GUPT_DIR%\workspaces"
if not exist "%GUPT_DIR%\config" mkdir "%GUPT_DIR%\config"

REM Launch Gupt
echo Starting Gupt...
echo.
"%GUPT_DIR%\gupt.exe" %*

endlocal
