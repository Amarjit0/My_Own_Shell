#include "kalishell/kalishell.h"
#include <sqlite3.h>

static sqlite3 *db = NULL;

int ks_db_init(const char *db_path) {
    if (db) {
        ks_db_close();
    }
    
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    // Enable WAL mode for better performance
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    
    // Create tables
    const char *create_tables =
        "CREATE TABLE IF NOT EXISTS workspaces ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL UNIQUE,"
        "    path TEXT NOT NULL,"
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS targets ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    workspace_id INTEGER REFERENCES workspaces(id),"
        "    name TEXT NOT NULL,"
        "    type TEXT CHECK(type IN ('domain', 'ip', 'url', 'cidr')),"
        "    status TEXT DEFAULT 'active',"
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS assets ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    target_id INTEGER REFERENCES targets(id),"
        "    type TEXT CHECK(type IN ('subdomain', 'endpoint', 'technology', 'screenshot', 'certificate')),"
        "    value TEXT NOT NULL,"
        "    metadata TEXT,"  // JSON string
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS findings ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    target_id INTEGER REFERENCES targets(id),"
        "    title TEXT NOT NULL,"
        "    severity TEXT CHECK(severity IN ('critical', 'high', 'medium', 'low', 'info')),"
        "    cvss_score REAL,"
        "    description TEXT,"
        "    recommendation TEXT,"
        "    evidence TEXT,"
        "    status TEXT DEFAULT 'open',"
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS notes ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    target_id INTEGER REFERENCES targets(id),"
        "    title TEXT NOT NULL,"
        "    content TEXT,"
        "    tags TEXT,"  // JSON string
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS tool_results ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    target_id INTEGER REFERENCES targets(id),"
        "    tool_name TEXT NOT NULL,"
        "    command TEXT,"
        "    output TEXT,"
        "    parsed_data TEXT,"  // JSON string
        "    execution_time INTEGER,"
        "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
        
        "CREATE TABLE IF NOT EXISTS asset_relationships ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    source_id INTEGER REFERENCES assets(id),"
        "    target_id INTEGER REFERENCES assets(id),"
        "    relationship TEXT NOT NULL"
        ");";
    
    rc = sqlite3_exec(db, create_tables, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return KS_ERROR_DB;
    }
    
    return KS_OK;
}

void ks_db_close(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int ks_db_exec(const char *sql) {
    if (!db || !sql) {
        return KS_ERROR_DB;
    }
    
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return KS_ERROR_DB;
    }
    
    return KS_OK;
}

int ks_db_exec_with_result(const char *sql, int (*callback)(void*, int, char**, char**), void *arg) {
    if (!db || !sql) {
        return KS_ERROR_DB;
    }
    
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, callback, arg, &errmsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return KS_ERROR_DB;
    }
    
    return KS_OK;
}

sqlite3 *ks_db_get_handle(void) {
    return db;
}
