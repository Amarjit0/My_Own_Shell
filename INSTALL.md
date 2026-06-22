# Installation Guide

Gupt runs on **Linux**, **Windows**, and **macOS**.

## Linux

```bash
# Ubuntu/Debian
sudo apt install gcc make libsqlite3-dev

# Fedora
sudo dnf install gcc make sqlite-devel

# Arch
sudo pacman -S gcc make sqlite

# Build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/gupt
```

## Windows

### MSYS2 (Recommended)
```bash
# Download MSYS2: https://www.msys2.org/
# Open MSYS2 MinGW64 terminal

pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-sqlite3

git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
mingw32-make
build\gupt.exe
```

### Visual Studio
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg && bootstrap-vcpkg.bat

# Install SQLite
vcpkg install sqlite3:x64-windows

# Build
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Portable
Download `gupt.exe` from [Releases](https://github.com/Amarjit0/My_Own_Shell/releases)

## macOS

```bash
xcode-select --install
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/gupt
```

## WSL

```bash
wsl
sudo apt install gcc make libsqlite3-dev
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/gupt
```

## Add to PATH

### Linux/macOS
```bash
echo 'export PATH="$PATH:/path/to/My_Own_Shell/build"' >> ~/.bashrc
source ~/.bashrc
```

### Windows
```powershell
# PowerShell (Admin)
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\My_Own_Shell\build", "Machine")
```

## Troubleshooting

**sqlite3.h not found:**
```bash
# Linux
sudo apt install libsqlite3-dev

# macOS
brew install sqlite

# Windows
pacman -S mingw-w64-x86_64-sqlite3
```

**Permission denied:**
```bash
chmod +x build/gupt
```
