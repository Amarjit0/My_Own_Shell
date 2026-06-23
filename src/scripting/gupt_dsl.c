#include "gupt/knowledge.h"
#include "gupt/tool_adapter.h"
#include "gupt/dsl.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Initialize DSL
int ks_dsl_init(void) {
    return KS_OK;
}

void ks_dsl_cleanup(void) {
}

// Tokenizer
ks_token_t ks_dsl_next_token(ks_dsl_parser_t *parser) {
    ks_token_t token = {0};
    token.line = parser->line;
    
    // Skip whitespace (but not newlines)
    while (parser->source[parser->pos] == ' ' || 
           parser->source[parser->pos] == '\t') {
        parser->pos++;
    }
    
    char c = parser->source[parser->pos];
    
    // End of input
    if (c == '\0') {
        token.type = TOK_EOF;
        return token;
    }
    
    // Newline
    if (c == '\n') {
        token.type = TOK_NEWLINE;
        token.value[0] = '\n';
        parser->pos++;
        parser->line++;
        return token;
    }
    
    // Comment
    if (c == '#') {
        token.type = TOK_COMMENT;
        int i = 0;
        while (parser->source[parser->pos] != '\n' && 
               parser->source[parser->pos] != '\0' &&
               i < 1023) {
            token.value[i++] = parser->source[parser->pos++];
        }
        token.value[i] = '\0';
        return token;
    }
    
    // String
    if (c == '"' || c == '\'') {
        char quote = c;
        parser->pos++;
        int i = 0;
        while (parser->source[parser->pos] != quote && 
               parser->source[parser->pos] != '\0' &&
               i < 1023) {
            token.value[i++] = parser->source[parser->pos++];
        }
        token.value[i] = '\0';
        if (parser->source[parser->pos] == quote) parser->pos++;
        token.type = TOK_STRING;
        return token;
    }
    
    // Number
    if (isdigit(c)) {
        int i = 0;
        while (isdigit(parser->source[parser->pos]) && i < 1023) {
            token.value[i++] = parser->source[parser->pos++];
        }
        token.value[i] = '\0';
        token.type = TOK_NUMBER;
        return token;
    }
    
    // Word (command or argument)
    int i = 0;
    while (parser->source[parser->pos] != ' ' && 
           parser->source[parser->pos] != '\n' &&
           parser->source[parser->pos] != '\t' &&
           parser->source[parser->pos] != '\0' &&
           parser->source[parser->pos] != '#' &&
           i < 1023) {
        token.value[i++] = parser->source[parser->pos++];
    }
    token.value[i] = '\0';
    token.type = TOK_WORD;
    
    return token;
}

// Get command type from string
static ks_dsl_cmd_t get_cmd_type(const char *word) {
    if (strcmp(word, "workspace") == 0) return CMD_WORKSPACE;
    if (strcmp(word, "target") == 0) return CMD_TARGET;
    if (strcmp(word, "discover") == 0) return CMD_DISCOVER;
    if (strcmp(word, "test") == 0) return CMD_TEST;
    if (strcmp(word, "report") == 0) return CMD_REPORT;
    if (strcmp(word, "note") == 0) return CMD_NOTE;
    if (strcmp(word, "search") == 0) return CMD_SEARCH;
    if (strcmp(word, "run") == 0) return CMD_RUN;
    if (strcmp(word, "set") == 0) return CMD_SET;
    return CMD_UNKNOWN;
}

// Parse DSL source into AST
ks_dsl_node_t *ks_dsl_parse(const char *source) {
    ks_dsl_parser_t parser = {
        .source = source,
        .pos = 0,
        .line = 1
    };
    
    ks_dsl_node_t *head = NULL;
    ks_dsl_node_t *tail = NULL;
    
    while (1) {
        ks_token_t token = ks_dsl_next_token(&parser);
        
        if (token.type == TOK_EOF) break;
        if (token.type == TOK_NEWLINE) continue;
        if (token.type == TOK_COMMENT) continue;
        
        if (token.type == TOK_WORD) {
            ks_dsl_node_t *node = calloc(1, sizeof(ks_dsl_node_t));
            node->cmd = get_cmd_type(token.value);
            node->line = token.line;
            strncpy(node->args[0], token.value, 1023);
            node->arg_count = 1;
            
            // Collect arguments until newline or EOF
            while (1) {
                token = ks_dsl_next_token(&parser);
                if (token.type == TOK_NEWLINE || token.type == TOK_EOF) break;
                if (token.type == TOK_COMMENT) continue;
                if (node->arg_count < 8) {
                    strncpy(node->args[node->arg_count++], token.value, 1023);
                }
            }
            
            // Add to linked list
            if (!head) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }
    }
    
    return head;
}

// Execute DSL AST
int ks_dsl_execute(ks_dsl_node_t *node) {
    int result = KS_OK;
    
    while (node) {
        switch (node->cmd) {
            case CMD_WORKSPACE:
                result = ks_dsl_cmd_workspace(node);
                break;
            case CMD_TARGET:
                result = ks_dsl_cmd_target(node);
                break;
            case CMD_DISCOVER:
                result = ks_dsl_cmd_discover(node);
                break;
            case CMD_TEST:
                result = ks_dsl_cmd_test(node);
                break;
            case CMD_REPORT:
                result = ks_dsl_cmd_report(node);
                break;
            case CMD_NOTE:
                result = ks_dsl_cmd_note(node);
                break;
            case CMD_SEARCH:
                result = ks_dsl_cmd_search(node);
                break;
            case CMD_RUN:
                result = ks_dsl_cmd_run(node);
                break;
            case CMD_SET:
                result = ks_dsl_cmd_set(node);
                break;
            default:
                fprintf(stderr, "Line %d: Unknown command: %s\n", 
                    node->line, node->args[0]);
                result = KS_ERROR;
        }
        
        if (result != KS_OK) {
            fprintf(stderr, "Line %d: Command failed: %s\n", 
                node->line, node->args[0]);
        }
        
        node = node->next;
    }
    
    return result;
}

// Free AST
void ks_dsl_free(ks_dsl_node_t *node) {
    while (node) {
        ks_dsl_node_t *next = node->next;
        free(node);
        node = next;
    }
}

// Execute DSL file
int ks_dsl_execute_file(const char *filename) {
    char *content = ks_file_read(filename);
    if (!content) {
        fprintf(stderr, "Cannot read file: %s\n", filename);
        return KS_ERROR_IO;
    }
    
    ks_dsl_node_t *ast = ks_dsl_parse(content);
    int result = ks_dsl_execute(ast);
    
    ks_dsl_free(ast);
    free(content);
    
    return result;
}

// DSL REPL
int ks_dsl_repl(void) {
    char input[4096];
    
    printf("Gupt DSL (type 'end' to execute, 'quit' to exit)\n\n");
    
    while (1) {
        printf("gupt> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) break;
        
        if (strcmp(input, "end") == 0) {
            break;
        }
        
        if (strlen(input) > 0) {
            ks_dsl_node_t *node = ks_dsl_parse(input);
            ks_dsl_execute(node);
            ks_dsl_free(node);
        }
    }
    
    return KS_OK;
}

// Command implementations

int ks_dsl_cmd_workspace(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: workspace <name>\n");
        return KS_ERROR_INVALID;
    }
    
    printf("[workspace] Creating workspace: %s\n", node->args[1]);
    return ks_workspace_create(node->args[1]);
}

int ks_dsl_cmd_target(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: target <url>\n");
        return KS_ERROR_INVALID;
    }
    
    printf("[target] Setting target: %s\n", node->args[1]);
    // TODO: Store target in current session
    return KS_OK;
}

int ks_dsl_cmd_discover(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: discover <type>\n");
        printf("  Types: subdomains, endpoints, technologies\n");
        return KS_ERROR_INVALID;
    }
    
    const char *type = node->args[1];
    printf("[discover] Running discovery: %s\n", type);
    
    if (strcmp(type, "subdomains") == 0) {
        return ks_adapter_run("subfinder", NULL, NULL, NULL);
    } else if (strcmp(type, "endpoints") == 0) {
        return ks_adapter_run("katana", NULL, NULL, NULL);
    } else if (strcmp(type, "technologies") == 0) {
        return ks_adapter_run("httpx", NULL, NULL, NULL);
    }
    
    printf("Unknown discovery type: %s\n", type);
    return KS_ERROR_INVALID;
}

int ks_dsl_cmd_test(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: test <type>\n");
        printf("  Types: authentication, cors, xss, sqli\n");
        return KS_ERROR_INVALID;
    }
    
    const char *type = node->args[1];
    printf("[test] Running test: %s\n", type);
    
    if (strcmp(type, "authentication") == 0) {
        // TODO: Run authentication tests
        printf("  Testing authentication...\n");
    } else if (strcmp(type, "cors") == 0) {
        // TODO: Run CORS tests
        printf("  Testing CORS...\n");
    } else if (strcmp(type, "xss") == 0) {
        return ks_adapter_run("dalfox", NULL, NULL, NULL);
    } else if (strcmp(type, "sqli") == 0) {
        return ks_adapter_run("sqlmap", NULL, NULL, NULL);
    }
    
    return KS_OK;
}

int ks_dsl_cmd_report(ks_dsl_node_t *node) {
    const char *format = node->arg_count >= 2 ? node->args[1] : "markdown";
    printf("[report] Generating report: %s\n", format);
    // TODO: Generate report
    return KS_OK;
}

int ks_dsl_cmd_note(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: note <text>\n");
        return KS_ERROR_INVALID;
    }
    
    // Join all args after "note"
    char note[4096] = {0};
    for (int i = 1; i < node->arg_count; i++) {
        if (i > 1) strcat(note, " ");
        strcat(note, node->args[i]);
    }
    
    printf("[note] %s\n", note);
    // TODO: Store note in workspace
    return KS_OK;
}

int ks_dsl_cmd_search(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: search <query>\n");
        return KS_ERROR_INVALID;
    }
    
    printf("[search] Searching for: %s\n", node->args[1]);
    // TODO: Search across workspace
    return KS_OK;
}

int ks_dsl_cmd_run(ks_dsl_node_t *node) {
    if (node->arg_count < 2) {
        printf("Usage: run <tool> [args]\n");
        return KS_ERROR_INVALID;
    }
    
    printf("[run] Running tool: %s\n", node->args[1]);
    return ks_adapter_run(node->args[1], NULL, NULL, NULL);
}

int ks_dsl_cmd_set(ks_dsl_node_t *node) {
    if (node->arg_count < 3) {
        printf("Usage: set <key> <value>\n");
        return KS_ERROR_INVALID;
    }
    
    printf("[set] %s = %s\n", node->args[1], node->args[2]);
    // TODO: Store setting
    return KS_OK;
}
