# Gupt

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
./build/gupt
```

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-sqlite3
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
mingw32-make
build\gupt.exe
```

### macOS
```bash
xcode-select --install
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell
make
./build/gupt
```

### Windows (Portable)
Download `gupt.exe` from [Releases](https://github.com/Amarjit0/My_Own_Shell/releases)

## Commands

```
help / ?            Show commands
new <name>          Create workspace
open <name>         Open workspace
spaces              List workspaces
tools               List security tools
run <tool> [args]   Run a tool
scan <target>       Full scan
recon <target>      Reconnaissance
score <vector>      Calculate CVSS
graph               Show asset graph
dash                Show dashboard
bug create <t> <s>  Report a bug
bug list            List bugs
report              Generate report
search <query>      Search everything
```

## License

MIT
