# Gupt Windows Setup Script
# Downloads everything and builds - no tools needed!

Write-Host ""
Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  Gupt - Windows Setup" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Stop"
$SetupDir = "$env:USERPROFILE\.gupt-setup"
$MinGWDir = "$SetupDir\mingw64"
$ProjectDir = Get-Location

# Create setup directory
if (!(Test-Path $SetupDir)) { New-Item -ItemType Directory -Path $SetupDir | Out-Null }

# Step 1: Download MinGW (if not installed)
Write-Host "[1/4] Checking MinGW..." -ForegroundColor Yellow
$GCC = "$MinGWDir\bin\gcc.exe"
if (Test-Path $GCC) {
    Write-Host "  [OK] MinGW found" -ForegroundColor Green
} else {
    Write-Host "  [..] Downloading MinGW (32MB)..." -ForegroundColor Yellow
    $MinGWUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev1.7z"
    $MinGWZip = "$SetupDir\mingw.7z"
    
    try {
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        Invoke-WebRequest -Uri $MinGWUrl -OutFile $MinGWZip -UseBasicParsing
    } catch {
        Write-Host "  [!!] Download failed. Please install 7-Zip and try again." -ForegroundColor Red
        Write-Host "       Or download MinGW from: https://github.com/niXman/mingw-builds-binaries/releases" -ForegroundColor Gray
        exit 1
    }
    
    Write-Host "  [..] Extracting MinGW..." -ForegroundColor Yellow
    $7z = "C:\Program Files\7-Zip\7z.exe"
    if (!(Test-Path $7z)) { $7z = "C:\Program Files (x86)\7-Zip\7z.exe" }
    
    if (Test-Path $7z) {
        & $7z x $MinGWZip -o"$SetupDir" -y | Out-Null
    } else {
        Write-Host "  [!!] 7-Zip not found. Install from: https://www.7-zip.org/" -ForegroundColor Red
        Write-Host "       Then re-run this script." -ForegroundColor Gray
        exit 1
    }
    
    Remove-Item $MinGWZip -Force -ErrorAction SilentlyContinue
    Write-Host "  [OK] MinGW installed" -ForegroundColor Green
}

# Step 2: Download SQLite (if not present)
Write-Host "[2/4] Checking SQLite..." -ForegroundColor Yellow
$SqliteDir = "$ProjectDir\src\utils"
if (Test-Path "$SqliteDir\sqlite3.h") {
    Write-Host "  [OK] SQLite found" -ForegroundColor Green
} else {
    Write-Host "  [..] Downloading SQLite..." -ForegroundColor Yellow
    $SqliteUrl = "https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip"
    $SqliteZip = "$SetupDir\sqlite.zip"
    
    Invoke-WebRequest -Uri $SqliteUrl -OutFile $SqliteZip -UseBasicParsing
    Expand-Archive -Path $SqliteZip -DestinationPath $SetupDir -Force
    Copy-Item "$SetupDir\sqlite-amalgamation-3450000\sqlite3.h" $SqliteDir
    Copy-Item "$SetupDir\sqlite-amalgamation-3450000\sqlite3.c" $SqliteDir
    Remove-Item $SqliteZip -Force
    Write-Host "  [OK] SQLite downloaded" -ForegroundColor Green
}

# Step 3: Set PATH
Write-Host "[3/4] Setting up environment..." -ForegroundColor Yellow
$env:PATH = "$MinGWDir\bin;$env:PATH"

# Step 4: Build
Write-Host "[4/4] Building Gupt..." -ForegroundColor Yellow
Write-Host ""

# Run make
& "$MinGWDir\bin\gcc.exe" --version | Select-Object -First 1
Write-Host ""

$Sources = @(
    "src\utils\sqlite3.c",
    "src\core\main.c", "src\core\shell.c", "src\core\config.c", "src\core\log.c",
    "src\parser\parser.c",
    "src\workspace\database.c", "src\workspace\workspace.c",
    "src\tools\tool.c", "src\tools\pipeline.c", "src\tools\orchestrator.c",
    "src\graph\graph.c", "src\graph\visualization.c",
    "src\scripting\dsl.c", "src\scripting\lua_engine.c",
    "src\tui\tui.c", "src\tui\dashboard.c", "src\tui\progress.c", "src\tui\themes.c",
    "src\findings\finding.c", "src\findings\cvss.c", "src\findings\template.c",
    "src\reports\report.c", "src\reports\markdown.c", "src\reports\export.c",
    "src\plugins\plugin.c", "src\plugins\loader.c", "src\plugins\api.c",
    "src\ai\ai.c", "src\ai\ollama.c", "src\ai\analyzer.c",
    "src\utils\platform_win.c", "src\utils\file_utils.c", "src\utils\string_utils.c"
)

$IncludeDir = "-Iinclude"
$CFlags = "-Wall -O2 -D_DEFAULT_SOURCE -DSQLITE_THREADSAFE=0"
$LFlags = "-lm -lws2_32"

$SrcList = $Sources -join " "
$Output = "build\gupt.exe"

if (!(Test-Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }

Write-Host "Compiling..." -ForegroundColor Cyan
& "$GCC" $IncludeDir.Split(" ") $CFlags.Split(" ") $SrcList.Split(" ") -o $Output $LFlags.Split(" ")

if ($LASTEXITCODE -eq 0) {
    $Size = (Get-Item $Output).Length / 1MB
    Write-Host ""
    Write-Host "====================================" -ForegroundColor Green
    Write-Host "  BUILD SUCCESS!" -ForegroundColor Green
    Write-Host "====================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "  Location: $ProjectDir\$Output" -ForegroundColor White
    Write-Host "  Size: $([math]::Round($Size, 1)) MB" -ForegroundColor White
    Write-Host ""
    Write-Host "  Run with: .\build\gupt.exe" -ForegroundColor Yellow
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}
