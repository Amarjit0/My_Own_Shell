#ifndef GUPT_SESSION_H
#define GUPT_SESSION_H

#include "gupt.h"
#include <time.h>

// Session entry types
typedef enum {
    ENTRY_COMMAND,
    ENTRY_OUTPUT,
    ENTRY_FINDING,
    ENTRY_NOTE,
    ENTRY_ERROR,
    ENTRY_SYSTEM
} ks_session_entry_type_t;

// Session entry
typedef struct ks_session_entry {
    ks_session_entry_type_t type;
    time_t timestamp;
    char command[4096];
    char output[8192];
    struct ks_session_entry *next;
} ks_session_entry_t;

// Session
typedef struct {
    char name[256];
    char workspace[256];
    char target[1024];
    time_t start_time;
    ks_session_entry_t *entries;
    int entry_count;
    char log_file[1024];
    FILE *log_fp;
} ks_session_t;

// Session functions
int ks_session_init(void);
void ks_session_cleanup(void);
int ks_session_start(const char *name, const char *workspace);
int ks_session_stop(void);
int ks_session_log_command(const char *command);
int ks_session_log_output(const char *output);
int ks_session_log_finding(const char *finding);
int ks_session_log_note(const char *note);
int ks_session_log_error(const char *error);
int ks_session_timeline(void);
int ks_session_save(const char *filename);
int ks_session_load(const char *filename);
ks_session_t *ks_session_get_current(void);

#endif // GUPT_SESSION_H
