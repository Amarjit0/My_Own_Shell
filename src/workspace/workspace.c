#include "kalishell/kalishell.h"
#include <sqlite3.h>

extern ks_shell_t shell;

static char *get_workspaces_dir(void) {
    char *home = getenv("HOME");
    if (!home) {
        home = "/tmp";
    }
    return ks_path_join(home, ".kalishell/workspaces");
}

static int workspace_callback(void *data, int argc, char **argv, char **colnames) {
    int *count = (int *)data;
    
    if (*count == 0) {
        printf("  ID  Name                    Path\n");
        printf("  --- ----------------------  ----------------------------\n");
    }
    
    printf("  %-3s %-22s %s\n", 
           argv[0] ? argv[0] : "?",
           argv[1] ? argv[1] : "?",
           argv[2] ? argv[2] : "?");
    
    (*count)++;
    return 0;
}

int ks_workspace_create(const char *name) {
    if (!name) {
        fprintf(stderr, "Usage: workspace create <name>\n");
        return 1;
    }
    
    // Get workspaces directory
    char *workspaces_dir = get_workspaces_dir();
    if (!workspaces_dir) {
        fprintf(stderr, "Error: Cannot determine workspaces directory\n");
        return 1;
    }
    
    // Create workspaces directory if it doesn't exist
    if (!ks_path_exists(workspaces_dir)) {
        ks_path_mkdir(workspaces_dir, 0755);
    }
    
    // Create workspace path
    char *workspace_path = ks_path_join(workspaces_dir, name);
    if (!workspace_path) {
        free(workspaces_dir);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Check if workspace already exists
    if (ks_path_exists(workspace_path)) {
        fprintf(stderr, "Error: Workspace '%s' already exists\n", name);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Create workspace directory structure
    ks_path_mkdir(workspace_path, 0755);
    
    char *recon_dir = ks_path_join(workspace_path, "recon");
    char *screenshots_dir = ks_path_join(workspace_path, "screenshots");
    char *notes_dir = ks_path_join(workspace_path, "notes");
    char *findings_dir = ks_path_join(workspace_path, "findings");
    char *reports_dir = ks_path_join(workspace_path, "reports");
    
    ks_path_mkdir(recon_dir, 0755);
    ks_path_mkdir(screenshots_dir, 0755);
    ks_path_mkdir(notes_dir, 0755);
    ks_path_mkdir(findings_dir, 0755);
    ks_path_mkdir(reports_dir, 0755);
    
    free(recon_dir);
    free(screenshots_dir);
    free(notes_dir);
    free(findings_dir);
    free(reports_dir);
    
    // Create database
    char *db_path = ks_path_join(workspace_path, "database.db");
    if (ks_db_init(db_path) != KS_OK) {
        fprintf(stderr, "Error: Failed to initialize database\n");
        free(db_path);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Insert workspace into database
    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO workspaces (name, path) VALUES ('%s', '%s');",
        name, workspace_path);
    
    if (ks_db_exec(sql) != KS_OK) {
        fprintf(stderr, "Error: Failed to save workspace\n");
        ks_db_close();
        free(db_path);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Set current workspace
    shell.current_workspace = strdup(name);
    shell.workspace_dir = strdup(workspace_path);
    
    printf("[+] Workspace '%s' created\n", name);
    printf("    Path: %s\n", workspace_path);
    
    free(db_path);
    free(workspace_path);
    free(workspaces_dir);
    
    return 0;
}

int ks_workspace_open(const char *name) {
    if (!name) {
        fprintf(stderr, "Usage: workspace open <name>\n");
        return 1;
    }
    
    // Get workspaces directory
    char *workspaces_dir = get_workspaces_dir();
    if (!workspaces_dir) {
        fprintf(stderr, "Error: Cannot determine workspaces directory\n");
        return 1;
    }
    
    // Create workspace path
    char *workspace_path = ks_path_join(workspaces_dir, name);
    if (!workspace_path) {
        free(workspaces_dir);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Check if workspace exists
    if (!ks_path_exists(workspace_path)) {
        fprintf(stderr, "Error: Workspace '%s' not found\n", name);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Check for database
    char *db_path = ks_path_join(workspace_path, "database.db");
    if (!ks_path_exists(db_path)) {
        fprintf(stderr, "Error: Workspace '%s' is corrupted (missing database)\n", name);
        free(db_path);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Open database
    if (ks_db_init(db_path) != KS_OK) {
        fprintf(stderr, "Error: Failed to open workspace database\n");
        free(db_path);
        free(workspace_path);
        free(workspaces_dir);
        return 1;
    }
    
    // Update shell state
    if (shell.current_workspace) {
        free(shell.current_workspace);
    }
    if (shell.workspace_dir) {
        free(shell.workspace_dir);
    }
    
    shell.current_workspace = strdup(name);
    shell.workspace_dir = strdup(workspace_path);
    
    printf("[+] Workspace '%s' opened\n", name);
    
    free(db_path);
    free(workspace_path);
    free(workspaces_dir);
    
    return 0;
}

int ks_workspace_close(void) {
    if (!shell.current_workspace) {
        printf("No workspace is currently open\n");
        return 0;
    }
    
    printf("[+] Workspace '%s' closed\n", shell.current_workspace);
    
    // Free workspace state
    free(shell.current_workspace);
    shell.current_workspace = NULL;
    
    if (shell.workspace_dir) {
        free(shell.workspace_dir);
        shell.workspace_dir = NULL;
    }
    
    // Close database
    ks_db_close();
    
    return 0;
}

int ks_workspace_list(void) {
    // Get workspaces directory
    char *workspaces_dir = get_workspaces_dir();
    if (!workspaces_dir) {
        fprintf(stderr, "Error: Cannot determine workspaces directory\n");
        return 1;
    }
    
    // Check if workspaces directory exists
    if (!ks_path_exists(workspaces_dir)) {
        printf("No workspaces found\n");
        free(workspaces_dir);
        return 0;
    }
    
    // Open workspaces directory
    DIR *dir = opendir(workspaces_dir);
    if (!dir) {
        fprintf(stderr, "Error: Cannot open workspaces directory\n");
        free(workspaces_dir);
        return 1;
    }
    
    struct dirent *entry;
    int count = 0;
    
    printf("Available workspaces:\n\n");
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;  // Skip hidden directories
        }
        
        // Check if it's a directory
        char *entry_path = ks_path_join(workspaces_dir, entry->d_name);
        if (entry_path && ks_path_is_dir(entry_path)) {
            // Check for database
            char *db_path = ks_path_join(entry_path, "database.db");
            if (db_path && ks_path_exists(db_path)) {
                printf("  * %s\n", entry->d_name);
                count++;
            }
            free(db_path);
        }
        free(entry_path);
    }
    
    closedir(dir);
    free(workspaces_dir);
    
    if (count == 0) {
        printf("  No workspaces found\n");
    }
    
    printf("\nUse 'workspace open <name>' to open a workspace\n");
    
    return 0;
}
