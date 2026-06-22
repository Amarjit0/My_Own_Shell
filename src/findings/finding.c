#include "gupt/gupt.h"

extern ks_shell_t shell;

// Finding structure
typedef struct {
    int64_t id;
    int64_t target_id;
    char *title;
    ks_severity_t severity;
    float cvss_score;
    char *description;
    char *recommendation;
    char *evidence;
    ks_finding_status_t status;
    char *cwe_id;
    char *owasp_category;
    time_t created_at;
} ks_finding_t;

// Severity names
static const char *severity_names[] = {
    "critical", "high", "medium", "low", "info"
};

// Create a new finding
ks_finding_t *ks_finding_create_new(const char *title, ks_severity_t severity,
                                     const char *description, const char *recommendation) {
    ks_finding_t *finding = calloc(1, sizeof(ks_finding_t));
    if (!finding) return NULL;
    
    finding->title = strdup(title);
    finding->severity = severity;
    finding->description = description ? strdup(description) : NULL;
    finding->recommendation = recommendation ? strdup(recommendation) : NULL;
    finding->status = FINDING_OPEN;
    finding->created_at = time(NULL);
    
    return finding;
}

// Free a finding
void ks_finding_free(ks_finding_t *finding) {
    if (!finding) return;
    
    free(finding->title);
    free(finding->description);
    free(finding->recommendation);
    free(finding->evidence);
    free(finding->cwe_id);
    free(finding->owasp_category);
    free(finding);
}

// Get severity name
const char *ks_finding_get_severity_name(ks_severity_t severity) {
    if (severity >= 0 && severity <= SEVERITY_INFO) {
        return severity_names[severity];
    }
    return "unknown";
}

// Get severity color
const char *ks_finding_get_severity_color(ks_severity_t severity) {
    switch (severity) {
        case SEVERITY_CRITICAL: return COLOR_RED;
        case SEVERITY_HIGH: return COLOR_RED;
        case SEVERITY_MEDIUM: return COLOR_YELLOW;
        case SEVERITY_LOW: return COLOR_BLUE;
        case SEVERITY_INFO: return COLOR_WHITE;
        default: return COLOR_RESET;
    }
}

// Create finding in database
int ks_finding_create(const char *title, ks_severity_t severity,
                      const char *description, const char *recommendation) {
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open\n");
        return KS_ERROR;
    }
    
    ks_finding_t *finding = ks_finding_create_new(title, severity, description, recommendation);
    if (!finding) return KS_ERROR_NOMEM;
    
    // TODO: Insert into database
    printf("[+] Finding created: %s [%s]\n", title, ks_finding_get_severity_name(severity));
    
    ks_finding_free(finding);
    return KS_OK;
}

// List findings
int ks_finding_list(int64_t target_id) {
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open\n");
        return KS_ERROR;
    }
    
    // TODO: Query database
    printf("Findings:\n\n");
    printf("  No findings yet\n");
    
    return KS_OK;
}

// Update finding
int ks_finding_update(int64_t id, const char *field, const char *value) {
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open\n");
        return KS_ERROR;
    }
    
    // TODO: Update in database
    printf("[+] Finding %lld updated: %s = %s\n", (long long)id, field, value);
    
    return KS_OK;
}

// Delete finding
int ks_finding_delete(int64_t id) {
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open\n");
        return KS_ERROR;
    }
    
    // TODO: Delete from database
    printf("[+] Finding %lld deleted\n", (long long)id);
    
    return KS_OK;
}

// Finding command handler
int ks_cmd_finding(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: finding <create|list|update|delete> [args]\n");
        return 1;
    }
    
    if (strcmp(argv[1], "create") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: finding create <title> <severity> [description]\n");
            return 1;
        }
        
        ks_severity_t severity = SEVERITY_MEDIUM;
        if (strcmp(argv[3], "critical") == 0) severity = SEVERITY_CRITICAL;
        else if (strcmp(argv[3], "high") == 0) severity = SEVERITY_HIGH;
        else if (strcmp(argv[3], "medium") == 0) severity = SEVERITY_MEDIUM;
        else if (strcmp(argv[3], "low") == 0) severity = SEVERITY_LOW;
        else if (strcmp(argv[3], "info") == 0) severity = SEVERITY_INFO;
        
        return ks_finding_create(argv[2], severity, argc > 4 ? argv[4] : NULL, NULL);
    }
    
    if (strcmp(argv[1], "list") == 0) {
        return ks_finding_list(0);
    }
    
    if (strcmp(argv[1], "update") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Usage: finding update <id> <field> <value>\n");
            return 1;
        }
        return ks_finding_update(atoll(argv[2]), argv[3], argv[4]);
    }
    
    if (strcmp(argv[1], "delete") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: finding delete <id>\n");
            return 1;
        }
        return ks_finding_delete(atoll(argv[2]));
    }
    
    fprintf(stderr, "Unknown finding command: %s\n", argv[1]);
    return 1;
}
