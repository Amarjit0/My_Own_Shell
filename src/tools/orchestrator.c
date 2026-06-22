#include "kalishell/kalishell.h"

extern ks_shell_t shell;

// Workflow step structure
typedef struct ks_workflow_step {
    char *tool_name;
    char *args_template;
    char *input_source;
    char *output_dest;
    struct ks_workflow_step *next;
} ks_workflow_step_t;

// Workflow structure
typedef struct {
    char *name;
    char *description;
    ks_workflow_step_t *steps;
    int step_count;
} ks_workflow_t;

// Built-in workflows
static ks_workflow_t workflows[] = {
    {
        .name = "recon",
        .description = "Basic reconnaissance pipeline",
        .steps = NULL,
        .step_count = 5
    },
    {
        .name = "web-basic",
        .description = "Basic web application testing",
        .steps = NULL,
        .step_count = 4
    },
    {
        .name = "api-audit",
        .description = "API security audit",
        .steps = NULL,
        .step_count = 6
    }
};

// Initialize orchestrator
int ks_orchestrator_init(void) {
    // Initialize built-in workflows
    return KS_OK;
}

// Run a tool with proper output handling
int ks_orchestrator_run_tool(const char *tool_name, const char *args) {
    if (!tool_name) {
        return KS_ERROR_INVALID;
    }
    
    ks_tool_def_t *tool = ks_tool_get(tool_name);
    if (!tool) {
        fprintf(stderr, "Unknown tool: %s\n", tool_name);
        return KS_ERROR_NOTFOUND;
    }
    
    if (!tool->installed) {
        fprintf(stderr, "Tool not installed: %s\n", tool_name);
        fprintf(stderr, "Install with: sudo apt install %s\n", tool_name);
        return KS_ERROR_TOOL;
    }
    
    // Build command
    char command[4096];
    if (args) {
        snprintf(command, sizeof(command), "%s %s", tool->command, args);
    } else {
        snprintf(command, sizeof(command), "%s", tool->command);
    }
    
    printf("[*] Running: %s\n", command);
    
    // Execute and capture output
    char output[65536] = {0};
    int result = ks_platform_execute(command, output, sizeof(output));
    
    // Print output
    if (strlen(output) > 0) {
        printf("%s\n", output);
    }
    
    // Save to workspace if open
    if (shell.current_workspace && shell.workspace_dir) {
        char *output_file = ks_path_join(shell.workspace_dir, "recon");
        if (output_file) {
            char *tool_output = ks_path_join(output_file, tool_name);
            if (tool_output) {
                ks_file_write(tool_output, output);
                free(tool_output);
            }
            free(output_file);
        }
    }
    
    return result;
}

// Run a pipeline (chain of tools)
int ks_orchestrator_run_pipeline(const char *pipeline_name, const char *target) {
    if (!pipeline_name || !target) {
        return KS_ERROR_INVALID;
    }
    
    printf("[*] Starting pipeline: %s\n", pipeline_name);
    printf("[*] Target: %s\n\n", target);
    
    // Check if we're in a workspace
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open. Use 'workspace create <name>' first.\n");
        return KS_ERROR;
    }
    
    char *workspace_dir = shell.workspace_dir;
    char *recon_dir = ks_path_join(workspace_dir, "recon");
    
    if (!ks_path_exists(recon_dir)) {
        ks_path_mkdir(recon_dir, 0755);
    }
    
    // Pipeline: subfinder -> httpx -> katana -> nuclei
    if (strcmp(pipeline_name, "recon") == 0) {
        // Step 1: Subdomain discovery
        printf("[1/4] Subdomain Discovery (subfinder)\n");
        char *subdomains_file = ks_path_join(recon_dir, "subdomains.txt");
        char args[1024];
        snprintf(args, sizeof(args), "-d %s -o %s", target, subdomains_file);
        ks_orchestrator_run_tool("subfinder", args);
        printf("\n");
        
        // Step 2: HTTP probing
        printf("[2/4] HTTP Probing (httpx)\n");
        char *live_hosts_file = ks_path_join(recon_dir, "live_hosts.txt");
        snprintf(args, sizeof(args), "-l %s -o %s -sc -title -tech-detect", subdomains_file, live_hosts_file);
        ks_orchestrator_run_tool("httpx", args);
        printf("\n");
        
        // Step 3: Endpoint discovery
        printf("[3/4] Endpoint Discovery (katana)\n");
        char *endpoints_file = ks_path_join(recon_dir, "endpoints.txt");
        snprintf(args, sizeof(args), "-u %s -o %s -d 3", target, endpoints_file);
        ks_orchestrator_run_tool("katana", args);
        printf("\n");
        
        // Step 4: Vulnerability scanning
        printf("[4/4] Vulnerability Scanning (nuclei)\n");
        char *findings_file = ks_path_join(recon_dir, "findings.json");
        snprintf(args, sizeof(args), "-l %s -o %s -severity critical,high,medium", live_hosts_file, findings_file);
        ks_orchestrator_run_tool("nuclei", args);
        printf("\n");
        
        // Cleanup
        free(subdomains_file);
        free(live_hosts_file);
        free(endpoints_file);
        free(findings_file);
        
        printf("[+] Pipeline complete!\n");
    }
    // Pipeline: web-basic
    else if (strcmp(pipeline_name, "web-basic") == 0) {
        printf("[1/3] Directory Discovery (ffuf)\n");
        char *dirs_file = ks_path_join(recon_dir, "directories.txt");
        char args[1024];
        snprintf(args, sizeof(args), "-u %s/FUZZ -w /usr/share/wordlists/dirb/common.txt -o %s", target, dirs_file);
        ks_orchestrator_run_tool("ffuf", args);
        printf("\n");
        
        printf("[2/3] Technology Detection (whatweb)\n");
        ks_orchestrator_run_tool("whatweb", target);
        printf("\n");
        
        printf("[3/3] Port Scanning (nmap)\n");
        char *ports_file = ks_path_join(recon_dir, "ports.txt");
        snprintf(args, sizeof(args), "-sV -oN %s %s", ports_file, target);
        ks_orchestrator_run_tool("nmap", args);
        printf("\n");
        
        free(dirs_file);
        free(ports_file);
        
        printf("[+] Web basic scan complete!\n");
    }
    // Pipeline: api-audit
    else if (strcmp(pipeline_name, "api-audit") == 0) {
        printf("[1/4] API Discovery (katana)\n");
        char *api_endpoints = ks_path_join(recon_dir, "api_endpoints.txt");
        char args[1024];
        snprintf(args, sizeof(args), "-u %s -o %s -d 5 -jc", target, api_endpoints);
        ks_orchestrator_run_tool("katana", args);
        printf("\n");
        
        printf("[2/4] Parameter Discovery (arjun)\n");
        char *params_file = ks_path_join(recon_dir, "parameters.txt");
        snprintf(args, sizeof(args), "-u %s -o %s", target, params_file);
        ks_orchestrator_run_tool("arjun", args);
        printf("\n");
        
        printf("[3/4] XSS Testing (dalfox)\n");
        snprintf(args, sizeof(args), "-u %s", target);
        ks_orchestrator_run_tool("dalfox", args);
        printf("\n");
        
        printf("[4/4] SQL Injection (sqlmap)\n");
        snprintf(args, sizeof(args), "-u %s --batch --random-agent", target);
        ks_orchestrator_run_tool("sqlmap", args);
        printf("\n");
        
        free(api_endpoints);
        free(params_file);
        
        printf("[+] API audit complete!\n");
    }
    else {
        fprintf(stderr, "Unknown pipeline: %s\n", pipeline_name);
        free(recon_dir);
        return KS_ERROR_NOTFOUND;
    }
    
    free(recon_dir);
    return KS_OK;
}

// Run a custom workflow
int ks_orchestrator_run_workflow(const char *workflow_name, const char *target) {
    // TODO: Load and execute custom workflows
    printf("[*] Custom workflows not yet implemented\n");
    return KS_OK;
}

// List available workflows
int ks_orchestrator_list_workflows(void) {
    printf("Available pipelines:\n\n");
    printf("  %-15s %s\n", "Name", "Description");
    printf("  %-15s %s\n", "----", "-----------");
    
    printf("  %-15s %s\n", "recon", "Basic reconnaissance pipeline");
    printf("  %-15s %s\n", "web-basic", "Basic web application testing");
    printf("  %-15s %s\n", "api-audit", "API security audit");
    
    printf("\nUsage: pipeline run <name> <target>\n");
    return KS_OK;
}

// Parse and execute a pipeline command
int ks_orchestrator_parse_pipeline(const char *input) {
    if (!input) {
        return KS_ERROR_INVALID;
    }
    
    // Parse: pipeline run <name> <target>
    char *args[MAX_ARGS];
    int argc = 0;
    
    if (ks_parse_command((char *)input, args, &argc) != KS_OK) {
        return KS_ERROR_INVALID;
    }
    
    if (argc < 4) {
        fprintf(stderr, "Usage: pipeline run <name> <target>\n");
        return KS_ERROR_INVALID;
    }
    
    if (strcmp(args[1], "run") == 0) {
        return ks_orchestrator_run_pipeline(args[2], args[3]);
    } else if (strcmp(args[1], "list") == 0) {
        return ks_orchestrator_list_workflows();
    } else {
        fprintf(stderr, "Unknown pipeline command: %s\n", args[1]);
        return KS_ERROR_INVALID;
    }
}
