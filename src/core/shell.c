#include "gupt/gupt.h"

// Command history
static char *history[MAX_HISTORY_SIZE];
static int history_count = 0;

// Built-in commands - simplified for humans
static ks_command_t builtins[] = {
    // Basic
    {"help",    "?",                  ks_cmd_help},
    {"exit",    "quit",               ks_cmd_exit},
    {"quit",    "exit",               ks_cmd_exit},
    {"clear",   "cls",                ks_cmd_clear},
    {"history", "hist",               ks_cmd_history},
    {"cd",      "chdir",              ks_cmd_cd},
    {"pwd",     "where",              ks_cmd_pwd},
    
    // Workspaces
    {"new",     "create workspace",   NULL},
    {"open",    "open workspace",     NULL},
    {"close",   "close workspace",    NULL},
    {"spaces",  "list workspaces",    NULL},
    
    // Tools
    {"tools",   "list tools",         NULL},
    {"run",     "run a tool",         NULL},
    
    // Scanning
    {"scan",    "run pipeline",       NULL},
    {"recon",   "run recon",          NULL},
    
    // Findings
    {"bug",     "manage findings",    NULL},
    {"report",  "generate report",    ks_cmd_report},
    
    // Analysis
    {"score",   "calculate CVSS",     ks_cmd_cvss},
    {"graph",   "show asset graph",   ks_cmd_graph},
    {"dash",    "show dashboard",     NULL},
    
    // Other
    {"ai",      "AI assistant",       ks_cmd_ai},
    {"search",  "find something",     NULL},
    
    {NULL, NULL, NULL}
};

void ks_shell_signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n");
    } else if (sig == SIGTERM) {
        shell.running = false;
    }
}

static void add_to_history(const char *command) {
    if (history_count >= MAX_HISTORY_SIZE) {
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
    char *user = shell.username ? shell.username : "gupt";
    char *host = shell.hostname ? shell.hostname : "shell";
    char *dir = shell.cwd ? shell.cwd : "~";
    
    // Get relative path from home
    if (shell.home_dir && strncmp(dir, shell.home_dir, strlen(shell.home_dir)) == 0) {
        dir += strlen(shell.home_dir);
        if (*dir == '/') dir++;
    }
    
    snprintf(prompt, sizeof(prompt), 
        COLOR_CYAN "%s" COLOR_RESET "@" COLOR_GREEN "%s" COLOR_RESET ":" COLOR_BLUE "~/%s" COLOR_RESET " > ",
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
    
    // Handle workspace commands
    if (strcmp(argv[0], "new") == 0 || strcmp(argv[0], "workspace") == 0) {
        if (argc < 2) {
            printf("Usage: new <name>\n");
            return 1;
        }
        return ks_workspace_create(argc > 1 ? argv[1] : NULL);
    }
    
    if (strcmp(argv[0], "open") == 0) {
        if (argc < 2) {
            printf("Usage: open <name>\n");
            return 1;
        }
        return ks_workspace_open(argv[1]);
    }
    
    if (strcmp(argv[0], "spaces") == 0 || strcmp(argv[0], "workspace") == 0) {
        return ks_workspace_list();
    }
    
    if (strcmp(argv[0], "close") == 0) {
        return ks_workspace_close();
    }
    
    // Handle tools
    if (strcmp(argv[0], "tools") == 0 || strcmp(argv[0], "tool") == 0) {
        return ks_tool_list();
    }
    
    if (strcmp(argv[0], "run") == 0) {
        if (argc < 2) {
            printf("Usage: run <tool> [args]\n");
            return 1;
        }
        return ks_orchestrator_run_tool(argv[1], argc > 2 ? argv[2] : NULL);
    }
    
    // Handle scanning
    if (strcmp(argv[0], "scan") == 0 || strcmp(argv[0], "recon") == 0) {
        if (argc < 2) {
            printf("Usage: scan <target>\n");
            return 1;
        }
        return ks_orchestrator_run_pipeline("recon", argv[1]);
    }
    
    // Handle findings
    if (strcmp(argv[0], "bug") == 0 || strcmp(argv[0], "finding") == 0) {
        if (argc < 2) {
            printf("Usage: bug create <title> <severity>\n");
            printf("       bug list\n");
            return 1;
        }
        if (strcmp(argv[1], "create") == 0) {
            if (argc < 4) {
                printf("Usage: bug create <title> <severity>\n");
                return 1;
            }
            return ks_finding_create(argv[2], 
                strcmp(argv[3], "high") == 0 ? SEVERITY_HIGH : SEVERITY_MEDIUM,
                NULL, NULL);
        }
        if (strcmp(argv[1], "list") == 0) {
            return ks_finding_list(0);
        }
    }
    
    // Handle dashboard
    if (strcmp(argv[0], "dash") == 0 || strcmp(argv[0], "dashboard") == 0) {
        return ks_tui_draw_dashboard();
    }
    
    // Handle search
    if (strcmp(argv[0], "search") == 0 || strcmp(argv[0], "find") == 0) {
        if (argc < 2) {
            printf("Usage: search <query>\n");
            return 1;
        }
        printf("[*] Searching for: %s\n", argv[1]);
        // TODO: Implement search
        return 0;
    }
    
    fprintf(stderr, "Unknown command: %s (type 'help' for commands)\n", argv[0]);
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
        shell->username = strdup("user");
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
    printf("  ____  _  __ ____  _     ___   __   __ \n");
    printf(" / ___|| |/ // ___|| |   / _ \\  \\ \\ / / \n");
    printf("| |  _ | ' /| |  _ | |  | | | |  \\ V /  \n");
    printf("| |_| || . \\| |_| || |__| |_| |   | |   \n");
    printf(" \\____||_|\\_\\\\____||_____\\___/    |_|   \n\n");
    printf(COLOR_RESET);
    printf("  Security Operating Environment\n");
    printf("  Type " COLOR_BOLD "help" COLOR_RESET " for commands\n\n");
    
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
    printf("\n");
    printf(COLOR_BOLD "  Gupt - Security Operating Environment\n" COLOR_RESET);
    printf("  ─────────────────────────────────────\n\n");
    
    printf("  " COLOR_BOLD "Basic:\n" COLOR_RESET);
    printf("    help              Show this help\n");
    printf("    exit / quit       Leave Gupt\n");
    printf("    clear             Clear screen\n");
    printf("    history           Command history\n");
    printf("    cd <dir>          Change directory\n");
    printf("    pwd               Current directory\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Workspaces:\n" COLOR_RESET);
    printf("    new <name>        Create workspace\n");
    printf("    open <name>       Open workspace\n");
    printf("    spaces            List workspaces\n");
    printf("    close             Close workspace\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Tools:\n" COLOR_RESET);
    printf("    tools             List available tools\n");
    printf("    run <tool> [args] Run a tool\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Scanning:\n" COLOR_RESET);
    printf("    scan <target>     Run full scan\n");
    printf("    recon <target>    Run reconnaissance\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Analysis:\n" COLOR_RESET);
    printf("    score <vector>    Calculate CVSS score\n");
    printf("    graph             Show asset graph\n");
    printf("    dash              Show dashboard\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Findings:\n" COLOR_RESET);
    printf("    bug create <t> <s> Report a bug\n");
    printf("    bug list          List bugs\n");
    printf("    report            Generate report\n");
    printf("\n");
    
    printf("  " COLOR_BOLD "Other:\n" COLOR_RESET);
    printf("    search <query>    Search everything\n");
    printf("    ai                AI assistant\n");
    printf("\n");
    
    return KS_OK;
}

int ks_cmd_exit(int argc, char **argv) {
    printf("Goodbye!\n");
    shell.running = false;
    return KS_OK;
}

int ks_cmd_clear(int argc, char **argv) {
    printf("\033[2J\033[1;1H");
    return KS_OK;
}

int ks_cmd_history(int argc, char **argv) {
    for (int i = 0; i < history_count; i++) {
        printf("  %3d  %s\n", i + 1, history[i]);
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
