#include "kalishell/kalishell.h"

typedef struct {
    char *name;
    char *description;
    char *severity;
    char *template_text;
} ks_finding_template_t;

static ks_finding_template_t templates[] = {
    {"xss", "Cross-Site Scripting", "medium", "Stored XSS vulnerability found"},
    {"sqli", "SQL Injection", "high", "SQL injection vulnerability found"},
    {"idor", "Insecure Direct Object Reference", "high", "IDOR vulnerability found"},
    {"ssrf", "Server-Side Request Forgery", "high", "SSRF vulnerability found"},
    {"xxe", "XML External Entity", "high", "XXE vulnerability found"},
    {NULL, NULL, NULL, NULL}
};

const char *ks_template_get(const char *name) {
    for (int i = 0; templates[i].name != NULL; i++) {
        if (strcmp(templates[i].name, name) == 0) {
            return templates[i].template_text;
        }
    }
    return NULL;
}

int ks_template_list(void) {
    printf("Available templates:\n");
    for (int i = 0; templates[i].name != NULL; i++) {
        printf("  %s - %s\n", templates[i].name, templates[i].description);
    }
    return KS_OK;
}
