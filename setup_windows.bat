@echo off
REM Setup Windows build environment

echo ====================================
echo  Gupt Windows Setup
echo ====================================
echo.

REM Check for Visual Studio
where cl >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [OK] Visual Studio compiler found
) else (
    echo [!!] Visual Studio compiler not found
    echo.
    echo Install Build Tools:
    echo   https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
    echo.
    echo Select "Desktop development with C++"
)

REM Check for SQLite
if exist "sqlite3.h" (
    echo [OK] SQLite header found
) else (
    echo [..] Downloading SQLite...
    curl -L -o sqlite3.h https://raw.githubusercontent.com/nicedoc/sqlite-amalgamation/master/sqlite3.h
    curl -L -o sqlite3.c https://raw.githubusercontent.com/nicedoc/sqlite-amalgamation/master/sqlite3.c
    echo [OK] SQLite downloaded
)

REM Check for git
where git >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [OK] Git found
) else (
    echo [!!] Git not found - install from https://git-scm.com/download/win
)

echo.
echo ====================================
echo  Ready to build!
echo ====================================
echo.
echo Run: build_msvc.bat
echo.
