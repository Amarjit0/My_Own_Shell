@echo off
REM Build Gupt with Visual Studio (MSVC)

REM Try to find Visual Studio
set "VS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

if "%VS_PATH%"=="" (
    echo [ERROR] Visual Studio not found!
    echo.
    echo Install one of these:
    echo   1. Visual Studio Community (free): https://visualstudio.microsoft.com/downloads/
    echo   2. Build Tools: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
    echo.
    echo After installing, run this script from "Developer Command Prompt" or "x64 Native Tools Command Prompt"
    exit /b 1
)

echo Setting up Visual Studio environment...
call "%VS_PATH%" x64

echo.
echo Building Gupt...
echo.

REM Create build directory
if not exist build mkdir build
if not exist build\core mkdir build\core
if not exist build\parser mkdir build\parser
if not exist build\workspace mkdir build\workspace
if not exist build\tools mkdir build\tools
if not exist build\graph mkdir build\graph
if not exist build\scripting mkdir build\scripting
if not exist build\tui mkdir build\tui
if not exist build\findings mkdir build\findings
if not exist build\reports mkdir build\reports
if not exist build\plugins mkdir build\plugins
if not exist build\ai mkdir build\ai
if not exist build\utils mkdir build\utils

REM Compile
cl /nologo /W4 /I include /D _CRT_SECURE_NO_WARNINGS /D _DEFAULT_SOURCE /D GUPT_PLATFORM_WINDOWS ^
    src\core\main.c src\core\shell.c src\core\config.c src\core\log.c ^
    src\parser\parser.c ^
    src\workspace\database.c src\workspace\workspace.c ^
    src\tools\tool.c src\tools\pipeline.c src\tools\orchestrator.c ^
    src\graph\graph.c src\graph\visualization.c ^
    src\scripting\dsl.c src\scripting\lua_engine.c ^
    src\tui\tui.c src\tui\dashboard.c src\tui\progress.c src\tui\themes.c ^
    src\findings\finding.c src\findings\cvss.c src\findings\template.c ^
    src\reports\report.c src\reports\markdown.c src\reports\export.c ^
    src\plugins\plugin.c src\plugins\loader.c src\plugins\api.c ^
    src\ai\ai.c src\ai\ollama.c src\ai\analyzer.c ^
    src\utils\platform_win.c src\utils\file_utils.c src\utils\string_utils.c ^
    /Febuild\gupt.exe /link sqlite3.lib Advapi32.lib User32.lib

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful: build\gupt.exe
    echo.
    echo Run with: build\gupt.exe
) else (
    echo.
    echo Build failed!
)
