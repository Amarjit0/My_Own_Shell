#include "gupt/tool_adapter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Katana adapter
static bool katana_is_installed(void) { return ks_platform_command_exists("katana"); }
static int katana_run(const char *target, const char *args, const char *output_file) {
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "katana -u %s -silent %s %s", 
        target, output_file ? "-o " : "", output_file ? output_file : "");
    return ks_platform_execute(cmd, NULL, 0);
}
static char *katana_parse_output(const char *raw) { 
    char *json = malloc(strlen(raw) + 256);
    strcpy(json, "[");
    char *line = strtok((char*)raw, "\n");
    bool first = true;
    while (line) {
        if (strlen(line) > 0) {
            if (!first) strcat(json, ",");
            strcat(json, "\""); strcat(json, line); strcat(json, "\"");
            first = false;
        }
        line = strtok(NULL, "\n");
    }
    strcat(json, "]");
    return json;
}
static const char *katana_get_help(void) { return "Usage: katana -u <url>\n"; }
static const ks_tool_adapter_t katana_adapter = {
    .name = "katana", .description = "Crawling and spidering framework", .category = "recon",
    .is_installed = katana_is_installed, .run = katana_run, 
    .parse_output = katana_parse_output, .get_help = katana_get_help,
};
const ks_tool_adapter_t *ks_adapter_katana(void) { return &katana_adapter; }

// Nuclei adapter
static bool nuclei_is_installed(void) { return ks_platform_command_exists("nuclei"); }
static int nuclei_run(const char *target, const char *args, const char *output_file) {
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "nuclei -u %s -silent %s %s",
        target, output_file ? "-o " : "", output_file ? output_file : "");
    return ks_platform_execute(cmd, NULL, 0);
}
static char *nuclei_parse_output(const char *raw) {
    char *json = malloc(strlen(raw) + 256);
    strcpy(json, "[");
    char *line = strtok((char*)raw, "\n");
    bool first = true;
    while (line) {
        if (strlen(line) > 0) {
            if (!first) strcat(json, ",");
            strcat(json, "\""); strcat(json, line); strcat(json, "\"");
            first = false;
        }
        line = strtok(NULL, "\n");
    }
    strcat(json, "]");
    return json;
}
static const char *nuclei_get_help(void) { return "Usage: nuclei -u <url>\n"; }
static const ks_tool_adapter_t nuclei_adapter = {
    .name = "nuclei", .description = "Vulnerability scanner", .category = "scanner",
    .is_installed = nuclei_is_installed, .run = nuclei_run,
    .parse_output = nuclei_parse_output, .get_help = nuclei_get_help,
};
const ks_tool_adapter_t *ks_adapter_nuclei(void) { return &nuclei_adapter; }

// Ffuf adapter
static bool ffuf_is_installed(void) { return ks_platform_command_exists("ffuf"); }
static int ffuf_run(const char *target, const char *args, const char *output_file) {
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "ffuf -u %s/FUZZ -w /usr/share/wordlists/dirb/common.txt -mc 200,301,302,403 %s",
        target, args ? args : "");
    return ks_platform_execute(cmd, NULL, 0);
}
static char *ffuf_parse_output(const char *raw) { return strdup("{}"); }
static const char *ffuf_get_help(void) { return "Usage: ffuf -u <url>/FUZZ -w <wordlist>\n"; }
static const ks_tool_adapter_t ffuf_adapter = {
    .name = "ffuf", .description = "Fuzzing tool", .category = "fuzzing",
    .is_installed = ffuf_is_installed, .run = ffuf_run,
    .parse_output = ffuf_parse_output, .get_help = ffuf_get_help,
};
const ks_tool_adapter_t *ks_adapter_ffuf(void) { return &ffuf_adapter; }

// Nmap adapter
static bool nmap_is_installed(void) { return ks_platform_command_exists("nmap"); }
static int nmap_run(const char *target, const char *args, const char *output_file) {
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "nmap %s %s", target, args ? args : "-sV -sC");
    return ks_platform_execute(cmd, NULL, 0);
}
static char *nmap_parse_output(const char *raw) { return strdup("{}"); }
static const char *nmap_get_help(void) { return "Usage: nmap <target> [options]\n"; }
static const ks_tool_adapter_t nmap_adapter = {
    .name = "nmap", .description = "Network scanner", .category = "scanner",
    .is_installed = nmap_is_installed, .run = nmap_run,
    .parse_output = nmap_parse_output, .get_help = nmap_get_help,
};
const ks_tool_adapter_t *ks_adapter_nmap(void) { return &nmap_adapter; }

// Sqlmap adapter
static bool sqlmap_is_installed(void) { return ks_platform_command_exists("sqlmap"); }
static int sqlmap_run(const char *target, const char *args, const char *output_file) {
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "sqlmap -u %s --batch %s", target, args ? args : "");
    return ks_platform_execute(cmd, NULL, 0);
}
static char *sqlmap_parse_output(const char *raw) { return strdup("{}"); }
static const char *sqlmap_get_help(void) { return "Usage: sqlmap -u <url> --batch\n"; }
static const ks_tool_adapter_t sqlmap_adapter = {
    .name = "sqlmap", .description = "SQL injection tool", .category = "scanner",
    .is_installed = sqlmap_is_installed, .run = sqlmap_run,
    .parse_output = sqlmap_parse_output, .get_help = sqlmap_get_help,
};
const ks_tool_adapter_t *ks_adapter_sqlmap(void) { return &sqlmap_adapter; }
