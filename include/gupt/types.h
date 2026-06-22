#ifndef GUPT_TYPES_H
#define GUPT_TYPES_H

#include <stdint.h>
#include <time.h>

// Asset types
typedef enum {
    ASSET_DOMAIN,
    ASSET_IP,
    ASSET_URL,
    ASSET_CIDR,
    ASSET_SUBDOMAIN,
    ASSET_ENDPOINT,
    ASSET_TECHNOLOGY,
    ASSET_SCREENSHOT,
    ASSET_CERTIFICATE,
} ks_asset_type_t;

// Severity levels
typedef enum {
    SEVERITY_CRITICAL,
    SEVERITY_HIGH,
    SEVERITY_MEDIUM,
    SEVERITY_LOW,
    SEVERITY_INFO,
} ks_severity_t;

// Target status
typedef enum {
    TARGET_ACTIVE,
    TARGET_COMPLETED,
    TARGET_ARCHIVED,
} ks_target_status_t;

// Finding status
typedef enum {
    FINDING_OPEN,
    FINDING_CONFIRMED,
    FINDING_FALSE_POSITIVE,
    FINDING_FIXED,
    FINDING_WONT_FIX,
} ks_finding_status_t;

// Workspace structure
typedef struct {
    int64_t id;
    char *name;
    char *path;
    time_t created_at;
    time_t updated_at;
} ks_workspace_t;

// Target structure
typedef struct {
    int64_t id;
    int64_t workspace_id;
    char *name;
    ks_asset_type_t type;
    ks_target_status_t status;
    time_t created_at;
} ks_target_t;

// Asset structure
typedef struct {
    int64_t id;
    int64_t target_id;
    ks_asset_type_t type;
    char *value;
    char *metadata;  // JSON string
    time_t created_at;
} ks_asset_t;

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
    time_t created_at;
} ks_finding_t;

// Note structure
typedef struct {
    int64_t id;
    int64_t target_id;
    char *title;
    char *content;
    char *tags;  // JSON string
    time_t created_at;
} ks_note_t;

// Tool result structure
typedef struct {
    int64_t id;
    int64_t target_id;
    char *tool_name;
    char *command;
    char *output;
    char *parsed_data;  // JSON string
    int execution_time;
    time_t created_at;
} ks_tool_result_t;

// Asset relationship (for graph)
typedef struct {
    int64_t id;
    int64_t source_id;
    int64_t target_id;
    char *relationship;
} ks_asset_relationship_t;

// Plugin structure
typedef struct {
    char *name;
    char *version;
    char *description;
    char *command;
    void *handle;  // DLL/dlopen handle
} ks_plugin_t;

// Pipeline step
typedef struct {
    char *tool_name;
    char *command_template;
    struct ks_pipeline_step *next;
} ks_pipeline_step_t;

// Pipeline structure
typedef struct {
    char *name;
    char *description;
    ks_pipeline_step_t *steps;
} ks_pipeline_t;

#endif // GUPT_TYPES_H
