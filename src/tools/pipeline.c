#include "gupt/gupt.h"

extern ks_shell_t shell;

// Pipeline step structure
typedef struct ks_pipeline_step {
    char *tool_name;
    char *args_template;
    char *input_from;
    char *output_to;
    struct ks_pipeline_step *next;
} ks_pipeline_step_t;

// Pipeline structure
typedef struct ks_pipeline {
    char *name;
    char *description;
    ks_pipeline_step_t *head;
    ks_pipeline_step_t *tail;
    int step_count;
    struct ks_pipeline *next;
} ks_pipeline_t;

// Pipeline registry
static ks_pipeline_t *pipelines = NULL;

// Create a new pipeline
ks_pipeline_t *ks_pipeline_create_new(const char *name, const char *description) {
    ks_pipeline_t *pipeline = calloc(1, sizeof(ks_pipeline_t));
    if (!pipeline) return NULL;
    
    pipeline->name = strdup(name);
    pipeline->description = description ? strdup(description) : NULL;
    pipeline->head = NULL;
    pipeline->tail = NULL;
    pipeline->step_count = 0;
    pipeline->next = NULL;
    
    return pipeline;
}

// Add a step to a pipeline
int ks_pipeline_add_step_to(ks_pipeline_t *pipeline, const char *tool_name, 
                             const char *args_template, const char *input_from,
                             const char *output_to) {
    if (!pipeline || !tool_name) {
        return KS_ERROR_INVALID;
    }
    
    ks_pipeline_step_t *step = calloc(1, sizeof(ks_pipeline_step_t));
    if (!step) return KS_ERROR_NOMEM;
    
    step->tool_name = strdup(tool_name);
    step->args_template = args_template ? strdup(args_template) : NULL;
    step->input_from = input_from ? strdup(input_from) : NULL;
    step->output_to = output_to ? strdup(output_to) : NULL;
    step->next = NULL;
    
    if (pipeline->tail) {
        pipeline->tail->next = step;
    } else {
        pipeline->head = step;
    }
    pipeline->tail = step;
    pipeline->step_count++;
    
    return KS_OK;
}

// Free a pipeline
void ks_pipeline_free(ks_pipeline_t *pipeline) {
    if (!pipeline) return;
    
    ks_pipeline_step_t *step = pipeline->head;
    while (step) {
        ks_pipeline_step_t *next = step->next;
        free(step->tool_name);
        free(step->args_template);
        free(step->input_from);
        free(step->output_to);
        free(step);
        step = next;
    }
    
    free(pipeline->name);
    free(pipeline->description);
    free(pipeline);
}

// Register a pipeline
int ks_pipeline_register(ks_pipeline_t *pipeline) {
    if (!pipeline) return KS_ERROR_INVALID;
    
    pipeline->next = pipelines;
    pipelines = pipeline;
    
    return KS_OK;
}

// Find a pipeline by name
ks_pipeline_t *ks_pipeline_find(const char *name) {
    ks_pipeline_t *current = pipelines;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Execute a pipeline
int ks_pipeline_execute(ks_pipeline_t *pipeline, const char *target) {
    if (!pipeline || !target) {
        return KS_ERROR_INVALID;
    }
    
    printf("[*] Executing pipeline: %s\n", pipeline->name);
    printf("[*] Target: %s\n\n", target);
    
    char *workspace_dir = shell.workspace_dir;
    char *temp_dir = ks_path_join(workspace_dir, ".temp");
    ks_path_mkdir(temp_dir, 0755);
    
    int step_num = 1;
    ks_pipeline_step_t *step = pipeline->head;
    
    while (step) {
        printf("[%d/%d] Running: %s\n", step_num, pipeline->step_count, step->tool_name);
        
        // Build arguments from template
        char args[4096] = {0};
        if (step->args_template) {
            strncpy(args, step->args_template, sizeof(args) - 1);
            
            // Replace {target}
            char *pos;
            if ((pos = strstr(args, "{target}")) != NULL) {
                *pos = '\0';
                char temp[4096];
                snprintf(temp, sizeof(temp), "%s%s%s", args, target, pos + 8);
                strncpy(args, temp, sizeof(args) - 1);
            }
            
            // Replace {input}
            if ((pos = strstr(args, "{input}")) != NULL) {
                *pos = '\0';
                char input_path[4096];
                if (step->input_from) {
                    snprintf(input_path, sizeof(input_path), "%s/%s", temp_dir, step->input_from);
                } else {
                    snprintf(input_path, sizeof(input_path), "%s/subdomains.txt", temp_dir);
                }
                char temp[4096];
                snprintf(temp, sizeof(temp), "%s%s%s", args, input_path, pos + 7);
                strncpy(args, temp, sizeof(args) - 1);
            }
            
            // Replace {output}
            if ((pos = strstr(args, "{output}")) != NULL) {
                *pos = '\0';
                char output_path[4096];
                if (step->output_to) {
                    snprintf(output_path, sizeof(output_path), "%s/%s", temp_dir, step->output_to);
                } else {
                    snprintf(output_path, sizeof(output_path), "%s/%s_output.txt", temp_dir, step->tool_name);
                }
                char temp[4096];
                snprintf(temp, sizeof(temp), "%s%s%s", args, output_path, pos + 8);
                strncpy(args, temp, sizeof(args) - 1);
            }
        }
        
        // Execute tool
        ks_orchestrator_run_tool(step->tool_name, args);
        printf("\n");
        
        step_num++;
        step = step->next;
    }
    
    // Copy results to recon directory
    char *recon_dir = ks_path_join(workspace_dir, "recon");
    if (!ks_path_exists(recon_dir)) {
        ks_path_mkdir(recon_dir, 0755);
    }
    
    // Move output files
    step = pipeline->head;
    while (step) {
        if (step->output_to) {
            char *src = ks_path_join(temp_dir, step->output_to);
            char *dst = ks_path_join(recon_dir, step->output_to);
            if (src && dst && ks_path_exists(src)) {
                // Read and write (simple copy)
                char *content = ks_file_read(src);
                if (content) {
                    ks_file_write(dst, content);
                    free(content);
                }
            }
            free(src);
            free(dst);
        }
        step = step->next;
    }
    
    // Cleanup temp directory
    // TODO: Implement recursive directory removal
    
    free(temp_dir);
    free(recon_dir);
    
    printf("[+] Pipeline '%s' complete!\n", pipeline->name);
    return KS_OK;
}

// Initialize built-in pipelines
int ks_pipeline_init(void) {
    // Recon pipeline
    ks_pipeline_t *recon = ks_pipeline_create_new("recon", "Basic reconnaissance pipeline");
    ks_pipeline_add_step_to(recon, "subfinder", "-d {target} -o {output}", NULL, "subdomains.txt");
    ks_pipeline_add_step_to(recon, "httpx", "-l {input} -o {output} -sc -title", "subdomains.txt", "live_hosts.txt");
    ks_pipeline_add_step_to(recon, "katana", "-u {target} -o {output} -d 3", NULL, "endpoints.txt");
    ks_pipeline_add_step_to(recon, "nuclei", "-l {input} -o {output} -severity critical,high,medium", "live_hosts.txt", "findings.json");
    ks_pipeline_register(recon);
    
    // Web basic pipeline
    ks_pipeline_t *web_basic = ks_pipeline_create_new("web-basic", "Basic web application testing");
    ks_pipeline_add_step_to(web_basic, "ffuf", "-u {target}/FUZZ -w /usr/share/wordlists/dirb/common.txt -o {output}", NULL, "directories.txt");
    ks_pipeline_add_step_to(web_basic, "whatweb", "{target}", NULL, NULL);
    ks_pipeline_add_step_to(web_basic, "nmap", "-sV -oN {output} {target}", NULL, "ports.txt");
    ks_pipeline_register(web_basic);
    
    // API audit pipeline
    ks_pipeline_t *api_audit = ks_pipeline_create_new("api-audit", "API security audit");
    ks_pipeline_add_step_to(api_audit, "katana", "-u {target} -o {output} -d 5 -jc", NULL, "api_endpoints.txt");
    ks_pipeline_add_step_to(api_audit, "arjun", "-u {target} -o {output}", NULL, "parameters.txt");
    ks_pipeline_add_step_to(api_audit, "dalfox", "-u {target}", NULL, NULL);
    ks_pipeline_add_step_to(api_audit, "sqlmap", "-u {target} --batch --random-agent", NULL, NULL);
    ks_pipeline_register(api_audit);
    
    return KS_OK;
}

// Cleanup pipelines
void ks_pipeline_cleanup(void) {
    ks_pipeline_t *current = pipelines;
    while (current) {
        ks_pipeline_t *next = current->next;
        ks_pipeline_free(current);
        current = next;
    }
    pipelines = NULL;
}

// List all pipelines
int ks_pipeline_list_all(void) {
    printf("Available pipelines:\n\n");
    printf("  %-15s %-40s %s\n", "Name", "Description", "Steps");
    printf("  %-15s %-40s %s\n", "----", "-----------", "-----");
    
    ks_pipeline_t *current = pipelines;
    while (current) {
        printf("  %-15s %-40s %d\n",
               current->name,
               current->description ? current->description : "",
               current->step_count);
        current = current->next;
    }
    
    printf("\nUsage: pipeline run <name> <target>\n");
    return KS_OK;
}

// Pipeline command handler
int ks_cmd_pipeline(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: pipeline <run|list> [name] [target]\n");
        return 1;
    }
    
    if (strcmp(argv[1], "list") == 0) {
        return ks_pipeline_list_all();
    }
    
    if (strcmp(argv[1], "run") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: pipeline run <name> <target>\n");
            return 1;
        }
        
        ks_pipeline_t *pipeline = ks_pipeline_find(argv[2]);
        if (!pipeline) {
            fprintf(stderr, "Unknown pipeline: %s\n", argv[2]);
            return 1;
        }
        
        return ks_pipeline_execute(pipeline, argv[3]);
    }
    
    fprintf(stderr, "Unknown pipeline command: %s\n", argv[1]);
    return 1;
}
