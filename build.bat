@echo off
echo.
echo ====================================
echo   Gupt - One-Click Build
echo ====================================
echo.
echo This will download MinGW compiler and build Gupt.
echo It takes about 2-3 minutes.
echo.

powershell -ExecutionPolicy Bypass -File "%~dp0setup.ps1"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo To run Gupt, double-click: build\gupt.exe
    echo.
    pause
) else (
    echo.
    echo Build failed. Try running as Administrator.
    pause
)
