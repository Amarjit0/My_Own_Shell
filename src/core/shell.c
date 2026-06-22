#include "kalishell/kalishell.h"

// Command history
static char *history[MAX_HISTORY_SIZE];
static int history_count = 0;

// Built-in commands
static ks_command_t builtins[] = {
    {"help", "Show available commands", ks_cmd_help},
    {"exit", "Exit the shell", ks_cmd_exit},
    {"quit", "Exit the shell", ks_cmd_exit},
    {"clear", "Clear the screen", ks_cmd_clear},
    {"history", "Show command history", ks_cmd_history},
    {"cd", "Change directory", ks_cmd_cd},
    {"pwd", "Print working directory", ks_cmd_pwd},
    {"workspace", "Manage workspaces", NULL},
    {"target", "Manage targets", NULL},
    {"show", "Show assets and findings", NULL},
    {"find", "Search across all data", NULL},
    {"recon", "Run reconnaissance", NULL},
    {"analyze", "Analyze findings", NULL},
    {"report", "Generate reports", ks_cmd_report},
    {"graph", "View asset graph", ks_cmd_graph},
    {"pipeline", "Run pipelines", ks_cmd_pipeline},
    {"plugin", "Manage plugins", ks_cmd_plugin},
    {"tool", "Manage tools", NULL},
    {"finding", "Manage findings", ks_cmd_finding},
    {"cvss", "Calculate CVSS score", ks_cmd_cvss},
    {"ai", "AI assistant", ks_cmd_ai},
    {"dsl", "Execute DSL scripts", ks_cmd_dsl},
    {"dashboard", "Show dashboard", NULL},
    {NULL, NULL, NULL}
};

void ks_shell_signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n");
        // Don't exit, just show new prompt
    } else if (sig == SIGTERM) {
        shell.running = false;
    }
}

static void add_to_history(const char *command) {
    if (history_count >= MAX_HISTORY_SIZE) {
        // Shift history
        free(history[0]);
        for (int i = 0; i < history_count - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count--;
    }
    history[history_count++] = strdup(command);
}

static char *get_prompt(void) {
    static char prompt[MAX_PATH_SIZE];
    char *user = shell.username ? shell.username : "kali";
    char *host = shell.hostname ? shell.hostname : "shell";
    char *dir = shell.cwd ? shell.cwd : "~";
    
    // Get relative path from home
    if (shell.home_dir && strncmp(dir, shell.home_dir, strlen(shell.home_dir)) == 0) {
        dir += strlen(shell.home_dir);
        if (*dir == '/') dir++;
    }
    
    snprintf(prompt, sizeof(prompt), 
        COLOR_CYAN "%s" COLOR_RESET "@" COLOR_GREEN "%s" COLOR_RESET ":" COLOR_BLUE "~/%s" COLOR_RESET "$ ",
        user, host, dir);
    
    return prompt;
}

static int execute_command(char *input) {
    char *argv[MAX_ARGS];
    int argc = 0;
    
    // Skip whitespace
    while (*input && isspace(*input)) input++;
    if (!*input) return 0;
    
    // Add to history
    add_to_history(input);
    
    // Parse command
    if (ks_parse_command(input, argv, &argc) != KS_OK) {
        fprintf(stderr, "Error parsing command\n");
        return 1;
    }
    
    if (argc == 0) return 0;
    
    // Check for builtins
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(argv[0], builtins[i].name) == 0) {
            if (builtins[i].func) {
                return builtins[i].func(argc, argv);
            }
            break;
        }
    }
    
    // Check for workspace commands
    if (strcmp(argv[0], "workspace") == 0) {
        if (argc < 2) {
            printf("Usage: workspace <create|open|list|close> [name]\n");
            return 1;
        }
        if (strcmp(argv[1], "create") == 0) {
            return ks_workspace_create(argc > 2 ? argv[2] : NULL);
        } else if (strcmp(argv[1], "open") == 0) {
            return ks_workspace_open(argc > 2 ? argv[2] : NULL);
        } else if (strcmp(argv[1], "list") == 0) {
            return ks_workspace_list();
        } else if (strcmp(argv[1], "close") == 0) {
            return ks_workspace_close();
        } else {
            fprintf(stderr, "Unknown workspace command: %s\n", argv[1]);
            return 1;
        }
    }
    
    // Handle recon command
    if (strcmp(argv[0], "recon") == 0) {
        if (argc < 2) {
            fprintf(stderr, "Usage: recon <target>\n");
            return 1;
        }
        return ks_orchestrator_run_pipeline("recon", argv[1]);
    }
    
    // Handle tool command
    if (strcmp(argv[0], "tool") == 0) {
        if (argc < 2) {
            printf("Usage: tool <list|run> [args]\n");
            return 1;
        }
        if (strcmp(argv[1], "list") == 0) {
            return ks_tool_list();
        }
        if (strcmp(argv[1], "run") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: tool run <name> [args]\n");
                return 1;
            }
            return ks_orchestrator_run_tool(argv[2], argc > 3 ? argv[3] : NULL);
        }
    }
    
    // Handle show command
    if (strcmp(argv[0], "show") == 0) {
        if (argc < 2) {
            printf("Usage: show <subdomains|endpoints|findings|assets>\n");
            return 1;
        }
        // TODO: Implement show commands
        printf("[*] Show %s not yet implemented\n", argv[1]);
        return 0;
    }
    
    // Handle dashboard command
    if (strcmp(argv[0], "dashboard") == 0) {
        return ks_tui_draw_dashboard();
    }
    
    // TODO: Implement other commands
    fprintf(stderr, "Command not implemented: %s\n", argv[0]);
    return 1;
}

int ks_shell_init(ks_shell_t *shell) {
    // Initialize shell state
    memset(shell, 0, sizeof(ks_shell_t));
    
    // Get username
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        shell->username = strdup(pw->pw_name);
    } else {
        shell->username = strdup("kali");
    }
    
    // Get hostname
    char hostname[MAX_PATH_SIZE];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        shell->hostname = strdup(hostname);
    } else {
        shell->hostname = strdup("shell");
    }
    
    // Get current directory
    char cwd[MAX_PATH_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        shell->cwd = strdup(cwd);
    } else {
        shell->cwd = strdup(getenv("HOME") ? getenv("HOME") : "/tmp");
    }
    
    // Get home directory
    shell->home_dir = strdup(getenv("HOME") ? getenv("HOME") : "/tmp");
    
    // Set up signal handlers
    signal(SIGINT, ks_shell_signal_handler);
    signal(SIGTERM, ks_shell_signal_handler);
    
    // Initialize shell
    shell->running = true;
    shell->verbose = false;
    shell->last_exit_code = 0;
    
    // Initialize subsystems
    ks_tool_init();
    ks_pipeline_init();
    ks_lua_init();
    ks_ai_init();
    
    // Print banner
    printf(COLOR_CYAN);
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│  " COLOR_BOLD "KaliShell" COLOR_RESET COLOR_CYAN " - Security Operating Environment   │\n");
    printf("│  Version %s                              │\n", KALISHELL_VERSION);
    printf("└─────────────────────────────────────────────┘\n");
    printf(COLOR_RESET);
    printf("Type " COLOR_BOLD "help" COLOR_RESET " for available commands\n\n");
    
    return KS_OK;
}

void ks_shell_cleanup(ks_shell_t *shell) {
    // Cleanup subsystems
    ks_tool_cleanup();
    ks_pipeline_cleanup();
    ks_lua_cleanup();
    ks_ai_cleanup();
    
    // Free allocated memory
    if (shell->username) free(shell->username);
    if (shell->hostname) free(shell->hostname);
    if (shell->cwd) free(shell->cwd);
    if (shell->home_dir) free(shell->home_dir);
    if (shell->workspace_dir) free(shell->workspace_dir);
    if (shell->current_workspace) free(shell->current_workspace);
    
    // Free history
    for (int i = 0; i < history_count; i++) {
        if (history[i]) free(history[i]);
    }
    history_count = 0;
}

int ks_shell_run(ks_shell_t *shell) {
    char input[MAX_INPUT_SIZE];
    
    while (shell->running) {
        // Get prompt
        char *prompt = get_prompt();
        
        // Read input
        printf("%s", prompt);
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // EOF (Ctrl+D)
            printf("\n");
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = '\0';
        
        // Skip empty input
        if (strlen(input) == 0) continue;
        
        // Execute command
        shell->last_exit_code = execute_command(input);
    }
    
    return shell->last_exit_code;
}

// Built-in command implementations
int ks_cmd_help(int argc, char **argv) {
    printf("Available commands:\n\n");
    printf("  " COLOR_BOLD "Built-in Commands:\n" COLOR_RESET);
    printf("    help              Show this help message\n");
    printf("    exit/quit         Exit the shell\n");
    printf("    clear             Clear the screen\n");
    printf("    history           Show command history\n");
    printf("    cd <dir>          Change directory\n");
    printf("    pwd               Print working directory\n");
    printf("\n");
    printf("  " COLOR_BOLD "Workspace Commands:\n" COLOR_RESET);
    printf("    workspace create <name>   Create a new workspace\n");
    printf("    workspace open <name>     Open an existing workspace\n");
    printf("    workspace list            List all workspaces\n");
    printf("    workspace close           Close current workspace\n");
    printf("\n");
    printf("  " COLOR_BOLD "Target Commands:\n" COLOR_RESET);
    printf("    target add <domain>       Add a target\n");
    printf("    target list               List all targets\n");
    printf("    target remove <domain>    Remove a target\n");
    printf("\n");
    printf("  " COLOR_BOLD "Discovery Commands:\n" COLOR_RESET);
    printf("    recon <target>            Run reconnaissance pipeline\n");
    printf("    show subdomains           Show discovered subdomains\n");
    printf("    show endpoints            Show discovered endpoints\n");
    printf("    show findings             Show all findings\n");
    printf("\n");
    printf("  " COLOR_BOLD "Tool Commands:\n" COLOR_RESET);
    printf("    tool list                 List available tools\n");
    printf("    tool run <name> [args]    Run a tool\n");
    printf("\n");
    printf("  " COLOR_BOLD "Pipeline Commands:\n" COLOR_RESET);
    printf("    pipeline list             List available pipelines\n");
    printf("    pipeline run <name> <target>  Run a pipeline\n");
    printf("\n");
    printf("  " COLOR_BOLD "Analysis Commands:\n" COLOR_RESET);
    printf("    analyze                   Analyze findings\n");
    printf("    find <query>              Search across all data\n");
    printf("    graph show                Show asset graph\n");
    printf("    graph export              Export graph to DOT\n");
    printf("\n");
    printf("  " COLOR_BOLD "Finding Commands:\n" COLOR_RESET);
    printf("    finding create <title> <severity>  Create finding\n");
    printf("    finding list              List all findings\n");
    printf("    finding update <id> <field> <value>  Update finding\n");
    printf("    finding delete <id>       Delete finding\n");
    printf("\n");
    printf("  " COLOR_BOLD "Report Commands:\n" COLOR_RESET);
    printf("    report generate [format]  Generate report\n");
    printf("    report export <format>    Export report\n");
    printf("\n");
    printf("  " COLOR_BOLD "Plugin Commands:\n" COLOR_RESET);
    printf("    plugin list               List installed plugins\n");
    printf("    plugin load <path>        Load a plugin\n");
    printf("    plugin unload <name>      Unload a plugin\n");
    printf("\n");
    printf("  " COLOR_BOLD "AI Commands:\n" COLOR_RESET);
    printf("    ai status                 Show AI status\n");
    printf("    ai enable                 Enable AI\n");
    printf("    ai disable                Disable AI\n");
    printf("    ai analyze <input>        Analyze with AI\n");
    printf("\n");
    printf("  " COLOR_BOLD "Other Commands:\n" COLOR_RESET);
    printf("    cvss <vector>             Calculate CVSS score\n");
    printf("    dsl execute <script>      Execute DSL script\n");
    printf("    dashboard                 Show dashboard\n");
    printf("\n");
    return KS_OK;
}

int ks_cmd_exit(int argc, char **argv) {
    shell.running = false;
    return KS_OK;
}

int ks_cmd_clear(int argc, char **argv) {
    printf("\033[2J\033[1;1H");
    return KS_OK;
}

int ks_cmd_history(int argc, char **argv) {
    for (int i = 0; i < history_count; i++) {
        printf("%4d  %s\n", i + 1, history[i]);
    }
    return KS_OK;
}

int ks_cmd_cd(int argc, char **argv) {
    char *dir = argc > 1 ? argv[1] : shell.home_dir;
    
    // Handle ~
    if (dir[0] == '~') {
        char new_dir[MAX_PATH_SIZE];
        snprintf(new_dir, sizeof(new_dir), "%s%s", shell.home_dir, dir + 1);
        dir = new_dir;
    }
    
    if (chdir(dir) != 0) {
        perror("cd");
        return 1;
    }
    
    // Update cwd
    char cwd[MAX_PATH_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        free(shell.cwd);
        shell.cwd = strdup(cwd);
    }
    
    return KS_OK;
}

int ks_cmd_pwd(int argc, char **argv) {
    printf("%s\n", shell.cwd);
    return KS_OK;
}
