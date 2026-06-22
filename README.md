# KaliShell

A cross-platform Security Operating Environment built in C.

## Features

- Workspace management with SQLite
- 14 security tools registered
- Pipelines (recon, web-basic, api-audit)
- CVSS v3.1 calculator
- Asset graph visualization
- Report generation
- Plugin system

## Install

### Linux
```bash
sudo apt install gcc make libsqlite3-dev
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/kalishell
```

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-sqlite3
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
mingw32-make
build\kalishell.exe
```

### macOS
```bash
xcode-select --install
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/kalishell
```

### Windows (Portable)
Download `kalishell.exe` from [Releases](https://github.com/Amarjit0/My_Own_Shell/releases)

## Commands

```
help                    Show commands
workspace create <name> Create workspace
tool list               List tools
pipeline run recon <t>  Run recon
cvss <vector>           Calculate CVSS
report generate         Generate report
```

## License

MIT
