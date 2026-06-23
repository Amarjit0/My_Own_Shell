#ifndef GUPT_TOOL_ADAPTER_H
#define GUPT_TOOL_ADAPTER_H

#include "gupt.h"

// Tool adapter interface
// Each external tool gets an adapter that knows how to run it
typedef struct ks_tool_adapter {
    const char *name;           // "subfinder", "httpx", etc.
    const char *description;    // What it does
    const char *category;       // "recon", "fuzzing", "scanner", etc.
    
    // Check if tool is installed on system
    bool (*is_installed)(void);
    
    // Run the tool with args, write output to stdout or file
    // Returns exit code
    int (*run)(const char *target, const char *args, const char *output_file);
    
    // Parse tool output into standardized format
    // Returns JSON string (caller must free)
    char *(*parse_output)(const char *raw_output);
    
    // Get help/usage string
    const char *(*get_help)(void);
    
} ks_tool_adapter_t;

// Tool registry
int ks_adapter_init(void);
void ks_adapter_cleanup(void);
int ks_adapter_register(const ks_tool_adapter_t *adapter);
const ks_tool_adapter_t *ks_adapter_get(const char *name);
int ks_adapter_run(const char *name, const char *target, const char *args, const char *output_file);
bool ks_adapter_is_installed(const char *name);
int ks_adapter_list(void);
int ks_adapter_list_installed(void);

// Built-in adapters (defined in adapters/)
const ks_tool_adapter_t *ks_adapter_subfinder(void);
const ks_tool_adapter_t *ks_adapter_httpx(void);
const ks_tool_adapter_t *ks_adapter_katana(void);
const ks_tool_adapter_t *ks_adapter_nuclei(void);
const ks_tool_adapter_t *ks_adapter_ffuf(void);
const ks_tool_adapter_t *ks_adapter_nmap(void);
const ks_tool_adapter_t *ks_adapter_sqlmap(void);

#endif // GUPT_TOOL_ADAPTER_H
