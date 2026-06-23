#include "gupt/tool_adapter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool httpx_is_installed(void) {
    return ks_platform_command_exists("httpx");
}

static int httpx_run(const char *target, const char *args, const char *output_file) {
    char command[4096];
    
    if (output_file) {
        snprintf(command, sizeof(command),
            "httpx -u %s -silent -o %s %s",
            target, output_file, args ? args : "");
    } else {
        snprintf(command, sizeof(command),
            "httpx -u %s -silent %s",
            target, args ? args : "");
    }
    
    return ks_platform_execute(command, NULL, 0);
}

static char *httpx_parse_output(const char *raw_output) {
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

static const char *httpx_get_help(void) {
    return "Usage: httpx [options]\n"
           "  -u    Target URL\n"
           "  -silent  Output only results\n";
}

static const ks_tool_adapter_t httpx_adapter = {
    .name = "httpx",
    .description = "Fast HTTP toolkit",
    .category = "recon",
    .is_installed = httpx_is_installed,
    .run = httpx_run,
    .parse_output = httpx_parse_output,
    .get_help = httpx_get_help,
};

const ks_tool_adapter_t *ks_adapter_httpx(void) {
    return &httpx_adapter;
}
