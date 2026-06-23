#include "gupt/tool_adapter.h"

// Adapter registry
static const ks_tool_adapter_t *adapters[64];
static int adapter_count = 0;

int ks_adapter_init(void) {
    adapter_count = 0;
    
    // Register built-in adapters
    ks_adapter_register(ks_adapter_subfinder());
    ks_adapter_register(ks_adapter_httpx());
    ks_adapter_register(ks_adapter_katana());
    ks_adapter_register(ks_adapter_nuclei());
    ks_adapter_register(ks_adapter_ffuf());
    ks_adapter_register(ks_adapter_nmap());
    ks_adapter_register(ks_adapter_sqlmap());
    
    return KS_OK;
}

void ks_adapter_cleanup(void) {
    adapter_count = 0;
}

int ks_adapter_register(const ks_tool_adapter_t *adapter) {
    if (!adapter || adapter_count >= 64) return KS_ERROR;
    adapters[adapter_count++] = adapter;
    return KS_OK;
}

const ks_tool_adapter_t *ks_adapter_get(const char *name) {
    for (int i = 0; i < adapter_count; i++) {
        if (strcmp(adapters[i]->name, name) == 0) {
            return adapters[i];
        }
    }
    return NULL;
}

int ks_adapter_run(const char *name, const char *target, const char *args, const char *output_file) {
    const ks_tool_adapter_t *adapter = ks_adapter_get(name);
    if (!adapter) {
        fprintf(stderr, "Unknown tool: %s\n", name);
        return KS_ERROR_NOTFOUND;
    }
    
    if (!adapter->is_installed()) {
        fprintf(stderr, "Tool not installed: %s\n", name);
        fprintf(stderr, "Install it and try again.\n");
        return KS_ERROR_TOOL;
    }
    
    return adapter->run(target, args, output_file);
}

bool ks_adapter_is_installed(const char *name) {
    const ks_tool_adapter_t *adapter = ks_adapter_get(name);
    if (!adapter) return false;
    return adapter->is_installed();
}

int ks_adapter_list(void) {
    printf("\n");
    printf("  Available Tool Adapters\n");
    printf("  ───────────────────────\n\n");
    
    for (int i = 0; i < adapter_count; i++) {
        const ks_tool_adapter_t *a = adapters[i];
        bool installed = a->is_installed();
        
        printf("  %s%-12s%s %s[%s]%s  %s\n",
            COLOR_BOLD, a->name, COLOR_RESET,
            COLOR_CYAN, a->category, COLOR_RESET,
            a->description);
        
        if (installed) {
            printf("               %s✓ installed%s\n", COLOR_GREEN, COLOR_RESET);
        } else {
            printf("               %s✗ not installed%s\n", COLOR_RED, COLOR_RESET);
        }
    }
    
    printf("\n");
    return KS_OK;
}

int ks_adapter_list_installed(void) {
    printf("\n");
    printf("  Installed Tools\n");
    printf("  ───────────────\n\n");
    
    int count = 0;
    for (int i = 0; i < adapter_count; i++) {
        const ks_tool_adapter_t *a = adapters[i];
        if (a->is_installed()) {
            printf("  %s%-12s%s %s\n",
                COLOR_GREEN, a->name, COLOR_RESET,
                a->description);
            count++;
        }
    }
    
    if (count == 0) {
        printf("  %sNo tools installed%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    
    printf("\n");
    return count;
}
