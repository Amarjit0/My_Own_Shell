#include "gupt/tool_adapter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool subfinder_is_installed(void) {
    return ks_platform_command_exists("subfinder");
}

static int subfinder_run(const char *target, const char *args, const char *output_file) {
    char command[4096];
    
    if (output_file) {
        snprintf(command, sizeof(command), 
            "subfinder -d %s -silent -o %s %s",
            target, output_file, args ? args : "");
    } else {
        snprintf(command, sizeof(command), 
            "subfinder -d %s -silent %s",
            target, args ? args : "");
    }
    
    return ks_platform_execute(command, NULL, 0);
}

static char *subfinder_parse_output(const char *raw_output) {
    // subfinder outputs one subdomain per line
    // Convert to JSON array
    char *json = malloc(strlen(raw_output) + 256);
    if (!json) return NULL;
    
    strcpy(json, "[");
    
    char *line = strtok((char *)raw_output, "\n");
    bool first = true;
    while (line) {
        if (strlen(line) > 0) {
            if (!first) strcat(json, ",");
            strcat(json, "\"");
            strcat(json, line);
            strcat(json, "\"");
            first = false;
        }
        line = strtok(NULL, "\n");
    }
    
    strcat(json, "]");
    return json;
}

static const char *subfinder_get_help(void) {
    return "Usage: subfinder -d <domain> [options]\n"
           "  -d    Domain to enumerate\n"
           "  -silent  Output only results\n"
           "  -o    Output file\n";
}

static const ks_tool_adapter_t subfinder_adapter = {
    .name = "subfinder",
    .description = "Fast passive subdomain enumeration",
    .category = "recon",
    .is_installed = subfinder_is_installed,
    .run = subfinder_run,
    .parse_output = subfinder_parse_output,
    .get_help = subfinder_get_help,
};

const ks_tool_adapter_t *ks_adapter_subfinder(void) {
    return &subfinder_adapter;
}
