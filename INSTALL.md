# KaliShell - Installation Guide

KaliShell is cross-platform and runs on **Linux**, **Windows**, and **macOS**.

---

## Quick Start

### Option 1: Download Pre-built Binary

| Platform | Download |
|----------|----------|
| Linux x64 | [kalishell-linux-x64](https://github.com/Amarjit0/My_Own_Shell/releases/latest) |
| Windows x64 | [kalishell-windows-x64.exe](https://github.com/Amarjit0/My_Own_Shell/releases/latest) |
| macOS ARM | [kalishell-macos-arm64](https://github.com/Amarjit0/My_Own_Shell/releases/latest) |
| Docker | See below |

### Option 2: Build from Source

```bash
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/kalishell
```

---

## Platform-Specific Instructions

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y gcc make libsqlite3-dev

# Clone and build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make

# Run
./build/kalishell

# Install system-wide (optional)
sudo make install
```

### Linux (Fedora/RHEL)

```bash
# Install dependencies
sudo dnf install -y gcc make sqlite-devel

# Clone and build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make

# Run
./build/kalishell
```

### Linux (Arch)

```bash
# Install dependencies
sudo pacman -S gcc make sqlite

# Clone and build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make

# Run
./build/kalishell
```

### Windows (MinGW)

```bash
# Install MSYS2 from https://www.msys2.org/
# Open MSYS2 MinGW64 terminal

# Install dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-sqlite3

# Clone and build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
mingw32-make

# Run
build\kalishell.exe
```

### Windows (Visual Studio)

```bash
# Install Visual Studio with C++ workload
# Install vcpkg: https://vcpkg.io/

# Install SQLite
vcpkg install sqlite3:x64-windows

# Clone
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell

# Build with CMake
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Run
Release\kalishell.exe
```

### Windows (Portable)

1. Download `kalishell.exe` from releases
2. Download `kalishell.bat` from repo
3. Put both in same folder
4. Double-click `kalishell.bat`

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Clone and build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make

# Run
./build/kalishell
```

### Docker

```bash
# Build image
docker build -t kalishell .

# Run interactively
docker run -it -v $(pwd)/workspaces:/workspace kalishell

# Or use docker-compose
docker-compose up
```

### Docker with Kali Tools

```bash
# Build full image with Kali tools
docker build -t kalishell:kali -f Dockerfile.kali .

# Run
docker run -it kalishell:kali
```

---

## Running from Anywhere

### Linux/macOS

```bash
# Add to PATH
echo 'export PATH="$PATH:/path/to/My_Own_Shell/build"' >> ~/.bashrc
source ~/.bashrc

# Now you can run from anywhere
kalishell
```

### Windows

```batch
# Add to PATH (PowerShell as Admin)
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\path\to\My_Own_Shell\build", "Machine")

# Or use the portable launcher
kalishell.bat
```

---

## WSL (Windows Subsystem for Linux)

KaliShell works perfectly in WSL:

```bash
# From Windows, open WSL terminal
wsl

# Follow Linux instructions above
sudo apt-get install -y gcc make libsqlite3-dev
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/kalishell
```

---

## Verification

After installation, verify it works:

```bash
# Check version
kalishell --version

# Run help
kalishell --help

# Test workspace creation
kalishell
kali> workspace create test
kali> workspace list
kali> exit
```

---

## Troubleshooting

### "sqlite3.h not found"

```bash
# Linux
sudo apt-get install libsqlite3-dev

# macOS
brew install sqlite

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-sqlite3
```

### "command not found"

Make sure the binary is in your PATH or run with full path:

```bash
./path/to/kalishell
```

### Permission denied (Linux/macOS)

```bash
chmod +x build/kalishell
```

---

## Support

- **Issues**: https://github.com/Amarjit0/My_Own_Shell/issues
- **Docs**: https://github.com/Amarjit0/My_Own_Shell/blob/main/README.md
