#include "gupt/gupt.h"

// Tool registry
static ks_tool_def_t *tools = NULL;
static int tool_count = 0;
static int tool_capacity = 0;

// Initialize tool registry
int ks_tool_init(void) {
    tool_capacity = 32;
    tools = calloc(tool_capacity, sizeof(ks_tool_def_t));
    if (!tools) {
        return KS_ERROR_NOMEM;
    }
    
    // Register common security tools
    ks_tool_register("subfinder", "2.6.7", "Fast passive subdomain enumeration", "subfinder");
    ks_tool_register("httpx", "1.6.10", "Fast HTTP toolkit", "httpx");
    ks_tool_register("katana", "1.1.2", "Crawling and spidering framework", "katana");
    ks_tool_register("nuclei", "3.3.7", "Vulnerability scanner", "nuclei");
    ks_tool_register("ffuf", "2.1.0", "Fuzzing tool", "ffuf");
    ks_tool_register("nmap", "7.94", "Network scanner", "nmap");
    ks_tool_register("sqlmap", "1.7.12", "SQL injection tool", "sqlmap");
    ks_tool_register("nikto", "2.5.0", "Web server scanner", "nikto");
    ks_tool_register("dirb", "2.22", "Web content scanner", "dirb");
    ks_tool_register("gobuster", "3.6", "Directory/file brute-forcer", "gobuster");
    ks_tool_register("whatweb", "0.5.5", "Web technology identification", "whatweb");
    ks_tool_register("dalfox", "2.8.3", "XSS scanner", "dalfox");
    ks_tool_register("arjun", "1.7.5", "HTTP parameter discovery", "arjun");
    ks_tool_register("sublist3r", "1.0", "Subdomain enumeration", "sublist3r");
    
    return KS_OK;
}

// Cleanup tool registry
void ks_tool_cleanup(void) {
    for (int i = 0; i < tool_count; i++) {
        free(tools[i].name);
        free(tools[i].version);
        free(tools[i].description);
        free(tools[i].command);
        free(tools[i].args_template);
        free(tools[i].output_format);
        free(tools[i].category);
    }
    
    free(tools);
    tools = NULL;
    tool_count = 0;
    tool_capacity = 0;
}

// Register a tool programmatically
int ks_tool_register(const char *name, const char *version, 
                     const char *description, const char *command) {
    if (tool_count >= tool_capacity) {
        tool_capacity *= 2;
        tools = realloc(tools, sizeof(ks_tool_def_t) * tool_capacity);
    }
    
    ks_tool_def_t *tool = &tools[tool_count++];
    tool->name = strdup(name);
    tool->version = version ? strdup(version) : NULL;
    tool->description = description ? strdup(description) : NULL;
    tool->command = strdup(command);
    tool->args_template = NULL;
    tool->output_format = NULL;
    tool->category = NULL;
    tool->installed = ks_platform_command_exists(command);
    
    return KS_OK;
}

// Get tool by name
ks_tool_def_t *ks_tool_get(const char *name) {
    for (int i = 0; i < tool_count; i++) {
        if (tools[i].name && strcmp(tools[i].name, name) == 0) {
            return &tools[i];
        }
    }
    return NULL;
}

// Check if tool is installed
bool ks_tool_is_installed(const char *name) {
    ks_tool_def_t *tool = ks_tool_get(name);
    return tool ? tool->installed : false;
}

// List all tools
int ks_tool_list(void) {
    printf("Available tools:\n\n");
    printf("  %-20s %-10s %-10s %s\n", "Name", "Version", "Status", "Description");
    printf("  %-20s %-10s %-10s %s\n", "----", "-------", "------", "-----------");
    
    for (int i = 0; i < tool_count; i++) {
        printf("  %-20s %-10s %-10s %s\n",
               tools[i].name ? tools[i].name : "?",
               tools[i].version ? tools[i].version : "?",
               tools[i].installed ? "INSTALLED" : "missing",
               tools[i].description ? tools[i].description : "");
    }
    
    printf("\nTotal: %d tools\n", tool_count);
    return KS_OK;
}

// Execute a tool with arguments
int ks_tool_execute(const char *name, const char *args, char *output, size_t output_size) {
    ks_tool_def_t *tool = ks_tool_get(name);
    if (!tool) {
        return KS_ERROR_NOTFOUND;
    }
    
    if (!tool->installed) {
        fprintf(stderr, "Tool '%s' is not installed\n", name);
        return KS_ERROR_TOOL;
    }
    
    // Build command
    char command[4096];
    if (args) {
        snprintf(command, sizeof(command), "%s %s", tool->command, args);
    } else {
        snprintf(command, sizeof(command), "%s", tool->command);
    }
    
    KS_LOG_DEBUG("Executing tool: %s", command);
    
    return ks_platform_execute(command, output, output_size);
}

// Execute a tool and save output to file
int ks_tool_execute_to_file(const char *name, const char *args, const char *output_file) {
    ks_tool_def_t *tool = ks_tool_get(name);
    if (!tool) {
        return KS_ERROR_NOTFOUND;
    }
    
    if (!tool->installed) {
        fprintf(stderr, "Tool '%s' is not installed\n", name);
        return KS_ERROR_TOOL;
    }
    
    // Build command with output redirect
    char command[4096];
    if (args) {
        snprintf(command, sizeof(command), "%s %s > %s", tool->command, args, output_file);
    } else {
        snprintf(command, sizeof(command), "%s > %s", tool->command, output_file);
    }
    
    KS_LOG_DEBUG("Executing tool: %s", command);
    
    return system(command);
}

// Execute a tool with template substitution
int ks_tool_execute_with_template(const char *name, const char *target, 
                                   const char *input_file, const char *output_file) {
    ks_tool_def_t *tool = ks_tool_get(name);
    if (!tool) {
        return KS_ERROR_NOTFOUND;
    }
    
    if (!tool->installed) {
        fprintf(stderr, "Tool '%s' is not installed\n", name);
        return KS_ERROR_TOOL;
    }
    
    // Build args from template
    char args[4096] = {0};
    if (tool->args_template) {
        strncpy(args, tool->args_template, sizeof(args) - 1);
        
        // Replace placeholders
        char *pos;
        if ((pos = strstr(args, "{target}")) != NULL) {
            *pos = '\0';
            char temp[4096];
            snprintf(temp, sizeof(temp), "%s%s%s", args, target ? target : "", pos + 8);
            strncpy(args, temp, sizeof(args) - 1);
        }
        
        if ((pos = strstr(args, "{input}")) != NULL) {
            *pos = '\0';
            char temp[4096];
            snprintf(temp, sizeof(temp), "%s%s%s", args, input_file ? input_file : "", pos + 7);
            strncpy(args, temp, sizeof(args) - 1);
        }
        
        if ((pos = strstr(args, "{output}")) != NULL) {
            *pos = '\0';
            char temp[4096];
            snprintf(temp, sizeof(temp), "%s%s%s", args, output_file ? output_file : "", pos + 8);
            strncpy(args, temp, sizeof(args) - 1);
        }
    }
    
    // Build command
    char command[4096];
    snprintf(command, sizeof(command), "%s %s", tool->command, args);
    
    KS_LOG_DEBUG("Executing tool with template: %s", command);
    
    return system(command);
}
