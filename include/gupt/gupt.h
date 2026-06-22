#ifndef GUPT_H
#define GUPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>
#include <signal.h>

// Constants (must be before platform section)
#define MAX_INPUT_SIZE 4096
#define MAX_ARGS 256
#define MAX_PATH_SIZE 1024
#define MAX_HISTORY_SIZE 1000

// Platform-specific headers
#ifdef _WIN32
    #ifndef GUPT_PLATFORM_WINDOWS
        #define GUPT_PLATFORM_WINDOWS
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #include <io.h>
    #include <sys/stat.h>
    #include <sys/timeb.h>
    #define PATH_SEPARATOR '\\'
    #define getcwd _getcwd
    #define chdir _chdir
    #define mkdir(p, m) _mkdir(p)
    #define access _access
    #define F_OK 0
    #define stat _stat
    #ifndef S_ISDIR
        #define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
    #endif
    typedef struct { char *pw_name; } passwd;
    static inline passwd *getpwuid(int uid) { 
        static passwd pw = {"user"};
        return &pw;
    }
    static inline int getuid() { return 0; }
    #define SIGINT 2
    #define SIGTERM 15
    static inline void (*signal(int sig, void (*handler)(int)))(int) { (void)sig; (void)handler; return NULL; }
    static inline int gettimeofday(struct timeval *tv, void *tz) {
        struct _timeb tb;
        _ftime64_s(&tb);
        tv->tv_sec = (long)tb.time;
        tv->tv_usec = tb.millitm * 1000;
        return 0;
    }
    #define strcasecmp _stricmp
    // POSIX-compatible DIR/dirent for Windows
    struct dirent {
        char d_name[MAX_PATH_SIZE];
    };
    typedef struct {
        intptr_t handle;
        struct _finddata_t info;
        struct dirent entry;
        int has_entry;
    } DIR;
    static inline DIR *opendir(const char *path) {
        static DIR dir;
        char pattern[MAX_PATH_SIZE];
        snprintf(pattern, MAX_PATH_SIZE, "%s\\*", path);
        dir.handle = _findfirst(pattern, &dir.info);
        if (dir.handle != -1) {
            strncpy(dir.entry.d_name, dir.info.name, MAX_PATH_SIZE - 1);
            dir.has_entry = 1;
        } else {
            dir.has_entry = 0;
        }
        return &dir;
    }
    static inline struct dirent *readdir(DIR *dir) {
        if (!dir->has_entry) return NULL;
        if (_findnext(dir->handle, &dir->info) == 0) {
            strncpy(dir->entry.d_name, dir->info.name, MAX_PATH_SIZE - 1);
        } else {
            dir->has_entry = 0;
        }
        return &dir->entry;
    }
    static inline int closedir(DIR *dir) {
        _findclose(dir->handle);
        return 0;
    }
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <dirent.h>
    #define PATH_SEPARATOR '/'
#endif

// Version
#define GUPT_VERSION "0.1.0"
#define GUPT_NAME "Gupt"

// Colors (ANSI)
#define COLOR_RESET     "\033[0m"
#define COLOR_BOLD      "\033[1m"
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_WHITE     "\033[37m"

// Asset types
typedef enum {
    ASSET_DOMAIN,
    ASSET_IP,
    ASSET_URL,
    ASSET_CIDR,
    ASSET_SUBDOMAIN,
    ASSET_ENDPOINT,
    ASSET_TECHNOLOGY,
    ASSET_SCREENSHOT,
    ASSET_CERTIFICATE,
} ks_asset_type_t;

// Severity levels
typedef enum {
    SEVERITY_CRITICAL,
    SEVERITY_HIGH,
    SEVERITY_MEDIUM,
    SEVERITY_LOW,
    SEVERITY_INFO,
} ks_severity_t;

// Target status
typedef enum {
    TARGET_ACTIVE,
    TARGET_COMPLETED,
    TARGET_ARCHIVED,
} ks_target_status_t;

// Finding status
typedef enum {
    FINDING_OPEN,
    FINDING_CONFIRMED,
    FINDING_FALSE_POSITIVE,
    FINDING_FIXED,
    FINDING_WONT_FIX,
} ks_finding_status_t;

// Error codes
typedef enum {
    KS_OK = 0,
    KS_ERROR = -1,
    KS_ERROR_NOMEM = -2,
    KS_ERROR_NOTFOUND = -3,
    KS_ERROR_INVALID = -4,
    KS_ERROR_IO = -5,
    KS_ERROR_DB = -6,
    KS_ERROR_TOOL = -7,
} ks_error_t;

// Shell state
typedef struct {
    char *username;
    char *hostname;
    char *cwd;
    char *home_dir;
    char *workspace_dir;
    char *current_workspace;
    bool running;
    bool verbose;
    int last_exit_code;
} ks_shell_t;

// Global shell instance (defined in main.c)
extern ks_shell_t shell;

// Command function type
typedef int (*ks_command_func)(int argc, char **argv);

// Command structure
typedef struct {
    const char *name;
    const char *description;
    ks_command_func func;
} ks_command_t;

// Function declarations
// Core
int ks_shell_init(ks_shell_t *shell);
void ks_shell_cleanup(ks_shell_t *shell);
int ks_shell_run(ks_shell_t *shell);
void ks_shell_signal_handler(int sig);

// Parser
int ks_parse_command(char *input, char **argv, int *argc);
char **ks_split_args(const char *input, int *argc);

// Commands
int ks_cmd_help(int argc, char **argv);
int ks_cmd_exit(int argc, char **argv);
int ks_cmd_clear(int argc, char **argv);
int ks_cmd_history(int argc, char **argv);
int ks_cmd_cd(int argc, char **argv);
int ks_cmd_pwd(int argc, char **argv);

// Workspace
int ks_workspace_create(const char *name);
int ks_workspace_open(const char *name);
int ks_workspace_close(void);
int ks_workspace_list(void);

// Database
int ks_db_init(const char *db_path);
void ks_db_close(void);
int ks_db_exec(const char *sql);

// Platform utilities
const char *ks_platform_get_name(void);
char ks_platform_get_path_separator(void);
int ks_platform_execute(const char *command, char *output, size_t output_size);
int ks_platform_execute_background(const char *command);
bool ks_platform_command_exists(const char *command);
char *ks_platform_get_cwd(void);
int ks_platform_set_cwd(const char *path);
char *ks_platform_get_home(void);
int ks_platform_mkdir_recursive(const char *path, mode_t mode);
time_t ks_platform_get_mtime(const char *path);
bool ks_platform_is_executable(const char *path);
int ks_platform_get_pid(void);
void ks_platform_sleep_ms(int ms);

// String utilities
char *ks_string_trim(char *str);
bool ks_string_starts_with(const char *str, const char *prefix);
bool ks_string_ends_with(const char *str, const char *suffix);
char *ks_string_duplicate(const char *str);
char *ks_string_to_lower(const char *str);
char *ks_string_to_upper(const char *str);
char *ks_string_replace(const char *str, const char *old, const char *new_str);
int ks_string_count(const char *str, char c);
bool ks_string_contains(const char *str, const char *substr);

// Path utilities
char *ks_path_join(const char *path1, const char *path2);
char *ks_path_get_filename(const char *path);
char *ks_path_get_dirname(const char *path);
bool ks_path_exists(const char *path);
bool ks_path_is_dir(const char *path);
int ks_path_mkdir(const char *path, mode_t mode);

// File utilities
char *ks_file_read(const char *path);
int ks_file_write(const char *path, const char *content);
int ks_file_append(const char *path, const char *content);
long ks_file_size(const char *path);
char **ks_file_list_dir(const char *path, int *count);
void ks_file_list_free(char **list, int count);

// Configuration
int ks_config_load(const char *config_path);
const char *ks_config_get(const char *key);
const char *ks_config_get_with_default(const char *key, const char *default_value);
void ks_config_set(const char *key, const char *value);
int ks_config_save(const char *config_path);
void ks_config_cleanup(void);

// Tools
typedef struct {
    char *name;
    char *version;
    char *description;
    char *command;
    char *args_template;
    char *output_format;
    char *category;
    char **tags;
    int tag_count;
    bool installed;
    char *install_path;
} ks_tool_def_t;

int ks_tool_init(void);
void ks_tool_cleanup(void);
int ks_tool_load_definitions(const char *dir_path);
int ks_tool_load_from_file(const char *file_path);
int ks_tool_register(const char *name, const char *version, const char *description, const char *command);
ks_tool_def_t *ks_tool_get(const char *name);
bool ks_tool_is_installed(const char *name);
int ks_tool_list(void);
int ks_tool_execute(const char *name, const char *args, char *output, size_t output_size);
int ks_tool_execute_to_file(const char *name, const char *args, const char *output_file);
int ks_tool_execute_with_template(const char *name, const char *target, const char *input_file, const char *output_file);

// Orchestrator
int ks_orchestrator_run_pipeline(const char *pipeline_name, const char *target);
int ks_orchestrator_run_tool(const char *tool_name, const char *args);
int ks_orchestrator_run_workflow(const char *workflow_name, const char *target);

// Pipelines
int ks_pipeline_init(void);
void ks_pipeline_cleanup(void);
int ks_pipeline_create(const char *name, const char *description);
int ks_pipeline_add_step(const char *pipeline_name, const char *tool_name, const char *args);
int ks_pipeline_run(const char *pipeline_name, const char *target);
int ks_pipeline_list(void);

// Findings
int ks_finding_create(const char *title, ks_severity_t severity, const char *description, const char *recommendation);
int ks_finding_update(int64_t id, const char *field, const char *value);
int ks_finding_list(int64_t target_id);
int ks_finding_delete(int64_t id);

// Templates
const char *ks_template_get(const char *name);
int ks_template_list(void);

// CVSS
float ks_cvss_calculate(int av, int ac, int pr, int ui, int s, int c, int i, int a);
const char *ks_cvss_severity(float score);

// Reports
int ks_report_generate(int64_t target_id, const char *format);
int ks_report_preview(int64_t target_id);
int ks_report_export(int64_t target_id, const char *format);

// AI
int ks_ai_init(void);
void ks_ai_cleanup(void);
void ks_ai_set_enabled(bool enabled);
bool ks_ai_is_enabled(void);
int ks_ai_analyze(const char *input, char *output, size_t output_size);
int ks_ai_suggest(const char *context, char *output, size_t output_size);
int ks_cmd_ai(int argc, char **argv);

// Ollama
int ks_ollama_init(const char *host, int port, const char *model);
int ks_ollama_generate(const char *prompt, char *output, size_t output_size);

// DSL
int ks_dsl_execute(const char *script);
int ks_dsl_execute_file(const char *filename);
int ks_cmd_dsl(int argc, char **argv);

// Lua
int ks_lua_init(void);
void ks_lua_cleanup(void);
int ks_lua_execute(const char *script);
int ks_lua_execute_file(const char *filename);

// TUI
int ks_tui_init(void);
void ks_tui_cleanup(void);
int ks_tui_draw_dashboard(void);
int ks_tui_draw_progress(const char *label, int percent);
int ks_tui_draw_box(const char *title, const char *content);
int ks_tui_draw_table(const char *headers[], const char *data[][8], int rows, int cols);
int ks_tui_draw_spinner(int spin);
int ks_tui_clear_line(void);

// Graph types
typedef struct ks_graph_node {
    int64_t id;
    int64_t asset_id;
    char *value;
    ks_asset_type_t type;
    struct ks_graph_node **children;
    int child_count;
    int child_capacity;
} ks_graph_node_t;

typedef struct ks_graph_edge {
    int64_t source_id;
    int64_t target_id;
    char *relationship;
    struct ks_graph_edge *next;
} ks_graph_edge_t;

typedef struct ks_graph {
    ks_graph_node_t *root;
    ks_graph_edge_t *edges;
    int node_count;
    int edge_count;
} ks_graph_t;

ks_graph_t *ks_graph_create(void);
void ks_graph_free(ks_graph_t *graph);
int ks_graph_add_edge(ks_graph_t *graph, int64_t source_id, int64_t target_id, const char *relationship);
int ks_graph_build_from_db(int64_t target_id);
ks_graph_node_t *ks_graph_find_node(ks_graph_t *graph, const char *value);
int ks_graph_visualize(ks_graph_t *graph);
void ks_graph_print_children(ks_graph_node_t *node, const char *prefix, int is_last);
int ks_graph_export_dot(ks_graph_t *graph, const char *filename);
void ks_graph_export_node_dot(ks_graph_node_t *node, FILE *f, const char *parent_id);
ks_graph_t *ks_graph_get_current(void);
int ks_cmd_graph(int argc, char **argv);

// Commands
int ks_cmd_report(int argc, char **argv);
int ks_cmd_pipeline(int argc, char **argv);
int ks_cmd_plugin(int argc, char **argv);
int ks_cmd_finding(int argc, char **argv);
int ks_cmd_cvss(int argc, char **argv);

// Orchestrator
int ks_orchestrator_init(void);
int ks_orchestrator_run_tool(const char *tool_name, const char *args);
int ks_orchestrator_run_pipeline(const char *pipeline_name, const char *target);
int ks_orchestrator_run_workflow(const char *workflow_name, const char *target);
int ks_orchestrator_list_workflows(void);

// Logging
typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} ks_log_level_t;

void ks_log_set_level(ks_log_level_t level);
void ks_log_set_file(const char *path);
void ks_log(ks_log_level_t level, const char *file, int line, const char *fmt, ...);
void ks_log_cleanup(void);

// Log macros
#define KS_LOG_TRACE(fmt, ...) ks_log(LOG_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define KS_LOG_DEBUG(fmt, ...) ks_log(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define KS_LOG_INFO(fmt, ...) ks_log(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define KS_LOG_WARN(fmt, ...) ks_log(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define KS_LOG_ERROR(fmt, ...) ks_log(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define KS_LOG_FATAL(fmt, ...) ks_log(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // GUPT_H
