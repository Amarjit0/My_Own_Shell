#ifndef GUPT_KNOWLEDGE_H
#define GUPT_KNOWLEDGE_H

#include "gupt.h"

// Note types
typedef enum {
    NOTE_GENERAL,
    NOTE_FINDING,
    NOTE_ENDPOINT,
    NOTE_CREDENTIAL,
    NOTE_SCREENSHOT,
    NOTE_TODO
} ks_note_type_t;

// Note
typedef struct ks_note {
    int64_t id;
    ks_note_type_t type;
    char title[256];
    char content[8192];
    char tags[512];
    time_t created;
    time_t modified;
    struct ks_note *next;
} ks_note_t;

// Knowledge base
typedef struct {
    ks_note_t *notes;
    int note_count;
    char db_path[1024];
} ks_knowledge_t;

// Knowledge base functions
int ks_knowledge_init(const char *workspace);
void ks_knowledge_cleanup(void);
int ks_knowledge_add(ks_note_type_t type, const char *title, const char *content, const char *tags);
int ks_knowledge_update(int64_t id, const char *content);
int ks_knowledge_delete(int64_t id);
ks_note_t *ks_knowledge_get(int64_t id);
int ks_knowledge_search(const char *query);
int ks_knowledge_list(ks_note_type_t type);
int ks_knowledge_export(const char *filename);

#endif // GUPT_KNOWLEDGE_H
