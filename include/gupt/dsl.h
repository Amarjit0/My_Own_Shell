#ifndef GUPT_DSL_H
#define GUPT_DSL_H

#include "gupt.h"

// DSL Token types
typedef enum {
    TOK_WORD,
    TOK_STRING,
    TOK_NUMBER,
    TOK_NEWLINE,
    TOK_EOF,
    TOK_ERROR,
    TOK_COMMENT
} ks_token_type_t;

typedef struct {
    ks_token_type_t type;
    char value[1024];
    int line;
} ks_token_t;

// DSL Command types
typedef enum {
    CMD_WORKSPACE,
    CMD_TARGET,
    CMD_DISCOVER,
    CMD_TEST,
    CMD_REPORT,
    CMD_NOTE,
    CMD_SEARCH,
    CMD_RUN,
    CMD_SET,
    CMD_IF,
    CMD_PIPE,
    CMD_SEMICOLON,
    CMD_COMMENT,
    CMD_UNKNOWN
} ks_dsl_cmd_t;

// DSL AST Node
typedef struct ks_dsl_node {
    ks_dsl_cmd_t cmd;
    char args[8][1024];
    int arg_count;
    struct ks_dsl_node *next;
    struct ks_dsl_node *children;
    int line;
} ks_dsl_node_t;

// Parser state
typedef struct {
    const char *source;
    int pos;
    int line;
    ks_token_t current;
} ks_dsl_parser_t;

// DSL functions
int ks_dsl_init(void);
void ks_dsl_cleanup(void);
ks_dsl_node_t *ks_dsl_parse(const char *source);
int ks_dsl_execute(ks_dsl_node_t *node);
void ks_dsl_free(ks_dsl_node_t *node);
int ks_dsl_execute_file(const char *filename);
int ks_dsl_repl(void);

// Tokenizer
ks_token_t ks_dsl_next_token(ks_dsl_parser_t *parser);

// Built-in commands
int ks_dsl_cmd_workspace(ks_dsl_node_t *node);
int ks_dsl_cmd_target(ks_dsl_node_t *node);
int ks_dsl_cmd_discover(ks_dsl_node_t *node);
int ks_dsl_cmd_test(ks_dsl_node_t *node);
int ks_dsl_cmd_report(ks_dsl_node_t *node);
int ks_dsl_cmd_note(ks_dsl_node_t *node);
int ks_dsl_cmd_search(ks_dsl_node_t *node);
int ks_dsl_cmd_run(ks_dsl_node_t *node);
int ks_dsl_cmd_set(ks_dsl_node_t *node);

#endif // GUPT_DSL_H
