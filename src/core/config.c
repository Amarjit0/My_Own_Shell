#include "kalishell/kalishell.h"

typedef struct {
    char *key;
    char *value;
} ks_config_entry_t;

typedef struct {
    ks_config_entry_t *entries;
    int count;
    int capacity;
} ks_config_t;

static ks_config_t config = {NULL, 0, 0};

static char *trim_whitespace(char *str) {
    while (*str && isspace(*str)) str++;
    if (!*str) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) *end-- = '\0';
    return str;
}

int ks_config_load(const char *config_path) {
    FILE *file = fopen(config_path, "r");
    if (!file) {
        // Config file not found, use defaults
        return KS_OK;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        char *trimmed = trim_whitespace(line);
        if (*trimmed == '#' || *trimmed == '\0') {
            continue;
        }
        
        // Parse key: value
        char *colon = strchr(trimmed, ':');
        if (!colon) {
            continue;
        }
        
        *colon = '\0';
        char *key = trim_whitespace(trimmed);
        char *value = trim_whitespace(colon + 1);
        
        // Remove quotes from value
        if (*value == '"' || *value == '\'') {
            char quote = *value++;
            char *end_quote = strchr(value, quote);
            if (end_quote) {
                *end_quote = '\0';
            }
        }
        
        // Add to config
        if (config.count >= config.capacity) {
            config.capacity = config.capacity ? config.capacity * 2 : 16;
            config.entries = realloc(config.entries, 
                sizeof(ks_config_entry_t) * config.capacity);
        }
        
        config.entries[config.count].key = strdup(key);
        config.entries[config.count].value = strdup(value);
        config.count++;
    }
    
    fclose(file);
    return KS_OK;
}

const char *ks_config_get(const char *key) {
    for (int i = 0; i < config.count; i++) {
        if (strcmp(config.entries[i].key, key) == 0) {
            return config.entries[i].value;
        }
    }
    return NULL;
}

const char *ks_config_get_with_default(const char *key, const char *default_value) {
    const char *value = ks_config_get(key);
    return value ? value : default_value;
}

void ks_config_set(const char *key, const char *value) {
    // Update existing entry
    for (int i = 0; i < config.count; i++) {
        if (strcmp(config.entries[i].key, key) == 0) {
            free(config.entries[i].value);
            config.entries[i].value = strdup(value);
            return;
        }
    }
    
    // Add new entry
    if (config.count >= config.capacity) {
        config.capacity = config.capacity ? config.capacity * 2 : 16;
        config.entries = realloc(config.entries, 
            sizeof(ks_config_entry_t) * config.capacity);
    }
    
    config.entries[config.count].key = strdup(key);
    config.entries[config.count].value = strdup(value);
    config.count++;
}

int ks_config_save(const char *config_path) {
    FILE *file = fopen(config_path, "w");
    if (!file) {
        return KS_ERROR_IO;
    }
    
    fprintf(file, "# KaliShell Configuration\n\n");
    fprintf(file, "# General Settings\n");
    
    for (int i = 0; i < config.count; i++) {
        fprintf(file, "%s: \"%s\"\n", 
            config.entries[i].key, 
            config.entries[i].value);
    }
    
    fclose(file);
    return KS_OK;
}

void ks_config_cleanup(void) {
    for (int i = 0; i < config.count; i++) {
        free(config.entries[i].key);
        free(config.entries[i].value);
    }
    free(config.entries);
    config.entries = NULL;
    config.count = 0;
    config.capacity = 0;
}
