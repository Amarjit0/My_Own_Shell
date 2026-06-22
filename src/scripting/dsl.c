#include "kalishell/kalishell.h"

extern ks_shell_t shell;

// DSL command types
typedef enum {
    DSL_CMD_TARGET,
    DSL_CMD_DISCOVER,
    DSL_CMD_TEST,
    DSL_CMD_FIND,
    DSL_CMD_SHOW,
    DSL_CMD_RUN,
    DSL_CMD_IF,
    DSL_CMD_FOR,
    DSL_CMD_END,
    DSL_CMD_UNKNOWN
} dsl_cmd_type_t;

// DSL command structure
typedef struct {
    dsl_cmd_type_t type;
    char *arg1;
    char *arg2;
    char *arg3;
} dsl_command_t;

// Parse DSL command
static dsl_cmd_type_t dsl_parse_command(const char *cmd) {
    if (!cmd) return DSL_CMD_UNKNOWN;
    
    if (strcmp(cmd, "target") == 0) return DSL_CMD_TARGET;
    if (strcmp(cmd, "discover") == 0) return DSL_CMD_DISCOVER;
    if (strcmp(cmd, "test") == 0) return DSL_CMD_TEST;
    if (strcmp(cmd, "find") == 0) return DSL_CMD_FIND;
    if (strcmp(cmd, "show") == 0) return DSL_CMD_SHOW;
    if (strcmp(cmd, "run") == 0) return DSL_CMD_RUN;
    if (strcmp(cmd, "if") == 0) return DSL_CMD_IF;
    if (strcmp(cmd, "for") == 0) return DSL_CMD_FOR;
    if (strcmp(cmd, "end") == 0) return DSL_CMD_END;
    
    return DSL_CMD_UNKNOWN;
}

// Execute DSL command
int ks_dsl_execute_command(const dsl_command_t *cmd) {
    if (!cmd) return KS_ERROR_INVALID;
    
    switch (cmd->type) {
        case DSL_CMD_TARGET:
            printf("[*] Target: %s\n", cmd->arg1);
            // TODO: Set target in workspace
            break;
            
        case DSL_CMD_DISCOVER:
            if (cmd->arg1) {
                printf("[*] Discovering: %s\n", cmd->arg1);
                // Run appropriate tool based on discovery type
                if (strcmp(cmd->arg1, "subdomains") == 0) {
                    ks_orchestrator_run_tool("subfinder", "-d example.com -o subdomains.txt");
                } else if (strcmp(cmd->arg1, "endpoints") == 0) {
                    ks_orchestrator_run_tool("katana", "-u example.com -o endpoints.txt");
                } else if (strcmp(cmd->arg1, "technologies") == 0) {
                    ks_orchestrator_run_tool("whatweb", "example.com");
                }
            }
            break;
            
        case DSL_CMD_TEST:
            if (cmd->arg1) {
                printf("[*] Testing: %s\n", cmd->arg1);
                // Run appropriate test
                if (strcmp(cmd->arg1, "xss") == 0) {
                    ks_orchestrator_run_tool("dalfox", "-u example.com");
                } else if (strcmp(cmd->arg1, "sqli") == 0) {
                    ks_orchestrator_run_tool("sqlmap", "-u example.com --batch");
                } else if (strcmp(cmd->arg1, "cors") == 0) {
                    printf("  Testing CORS configuration...\n");
                } else if (strcmp(cmd->arg1, "redirects") == 0) {
                    printf("  Testing redirect chains...\n");
                } else if (strcmp(cmd->arg1, "authentication") == 0) {
                    printf("  Testing authentication...\n");
                }
            }
            break;
            
        case DSL_CMD_FIND:
            if (cmd->arg1) {
                printf("[*] Searching for: %s\n", cmd->arg1);
                // TODO: Search in workspace data
            }
            break;
            
        case DSL_CMD_SHOW:
            if (cmd->arg1) {
                printf("[*] Showing: %s\n", cmd->arg1);
                // TODO: Show assets/findings
            }
            break;
            
        case DSL_CMD_RUN:
            if (cmd->arg1) {
                printf("[*] Running: %s\n", cmd->arg1);
                // TODO: Execute tool/pipeline
            }
            break;
            
        case DSL_CMD_IF:
            // TODO: Implement conditional logic
            printf("[*] Conditional: %s\n", cmd->arg1 ? cmd->arg1 : "");
            break;
            
        case DSL_CMD_FOR:
            // TODO: Implement loop
            printf("[*] Loop: %s\n", cmd->arg1 ? cmd->arg1 : "");
            break;
            
        case DSL_CMD_END:
            // End of block
            break;
            
        default:
            fprintf(stderr, "Unknown DSL command: %s\n", cmd->arg1);
            return KS_ERROR_INVALID;
    }
    
    return KS_OK;
}

// Parse and execute DSL line
int ks_dsl_parse_line(const char *line) {
    if (!line || !*line) return KS_OK;
    
    // Skip whitespace
    while (*line && isspace(*line)) line++;
    if (!*line || *line == '#') return KS_OK;  // Skip comments
    
    // Tokenize
    char *tokens[8];
    int token_count = 0;
    
    char *token = strtok((char *)line, " \t");
    while (token && token_count < 8) {
        tokens[token_count++] = token;
        token = strtok(NULL, " \t");
    }
    
    if (token_count == 0) return KS_OK;
    
    // Parse command
    dsl_command_t cmd = {0};
    cmd.type = dsl_parse_command(tokens[0]);
    cmd.arg1 = token_count > 1 ? tokens[1] : NULL;
    cmd.arg2 = token_count > 2 ? tokens[2] : NULL;
    cmd.arg3 = token_count > 3 ? tokens[3] : NULL;
    
    return ks_dsl_execute_command(&cmd);
}

// Execute DSL script
int ks_dsl_execute(const char *script) {
    if (!script) return KS_ERROR_INVALID;
    
    // Make a copy for tokenization
    char *script_copy = strdup(script);
    if (!script_copy) return KS_ERROR_NOMEM;
    
    // Split by newlines
    char *line = strtok(script_copy, "\n");
    while (line) {
        int result = ks_dsl_parse_line(line);
        if (result != KS_OK) {
            free(script_copy);
            return result;
        }
        line = strtok(NULL, "\n");
    }
    
    free(script_copy);
    return KS_OK;
}

// Execute DSL file
int ks_dsl_execute_file(const char *filename) {
    if (!filename) return KS_ERROR_INVALID;
    
    char *content = ks_file_read(filename);
    if (!content) {
        fprintf(stderr, "Cannot read file: %s\n", filename);
        return KS_ERROR_IO;
    }
    
    int result = ks_dsl_execute(content);
    free(content);
    
    return result;
}

// DSL command handler
int ks_cmd_dsl(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: dsl <execute|run> <script|file>\n");
        return 1;
    }
    
    if (strcmp(argv[1], "execute") == 0 || strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: dsl execute <script>\n");
            return 1;
        }
        
        // Check if it's a file
        if (ks_path_exists(argv[2])) {
            return ks_dsl_execute_file(argv[2]);
        }
        
        // Otherwise execute as inline script
        return ks_dsl_execute(argv[2]);
    }
    
    fprintf(stderr, "Unknown DSL command: %s\n", argv[1]);
    return 1;
}
