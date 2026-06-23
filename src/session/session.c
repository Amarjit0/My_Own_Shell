#include "gupt/session.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

static ks_session_t current_session = {0};

int ks_session_init(void) {
    memset(&current_session, 0, sizeof(ks_session_t));
    return KS_OK;
}

void ks_session_cleanup(void) {
    if (current_session.log_fp) {
        fclose(current_session.log_fp);
    }
    
    ks_session_entry_t *entry = current_session.entries;
    while (entry) {
        ks_session_entry_t *next = entry->next;
        free(entry);
        entry = next;
    }
    
    memset(&current_session, 0, sizeof(ks_session_t));
}

static int add_entry(ks_session_entry_type_t type, const char *command, const char *output);
static int ks_session_log_system(const char *message) {
    return add_entry(ENTRY_SYSTEM, message, NULL);
}

int ks_session_start(const char *name, const char *workspace) {
    // Stop any existing session
    if (current_session.start_time > 0) {
        ks_session_stop();
    }
    
    // Initialize new session
    strncpy(current_session.name, name ? name : "unnamed", 255);
    strncpy(current_session.workspace, workspace ? workspace : "default", 255);
    current_session.start_time = time(NULL);
    current_session.entry_count = 0;
    current_session.entries = NULL;
    
    // Create log file
    char dir[1024];
    snprintf(dir, sizeof(dir), ".gupt/sessions");
    ks_platform_mkdir_recursive(dir, 0755);
    
    char filename[1024];
    struct tm *tm = localtime(&current_session.start_time);
    snprintf(filename, sizeof(filename), 
        ".gupt/sessions/%s_%04d%02d%02d_%02d%02d%02d.log",
        name ? name : "session",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    strncpy(current_session.log_file, filename, 1023);
    current_session.log_fp = fopen(filename, "w");
    
    // Log session start
    ks_session_log_system("Session started");
    
    printf("[session] Started: %s\n", current_session.name);
    printf("[session] Log: %s\n", current_session.log_file);
    
    return KS_OK;
}

int ks_session_stop(void) {
    if (current_session.start_time == 0) {
        printf("[session] No active session\n");
        return KS_ERROR;
    }
    
    ks_session_log_system("Session stopped");
    
    if (current_session.log_fp) {
        fclose(current_session.log_fp);
        current_session.log_fp = NULL;
    }
    
    printf("[session] Stopped: %s (%d entries)\n", 
        current_session.name, current_session.entry_count);
    
    memset(&current_session, 0, sizeof(ks_session_t));
    return KS_OK;
}

static int add_entry(ks_session_entry_type_t type, const char *command, const char *output) {
    ks_session_entry_t *entry = calloc(1, sizeof(ks_session_entry_t));
    if (!entry) return KS_ERROR_NOMEM;
    
    entry->type = type;
    entry->timestamp = time(NULL);
    if (command) strncpy(entry->command, command, 4095);
    if (output) strncpy(entry->output, output, 8191);
    
    // Add to linked list
    entry->next = current_session.entries;
    current_session.entries = entry;
    current_session.entry_count++;
    
    // Write to log file
    if (current_session.log_fp) {
        struct tm *tm = localtime(&entry->timestamp);
        const char *type_str = "UNKNOWN";
        switch (type) {
            case ENTRY_COMMAND: type_str = "CMD"; break;
            case ENTRY_OUTPUT: type_str = "OUTPUT"; break;
            case ENTRY_FINDING: type_str = "FINDING"; break;
            case ENTRY_NOTE: type_str = "NOTE"; break;
            case ENTRY_ERROR: type_str = "ERROR"; break;
            case ENTRY_SYSTEM: type_str = "SYSTEM"; break;
        }
        
        fprintf(current_session.log_fp, "[%02d:%02d:%02d] [%s] %s\n",
            tm->tm_hour, tm->tm_min, tm->tm_sec, type_str, command ? command : output);
        fflush(current_session.log_fp);
    }
    
    return KS_OK;
}

int ks_session_log_command(const char *command) {
    return add_entry(ENTRY_COMMAND, command, NULL);
}

int ks_session_log_output(const char *output) {
    return add_entry(ENTRY_OUTPUT, NULL, output);
}

int ks_session_log_finding(const char *finding) {
    return add_entry(ENTRY_FINDING, finding, NULL);
}

int ks_session_log_note(const char *note) {
    return add_entry(ENTRY_NOTE, note, NULL);
}

int ks_session_log_error(const char *error) {
    return add_entry(ENTRY_ERROR, error, NULL);
}

int ks_session_timeline(void) {
    if (current_session.start_time == 0) {
        printf("[session] No active session\n");
        return KS_ERROR;
    }
    
    printf("\n");
    printf("  Session Timeline: %s\n", current_session.name);
    printf("  ─────────────────────────────────────\n\n");
    
    ks_session_entry_t *entry = current_session.entries;
    int index = current_session.entry_count;
    
    while (entry) {
        struct tm *tm = localtime(&entry->timestamp);
        
        const char *color = COLOR_WHITE;
        const char *prefix = "";
        switch (entry->type) {
            case ENTRY_COMMAND: color = COLOR_CYAN; prefix = "$ "; break;
            case ENTRY_OUTPUT: color = COLOR_WHITE; prefix = ""; break;
            case ENTRY_FINDING: color = COLOR_RED; prefix = "! "; break;
            case ENTRY_NOTE: color = COLOR_YELLOW; prefix = "# "; break;
            case ENTRY_ERROR: color = COLOR_RED; prefix = "ERROR: "; break;
            case ENTRY_SYSTEM: color = COLOR_GREEN; prefix = "* "; break;
        }
        
        printf("  %s%02d:%02d:%02d%s %s%s%s%s\n",
            COLOR_BLUE, tm->tm_hour, tm->tm_min, tm->tm_sec, COLOR_RESET,
            color, prefix, 
            entry->type == ENTRY_COMMAND ? entry->command : 
            (entry->output[0] ? entry->output : "(no output)"),
            COLOR_RESET);
        
        entry = entry->next;
        index--;
    }
    
    printf("\n");
    return KS_OK;
}

int ks_session_save(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return KS_ERROR_IO;
    
    fprintf(fp, "name: %s\n", current_session.name);
    fprintf(fp, "workspace: %s\n", current_session.workspace);
    fprintf(fp, "target: %s\n", current_session.target);
    fprintf(fp, "start_time: %ld\n", current_session.start_time);
    fprintf(fp, "entries: %d\n", current_session.entry_count);
    
    ks_session_entry_t *entry = current_session.entries;
    while (entry) {
        fprintf(fp, "---\n");
        fprintf(fp, "type: %d\n", entry->type);
        fprintf(fp, "timestamp: %ld\n", entry->timestamp);
        fprintf(fp, "command: %s\n", entry->command);
        fprintf(fp, "output: %s\n", entry->output);
        entry = entry->next;
    }
    
    fclose(fp);
    return KS_OK;
}

int ks_session_load(const char *filename) {
    // TODO: Implement session loading
    return KS_OK;
}

ks_session_t *ks_session_get_current(void) {
    return &current_session;
}
