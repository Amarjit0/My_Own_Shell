#ifndef GUPT_ERRORS_H
#define GUPT_ERRORS_H

// Error codes
typedef enum {
    KS_OK = 0,
    KS_ERROR = -1,
    KS_ERROR_NOMEM = -2,
    KS_ERROR_NOTFOUND = -3,
    KS_ERROR_INVALID = -4,
    KS_ERROR_IO = -5,
    KS_ERROR_DB = -6,
    KS_ERROR_TOOL = -7,
    KS_ERROR_PLUGIN = -8,
    KS_ERROR_PERMISSION = -9,
    KS_ERROR_TIMEOUT = -10,
} ks_error_t;

// Error messages
static const char *ks_error_messages[] = {
    "Success",
    "General error",
    "Memory allocation failed",
    "Not found",
    "Invalid input",
    "I/O error",
    "Database error",
    "Tool error",
    "Plugin error",
    "Permission denied",
    "Timeout",
};

// Get error message
static inline const char *ks_error_message(ks_error_t error) {
    if (error >= 0 && error <= KS_ERROR_TIMEOUT) {
        return ks_error_messages[-error];
    }
    return "Unknown error";
}

#endif // GUPT_ERRORS_H
