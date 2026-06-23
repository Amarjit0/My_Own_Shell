#include "gupt/knowledge.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>

static ks_knowledge_t knowledge = {0};
static sqlite3 *db = NULL;

int ks_knowledge_init(const char *workspace) {
    knowledge.note_count = 0;
    knowledge.notes = NULL;
    
    // Create database path
    snprintf(knowledge.db_path, sizeof(knowledge.db_path), 
        "%s/knowledge.db", workspace ? workspace : ".");
    
    // Open database
    int rc = sqlite3_open(knowledge.db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open knowledge database: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    // Create table
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS notes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "type INTEGER DEFAULT 0,"
        "title TEXT NOT NULL,"
        "content TEXT,"
        "tags TEXT,"
        "created INTEGER,"
        "modified INTEGER"
        ");";
    
    rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create notes table: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    return KS_OK;
}

void ks_knowledge_cleanup(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
    
    ks_note_t *note = knowledge.notes;
    while (note) {
        ks_note_t *next = note->next;
        free(note);
        note = next;
    }
    
    knowledge.notes = NULL;
    knowledge.note_count = 0;
}

int ks_knowledge_add(ks_note_type_t type, const char *title, const char *content, const char *tags) {
    if (!db || !title) return KS_ERROR;
    
    time_t now = time(NULL);
    
    const char *sql = "INSERT INTO notes (type, title, content, tags, created, modified) VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    sqlite3_bind_int(stmt, 1, type);
    sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content ? content : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, tags ? tags : "", -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, now);
    sqlite3_bind_int64(stmt, 6, now);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    printf("[knowledge] Added: %s\n", title);
    return KS_OK;
}

int ks_knowledge_search(const char *query) {
    if (!db || !query) return KS_ERROR;
    
    printf("\n");
    printf("  Search Results: \"%s\"\n", query);
    printf("  ─────────────────────\n\n");
    
    char sql[1024];
    snprintf(sql, sizeof(sql), 
        "SELECT id, type, title, content, tags FROM notes "
        "WHERE title LIKE '%%%s%%' OR content LIKE '%%%s%%' OR tags LIKE '%%%s%%'",
        query, query, query);
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        int type = sqlite3_column_int(stmt, 1);
        const char *title = (const char *)sqlite3_column_text(stmt, 2);
        const char *content = (const char *)sqlite3_column_text(stmt, 3);
        const char *tags = (const char *)sqlite3_column_text(stmt, 4);
        
        const char *type_str = "general";
        switch (type) {
            case NOTE_FINDING: type_str = "finding"; break;
            case NOTE_ENDPOINT: type_str = "endpoint"; break;
            case NOTE_CREDENTIAL: type_str = "credential"; break;
            case NOTE_TODO: type_str = "todo"; break;
        }
        
        printf("  %s#%ld%s [%s] %s%s%s\n",
            COLOR_CYAN, id, COLOR_RESET,
            type_str,
            COLOR_BOLD, title, COLOR_RESET);
        
        if (content && strlen(content) > 0) {
            printf("    %.80s%s\n", content, strlen(content) > 80 ? "..." : "");
        }
        
        if (tags && strlen(tags) > 0) {
            printf("    %sTags: %s%s\n", COLOR_YELLOW, tags, COLOR_RESET);
        }
        
        printf("\n");
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (count == 0) {
        printf("  No results found.\n\n");
    } else {
        printf("  Found %d results.\n\n", count);
    }
    
    return count;
}

int ks_knowledge_list(ks_note_type_t type) {
    if (!db) return KS_ERROR;
    
    printf("\n");
    printf("  Notes\n");
    printf("  ─────\n\n");
    
    const char *sql;
    sqlite3_stmt *stmt;
    
    if (type >= 0) {
        sql = "SELECT id, type, title, content, tags FROM notes WHERE type = ? ORDER BY created DESC";
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, type);
    } else {
        sql = "SELECT id, type, title, content, tags FROM notes ORDER BY created DESC";
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        int t = sqlite3_column_int(stmt, 1);
        const char *title = (const char *)sqlite3_column_text(stmt, 2);
        
        const char *type_str = "general";
        switch (t) {
            case NOTE_FINDING: type_str = "finding"; break;
            case NOTE_ENDPOINT: type_str = "endpoint"; break;
            case NOTE_CREDENTIAL: type_str = "credential"; break;
            case NOTE_TODO: type_str = "todo"; break;
        }
        
        printf("  %s#%ld%s [%s] %s\n",
            COLOR_CYAN, id, COLOR_RESET, type_str, title);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("\n  Total: %d notes\n\n", count);
    
    return count;
}

int ks_knowledge_delete(int64_t id) {
    if (!db) return KS_ERROR;
    
    const char *sql = "DELETE FROM notes WHERE id = ?";
    sqlite3_stmt *stmt;
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc == SQLITE_DONE) {
        printf("[knowledge] Deleted note #%ld\n", id);
        return KS_OK;
    }
    
    return KS_ERROR_DB;
}

int ks_knowledge_export(const char *filename) {
    // TODO: Export to markdown
    return KS_OK;
}
