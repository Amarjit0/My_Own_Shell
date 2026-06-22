# KaliShell - Security Operating Environment

A workspace-aware shell designed specifically for penetration testers and security researchers. Unlike general-purpose shells (CMD, PowerShell, Bash), KaliShell understands targets, assets, findings, and reports natively.

## Features

### Core Features
- **Workspace Management**: Create and manage project workspaces
- **Target Tracking**: Organize findings by target
- **Asset Graph**: Visualize relationships between assets
- **Tool Orchestration**: Chain security tools together
- **Findings Management**: Track vulnerabilities with CVSS scoring
- **Report Generation**: Create professional reports from findings
- **Plugin System**: Extend functionality with plugins
- **AI Assistant**: Get suggestions from local LLMs (planned)

### Unique Value Proposition
KaliShell is not just another shell - it's a **Security Operating Environment** that combines:
- Shell functionality
- Database storage
- Asset graph visualization
- Reconnaissance engine
- Reporting engine
- Workflow automation
- Plugin system
- Security-focused DSL

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/kalishell.git
cd kalishell

# Install dependencies (Ubuntu/Debian)
sudo apt-get install -y gcc make libsqlite3-dev libncurses-dev

# Build
make

# Install (optional)
sudo make install
```

### Quick Start

```bash
# Build and run
make
./build/kalishell

# Or run directly
./build/kalishell --help
```

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
kali> recon

# View findings
kali> show findings

# Generate report
kali> report generate
```

### Workspace Structure

```
example/
├── recon/           # Reconnaissance data
├── screenshots/     # Screenshots
├── notes/           # Notes
├── findings/        # Vulnerability findings
├── reports/         # Generated reports
└── database.db      # SQLite database
```

### Commands Reference

#### Workspace Commands
- `workspace create <name>` - Create a new workspace
- `workspace open <name>` - Open an existing workspace
- `workspace list` - List all workspaces
- `workspace close` - Close current workspace

#### Target Commands
- `target add <domain>` - Add a target
- `target list` - List all targets
- `target remove <domain>` - Remove a target

#### Discovery Commands
- `recon` - Run reconnaissance pipeline
- `show subdomains` - Show discovered subdomains
- `show endpoints` - Show discovered endpoints
- `show findings` - Show all findings

#### Analysis Commands
- `analyze` - Analyze findings
- `find <query>` - Search across all data
- `graph show` - Show asset graph

#### Report Commands
- `report generate` - Generate report
- `report export <format>` - Export report (pdf, html, md)

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    KaliShell (Windows/Linux)                │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   Parser    │  │  Completer  │  │  Script Eng │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│                         │                                   │
│  ┌──────────────────────┴──────────────────────┐            │
│  │           Command Dispatcher                 │            │
│  └──────────────────────┬──────────────────────┘            │
│         ┌───────────────┼───────────────┐                   │
│         ▼               ▼               ▼                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  Workspace  │  │    Tool     │  │   Plugin    │         │
│  │   Engine    │  │ Orchestrator│  │   System    │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

## Technology Stack

| Component | Technology | Purpose |
|-----------|------------|---------|
| Core | C (C11) | Shell logic, performance |
| Database | SQLite3 | Workspace storage |
| Scripting | Lua 5.4 | DSL, extensibility |
| UI | ncurses | Terminal UI |
| Build | Make | Build system |

## Development

### Project Structure

```
kalishell/
├── src/
│   ├── core/           # Shell core logic
│   ├── parser/         # Command parsing
│   ├── workspace/      # Workspace engine
│   ├── tools/          # Tool orchestration
│   ├── graph/          # Asset graph
│   ├── scripting/      # Lua/DSL engine
│   ├── tui/            # Terminal UI
│   ├── findings/       # Finding management
│   ├── reports/        # Report generation
│   ├── plugins/        # Plugin system
│   ├── ai/             # AI integration
│   └── utils/          # Utility functions
├── include/            # Header files
├── plugins/            # Plugin examples
├── scripts/            # Example scripts
├── tools/              # Tool definitions
├── docs/               # Documentation
├── tests/              # Unit tests
└── config/             # Configuration files
```

### Building

```bash
# Debug build
make debug

# Release build
make release

# Clean
make clean
```

### Testing

```bash
# Run tests
make test
```

## Roadmap

### Phase 1: Core Shell Foundation ✅
- [x] Main REPL loop
- [x] Command tokenizer/parser
- [x] Built-in commands
- [x] Signal handling
- [x] Configuration loading
- [x] Logging system

### Phase 2: Workspace Engine ✅
- [x] SQLite database initialization
- [x] Workspace CRUD
- [x] Target management
- [ ] Asset storage
- [ ] Finding management
- [ ] Notes system

### Phase 3: Tool Orchestration (In Progress)
- [ ] Tool detection and registration
- [ ] Tool manifest parsing
- [ ] Command execution with output capture
- [ ] Pipeline chaining
- [ ] Result parsing and storage

### Phase 4: Asset Graph
- [ ] Relationship modeling
- [ ] Graph data structure
- [ ] Text-based visualization
- [ ] Graph queries

### Phase 5: Security DSL
- [ ] Lua integration
- [ ] Custom syntax parser
- [ ] Domain-specific commands
- [ ] Variable system
- [ ] Control flow

### Phase 6: Visual Terminal
- [ ] ncurses integration
- [ ] Progress bars
- [ ] Dashboard view
- [ ] Color themes

### Phase 7: Findings & Reporting
- [ ] Finding templates
- [ ] CVSS calculator
- [ ] Report generation
- [ ] Screenshot management

### Phase 8: Plugin System
- [ ] Plugin API definition
- [ ] DLL/dlopen loading
- [ ] Plugin manifest parsing
- [ ] Plugin marketplace

### Phase 9: AI Assistant
- [ ] Local LLM interface
- [ ] Response analysis
- [ ] Suggestion engine

### Phase 10: Installer & Polish
- [ ] NSIS installer
- [ ] Portable executable
- [ ] Documentation
- [ ] Example scripts

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Built for penetration testers and security researchers
- Inspired by modern security tools and workflows
- Designed to be the ultimate security operating environment
