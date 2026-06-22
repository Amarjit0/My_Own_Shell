# KaliShell - Security Operating Environment

A workspace-aware shell designed specifically for penetration testers and security researchers. Unlike general-purpose shells (CMD, PowerShell, Bash), KaliShell understands targets, assets, findings, and reports natively.

## Features

- **Workspace Management**: Create and manage project workspaces
- **Target Tracking**: Organize findings by target
- **Asset Graph**: Visualize relationships between assets
- **Tool Orchestration**: Chain security tools together
- **Findings Management**: Track vulnerabilities with CVSS scoring
- **Report Generation**: Create professional reports
- **Plugin System**: Extend functionality with plugins
- **AI Assistant**: Get suggestions from local LLMs

## Installation

### Linux

```bash
# Clone the repository
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell

# Install dependencies (Ubuntu/Debian)
sudo apt-get install -y gcc make libsqlite3-dev

# Build
make

# Run
./build/kalishell
```

### Windows (MinGW)

```bash
# Clone the repository
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell

# Run the Windows build script
build_windows.bat

# Run
build\kalishell.exe
```

### Windows (CMake + Visual Studio)

```bash
# Clone the repository
git clone https://github.com/Amarjit0/My_Own_Shell.git
cd My_Own_Shell

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# Run
Release\kalishell.exe
```

### Windows Installer (NSIS)

1. Download and install [NSIS](https://nsis.sourceforge.io/)
2. Right-click on `installer\kalishell.nsi`
3. Click "Compile NSIS Script"
4. Run the generated installer

### Portable Windows

1. Download `kalishell.exe` from releases
2. Place in any folder
3. Double-click `kalishell.bat` to run

## Usage

### Basic Commands

```bash
# Show help
kali> help

# Create a workspace
kali> workspace create example

# Add a target
kali> target add example.com

# Run reconnaissance
kali> recon example.com

# View findings
kali> show findings

# Generate report
kali> report generate
```

### Tool Management

```bash
# List available tools
kali> tool list

# Run a tool
kali> tool run nmap -sV example.com

# Run a pipeline
kali> pipeline run recon example.com
```

### CVSS Calculator

```bash
kali> cvss CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H
CVSS Score: 10.0 (critical)
```

## Project Structure

```
kalishell/
├── src/                     # Source files
│   ├── core/               # Shell core
│   ├── parser/             # Command parser
│   ├── workspace/          # Workspace engine
│   ├── tools/              # Tool orchestration
│   ├── graph/              # Asset graph
│   ├── scripting/          # DSL/Lua
│   ├── tui/                # Terminal UI
│   ├── findings/           # Finding management
│   ├── reports/            # Report generation
│   ├── plugins/            # Plugin system
│   ├── ai/                 # AI integration
│   └── utils/              # Utilities
├── include/                # Headers
├── tools/                  # Tool definitions
├── scripts/                # Example scripts
├── installer/              # NSIS installer
└── config/                 # Configuration
```

## Dependencies

### Linux
- GCC
- Make
- SQLite3 dev libraries

### Windows
- MinGW or Visual Studio
- SQLite3

### Optional
- ncurses (for TUI)
- Lua 5.4 (for scripting)

## License

MIT License - see [LICENSE](LICENSE) for details

## Contributing

Contributions welcome! Please open an issue or pull request.
