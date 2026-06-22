#include "gupt/gupt.h"

int ks_parse_command(char *input, char **argv, int *argc) {
    if (!input || !argv || !argc) {
        return KS_ERROR_INVALID;
    }
    
    *argc = 0;
    
    // Skip leading whitespace
    while (*input && isspace(*input)) {
        input++;
    }
    
    if (!*input) {
        return KS_OK;
    }
    
    // Parse arguments
    while (*input && *argc < MAX_ARGS - 1) {
        // Skip whitespace
        while (*input && isspace(*input)) {
            input++;
        }
        
        if (!*input) {
            break;
        }
        
        // Check for quotes
        if (*input == '"' || *input == '\'') {
            char quote = *input++;
            char *start = input;
            
            // Find closing quote
            while (*input && *input != quote) {
                if (*input == '\\' && *(input + 1)) {
                    input++;  // Skip escaped character
                }
                input++;
            }
            
            if (*input == quote) {
                *input++ = '\0';
                argv[*argc] = strdup(start);
                (*argc)++;
            } else {
                // Unterminated quote
                return KS_ERROR_INVALID;
            }
        } else {
            // Unquoted argument
            char *start = input;
            while (*input && !isspace(*input)) {
                if (*input == '\\' && *(input + 1)) {
                    input++;  // Skip escaped character
                }
                input++;
            }
            
            if (*input) {
                *input++ = '\0';
            }
            
            argv[*argc] = strdup(start);
            (*argc)++;
        }
    }
    
    argv[*argc] = NULL;
    
    return KS_OK;
}

char **ks_split_args(const char *input, int *argc) {
    if (!input || !argc) {
        return NULL;
    }
    
    char **argv = malloc(sizeof(char *) * MAX_ARGS);
    if (!argv) {
        return NULL;
    }
    
    // Make a copy of input
    char *input_copy = strdup(input);
    if (!input_copy) {
        free(argv);
        return NULL;
    }
    
    int result = ks_parse_command(input_copy, argv, argc);
    free(input_copy);
    
    if (result != KS_OK) {
        // Free allocated arguments
        for (int i = 0; i < *argc; i++) {
            if (argv[i]) {
                free(argv[i]);
            }
        }
        free(argv);
        return NULL;
    }
    
    return argv;
}

void ks_free_args(char **argv, int argc) {
    if (!argv) {
        return;
    }
    
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            free(argv[i]);
        }
    }
    free(argv);
}

// String utility functions
char *ks_string_trim(char *str) {
    if (!str) {
        return NULL;
    }
    
    // Trim leading whitespace
    while (*str && isspace(*str)) {
        str++;
    }
    
    if (!*str) {
        return str;
    }
    
    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end-- = '\0';
    }
    
    return str;
}

bool ks_string_starts_with(const char *str, const char *prefix) {
    if (!str || !prefix) {
        return false;
    }
    
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool ks_string_ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return false;
    }
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return false;
    }
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

char *ks_string_duplicate(const char *str) {
    if (!str) {
        return NULL;
    }
    
    char *dup = malloc(strlen(str) + 1);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

// Path utility functions
char *ks_path_join(const char *path1, const char *path2) {
    if (!path1 || !path2) {
        return NULL;
    }
    
    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    
    // Check if path1 ends with /
    bool needs_slash = (len1 > 0 && path1[len1 - 1] != '/');
    
    size_t total_len = len1 + (needs_slash ? 1 : 0) + len2 + 1;
    char *result = malloc(total_len);
    
    if (result) {
        strcpy(result, path1);
        if (needs_slash) {
            strcat(result, "/");
        }
        strcat(result, path2);
    }
    
    return result;
}

char *ks_path_get_filename(const char *path) {
    if (!path) {
        return NULL;
    }
    
    const char *last_slash = strrchr(path, '/');
    if (last_slash) {
        return strdup(last_slash + 1);
    }
    
    return strdup(path);
}

char *ks_path_get_dirname(const char *path) {
    if (!path) {
        return NULL;
    }
    
    const char *last_slash = strrchr(path, '/');
    if (last_slash) {
        size_t len = last_slash - path;
        char *dir = malloc(len + 1);
        if (dir) {
            strncpy(dir, path, len);
            dir[len] = '\0';
        }
        return dir;
    }
    
    return strdup(".");
}

bool ks_path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

bool ks_path_is_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

int ks_path_mkdir(const char *path, mode_t mode) {
    if (!path) {
        return KS_ERROR_INVALID;
    }
    
    // Create directory recursively
    char *tmp = strdup(path);
    if (!tmp) {
        return KS_ERROR_NOMEM;
    }
    
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
                free(tmp);
                return KS_ERROR_IO;
            }
            *p = '/';
        }
    }
    
    if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
        free(tmp);
        return KS_ERROR_IO;
    }
    
    free(tmp);
    return KS_OK;
}
