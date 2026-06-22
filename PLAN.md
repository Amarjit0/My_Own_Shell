# Gupt - Security Operating Environment

## Vision

A workspace-aware shell that understands targets, assets, findings, and reports natively. Unlike general-purpose shells (CMD, PowerShell, Bash), Gupt is designed specifically for penetration testers and security researchers.

## Core Concepts

### Everything Is an Asset
```
gupt> target example.com
gupt> recon
gupt> analyze
gupt> report
```

### Workspace Structure
```
example/
├── recon/
├── screenshots/
├── notes/
├── findings/
├── reports/
└── database.db
```

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Gupt (Windows/Linux)                │
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
| Config | YAML (libyaml) | Configuration |
| Plugins | dlopen/LoadLibrary | Dynamic loading |
| Build | CMake | Cross-platform build |
| Installer | NSIS (Windows) | Installation |

## Implementation Phases

### Phase 1: Core Shell Foundation (Weeks 1-2)
- [ ] Main REPL loop
- [ ] Command tokenizer/parser
- [ ] Built-in commands (help, exit, clear, history)
- [ ] Signal handling (Ctrl+C, Ctrl+D)
- [ ] Configuration loading
- [ ] Logging system

### Phase 2: Workspace Engine (Weeks 3-4)
- [ ] SQLite database initialization
- [ ] Workspace CRUD (create, open, close, delete)
- [ ] Target management (add, remove, list)
- [ ] Asset storage (subdomains, endpoints, technologies)
- [ ] Finding management
- [ ] Notes system

### Phase 3: Tool Orchestration (Weeks 5-6)
- [ ] Tool detection and registration
- [ ] Tool manifest parsing (YAML)
- [ ] Command execution with output capture
- [ ] Pipeline chaining (tool A → tool B)
- [ ] Result parsing and storage
- [ ] Error handling and recovery

### Phase 4: Asset Graph (Weeks 7-8)
- [ ] Relationship modeling (domain → subdomain → endpoint)
- [ ] Graph data structure
- [ ] Text-based visualization
- [ ] Graph queries (find all endpoints for a subdomain)
- [ ] Export to DOT/GraphViz format

### Phase 5: Security DSL (Weeks 9-10)
- [ ] Lua integration
- [ ] Custom syntax parser
- [ ] Domain-specific commands (target, discover, test, find)
- [ ] Variable system
- [ ] Control flow (if, for, while)
- [ ] Function definitions

### Phase 6: Visual Terminal (Weeks 11-12)
- [ ] ncurses integration
- [ ] Progress bars for long operations
- [ ] Dashboard view (targets, findings, stats)
- [ ] Color themes
- [ ] Split views (terminal + output)

### Phase 7: Findings & Reporting (Weeks 13-14)
- [ ] Finding templates (XSS, SQLi, IDOR, etc.)
- [ ] CVSS calculator
- [ ] Report generation (Markdown → HTML → PDF)
- [ ] Screenshot management
- [ ] Evidence attachment

### Phase 8: Plugin System (Weeks 15-16)
- [ ] Plugin API definition
- [ ] DLL/dlopen loading
- [ ] Plugin manifest parsing
- [ ] Plugin marketplace concept
- [ ] Built-in plugins (nuclei, ffuf, katana)

### Phase 9: AI Assistant (Weeks 17-18)
- [ ] Local LLM interface (Ollama)
- [ ] Response analysis
- [ ] Suggestion engine
- [ ] Finding enrichment

### Phase 10: Installer & Polish (Weeks 19-20)
- [ ] NSIS installer (Windows)
- [ ] Portable executable
- [ ] Documentation
- [ ] Example scripts
- [ ] Testing

## Database Schema

```sql
-- Workspaces
CREATE TABLE workspaces (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    path TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Targets
CREATE TABLE targets (
    id INTEGER PRIMARY KEY,
    workspace_id INTEGER REFERENCES workspaces(id),
    name TEXT NOT NULL,
    type TEXT CHECK(type IN ('domain', 'ip', 'url', 'cidr')),
    status TEXT DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Assets
CREATE TABLE assets (
    id INTEGER PRIMARY KEY,
    target_id INTEGER REFERENCES targets(id),
    type TEXT CHECK(type IN ('subdomain', 'endpoint', 'technology', 'screenshot')),
    value TEXT NOT NULL,
    metadata JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Findings
CREATE TABLE findings (
    id INTEGER PRIMARY KEY,
    target_id INTEGER REFERENCES targets(id),
    title TEXT NOT NULL,
    severity TEXT CHECK(severity IN ('critical', 'high', 'medium', 'low', 'info')),
    cvss_score REAL,
    description TEXT,
    recommendation TEXT,
    evidence TEXT,
    status TEXT DEFAULT 'open',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Notes
CREATE TABLE notes (
    id INTEGER PRIMARY KEY,
    target_id INTEGER REFERENCES targets(id),
    title TEXT NOT NULL,
    content TEXT,
    tags JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Tool Results
CREATE TABLE tool_results (
    id INTEGER PRIMARY KEY,
    target_id INTEGER REFERENCES targets(id),
    tool_name TEXT NOT NULL,
    command TEXT,
    output TEXT,
    parsed_data JSON,
    execution_time INTEGER,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Asset Relationships (Graph)
CREATE TABLE asset_relationships (
    id INTEGER PRIMARY KEY,
    source_id INTEGER REFERENCES assets(id),
    target_id INTEGER REFERENCES assets(id),
    relationship TEXT NOT NULL
);
```

## File Structure

```
gupt/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── PLAN.md                     # This file
├── src/
│   ├── core/
│   │   ├── main.c             # Entry point
│   │   ├── shell.c            # Main REPL loop
│   │   ├── shell.h            # Shell interface
│   │   ├── config.c           # Configuration loader
│   │   ├── config.h
│   │   ├── log.c              # Logging system
│   │   └── log.h
│   ├── parser/
│   │   ├── parser.c           # Command parser
│   │   ├── parser.h
│   │   ├── tokenizer.c        # Tokenizer
│   │   └── tokenizer.h
│   ├── completion/
│   │   ├── completion.c       # Tab completion
│   │   └── completion.h
│   ├── workspace/
│   │   ├── workspace.c        # Workspace engine
│   │   ├── workspace.h
│   │   ├── database.c         # SQLite operations
│   │   ├── database.h
│   │   ├── target.c           # Target management
│   │   └── target.h
│   ├── tools/
│   │   ├── tool.c             # Tool detection/registration
│   │   ├── tool.h
│   │   ├── orchestrator.c     # Tool execution
│   │   ├── orchestrator.h
│   │   ├── pipeline.c         # Pipeline management
│   │   └── pipeline.h
│   ├── graph/
│   │   ├── graph.c            # Asset graph
│   │   ├── graph.h
│   │   └── visualization.c    # Text visualization
│   ├── scripting/
│   │   ├── lua_engine.c       # Lua integration
│   │   ├── lua_engine.h
│   │   ├── dsl.c              # Security DSL
│   │   └── dsl.h
│   ├── tui/
│   │   ├── tui.c              # ncurses UI
│   │   ├── tui.h
│   │   ├── dashboard.c        # Dashboard view
│   │   ├── progress.c         # Progress bars
│   │   └── themes.c           # Color themes
│   ├── findings/
│   │   ├── finding.c          # Finding management
│   │   ├── finding.h
│   │   ├── template.c         # Finding templates
│   │   └── cvss.c             # CVSS calculator
│   ├── reports/
│   │   ├── report.c           # Report generation
│   │   ├── report.h
│   │   ├── markdown.c         # Markdown parser
│   │   └── export.c           # PDF/HTML export
│   ├── plugins/
│   │   ├── plugin.c           # Plugin system
│   │   ├── plugin.h
│   │   ├── loader.c           # DLL/dlopen loader
│   │   └── api.c              # Plugin API
│   ├── ai/
│   │   ├── ai.c               # AI integration
│   │   ├── ai.h
│   │   ├── ollama.c           # Ollama client
│   │   └── analyzer.c         # Response analyzer
│   └── utils/
│       ├── string_utils.c     # String utilities
│       ├── string_utils.h
│       ├── file_utils.c       # File utilities
│       ├── file_utils.h
│       └── platform.c         # Platform abstraction
├── include/
│   └── gupt/
│       ├── gupt.h        # Main include
│       ├── types.h            # Common types
│       └── errors.h           # Error codes
├── plugins/
│   ├── nuclei/
│   │   ├── nuclei.c           # Nuclei plugin
│   │   └── manifest.yaml
│   ├── ffuf/
│   │   ├── ffuf.c             # Ffuf plugin
│   │   └── manifest.yaml
│   └── katana/
│       ├── katana.c           # Katana plugin
│       └── manifest.yaml
├── scripts/
│   ├── recon.lua              # Recon workflow
│   ├── api-audit.lua          # API audit workflow
│   └── web-basic.lua          # Basic web test
├── tools/
│   └── definitions/
│       ├── subfinder.yaml     # Tool definition
│       ├── httpx.yaml
│       ├── katana.yaml
│       └── nuclei.yaml
├── installer/
│   ├── gupt.nsi          # NSIS installer script
│   └── portable.bat           # Portable launcher
├── docs/
│   ├── getting-started.md
│   ├── commands.md
│   ├── plugins.md
│   └── scripting.md
├── tests/
│   ├── test_parser.c
│   ├── test_workspace.c
│   └── test_tools.c
├── config/
│   └── gupt.yaml         # Default configuration
└── data/
    └── templates/             # Finding templates
        ├── xss.yaml
        ├── sqli.yaml
        └── idor.yaml
```

## Command Reference

### Workspace Commands
```bash
workspace create <name>      # Create new workspace
workspace open <name>        # Open existing workspace
workspace list               # List all workspaces
workspace close              # Close current workspace
```

### Target Commands
```bash
target add <domain>          # Add target
target list                  # List targets
target remove <domain>       # Remove target
target select <domain>       # Select active target
```

### Discovery Commands
```bash
discover subdomains          # Run subdomain enumeration
discover endpoints           # Crawl for endpoints
discover technologies        # Detect technologies
discover screenshots         # Take screenshots
```

### Analysis Commands
```bash
analyze                      # Analyze all findings
analyze <target>             # Analyze specific target
find <query>                 # Search across all data
show findings                # Show all findings
show assets                  # Show all assets
```

### Graph Commands
```bash
graph show                   # Display asset graph
graph export                 # Export to DOT format
graph stats                  # Show graph statistics
```

### Report Commands
```bash
report generate              # Generate report
report preview               # Preview report
report export <format>       # Export (pdf, html, md)
```

### Pipeline Commands
```bash
pipeline run <name>          # Run pipeline
pipeline list                # List available pipelines
pipeline create <name>       # Create custom pipeline
```

## Example Workflow

```bash
# Start Gupt
$ gupt

# Create workspace
gupt> workspace create example
[+] Workspace 'example' created

# Add target
gupt> target add example.com
[+] Target 'example.com' added

# Run recon pipeline
gupt> recon
[*] Starting reconnaissance...
[*] Running subfinder...
[+] Found 154 subdomains
[*] Running httpx...
[+] 32 live hosts
[*] Running katana...
[+] 2400 endpoints discovered
[+] Reconnaissance complete

# View results
gupt> show subdomains
┌─────────────────────────────┬──────────┬────────────┐
│ Subdomain                   │ Status   │ IP Address │
├─────────────────────────────┼──────────┼────────────┤
│ api.example.com             │ Live     │ 1.2.3.4    │
│ admin.example.com           │ Live     │ 1.2.3.5    │
│ cdn.example.com             │ Live     │ 1.2.3.6    │
└─────────────────────────────┴──────────┴────────────┘

# View asset graph
gupt> graph show
example.com
├── api.example.com
│   ├── /api/v1/users
│   ├── /api/v1/admin
│   └── /graphql
├── admin.example.com
│   ├── /login
│   └── /dashboard
└── cdn.example.com
    └── /static/*

# Analyze
gupt> analyze
[+] Analysis complete
[+] Found 4 potential issues

# Generate report
gupt> report generate
[+] Report generated: reports/example_2026-06-22.pdf
```

## Next Steps

1. Start with Phase 1: Core Shell Foundation
2. Implement basic REPL and command parsing
3. Add SQLite for workspace storage
4. Build tool orchestration system
5. Add TUI for modern interface

This project will demonstrate:
- Systems programming (C)
- Database design (SQLite)
- Plugin architecture
- Security tooling
- Workflow automation
- Modern terminal UI
