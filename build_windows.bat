@echo off
REM Gupt Windows Build Script (MinGW)

echo ========================================
echo   Gupt Windows Build Script
echo ========================================
echo.

REM Check for MinGW
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] MinGW/gcc not found!
    echo.
    echo Install MinGW from: https://www.mingw-w64.org/
    echo Or use: scoop install mingw
    echo.
    pause
    exit /b 1
)

REM Check for SQLite
where sqlite3 >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] SQLite3 not found, downloading...
    REM You would download sqlite here
)

echo [1/4] Cleaning build directory...
if exist build rmdir /s /q build
mkdir build

echo [2/4] Compiling source files...

REM Compile all C files
set CFLAGS=-Wall -Wextra -g -Iinclude -D_DEFAULT_SOURCE
set LIBS=-lsqlite3 -lws2_32

set SOURCES= ^
    src\core\main.c ^
    src\core\shell.c ^
    src\core\config.c ^
    src\core\log.c ^
    src\parser\parser.c ^
    src\tools\tool.c ^
    src\tools\pipeline.c ^
    src\tools\orchestrator.c ^
    src\workspace\workspace.c ^
    src\workspace\database.c ^
    src\graph\graph.c ^
    src\graph\visualization.c ^
    src\findings\finding.c ^
    src\findings\cvss.c ^
    src\findings\template.c ^
    src\reports\report.c ^
    src\reports\markdown.c ^
    src\reports\export.c ^
    src\plugins\plugin.c ^
    src\plugins\loader.c ^
    src\plugins\api.c ^
    src\scripting\dsl.c ^
    src\scripting\lua_engine.c ^
    src\tui\tui.c ^
    src\tui\dashboard.c ^
    src\tui\progress.c ^
    src\tui\themes.c ^
    src\ai\ai.c ^
    src\ai\ollama.c ^
    src\ai\analyzer.c ^
    src\utils\platform_win.c ^
    src\utils\string_utils.c ^
    src\utils\file_utils.c

for %%f in (%SOURCES%) do (
    echo   Compiling %%f
    gcc %CFLAGS% -c %%f -o build\%%~nxf.o
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to compile %%f
        pause
        exit /b 1
    )
)

echo [3/4] Linking...
gcc build\*.o -o build\gupt.exe %LIBS%

if %errorlevel% neq 0 (
    echo [ERROR] Linking failed!
    pause
    exit /b 1
)

echo [4/4] Build complete!
echo.
echo ========================================
echo   Build successful!
echo   Output: build\gupt.exe
echo ========================================
echo.
echo Run with: build\gupt.exe
echo.
pause
