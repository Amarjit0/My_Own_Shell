#include "kalishell/kalishell.h"
#include <stdarg.h>

static ks_log_level_t current_level = LOG_INFO;
static FILE *log_file = NULL;

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *level_colors[] = {
    COLOR_WHITE, COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_RED, COLOR_RED
};

void ks_log_set_level(ks_log_level_t level) {
    current_level = level;
}

void ks_log_set_file(const char *path) {
    if (log_file) {
        fclose(log_file);
    }
    log_file = fopen(path, "a");
}

void ks_log(ks_log_level_t level, const char *file, int line, const char *fmt, ...) {
    if (level < current_level) {
        return;
    }
    
    // Get timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *tm_info = localtime(&tv.tv_sec);
    
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Get filename from path
    const char *filename = strrchr(file, '/');
    if (filename) {
        filename++;
    } else {
        filename = file;
    }
    
    // Print to stderr with color
    fprintf(stderr, "%s[%s]%s %s %s:%d - ", 
        level_colors[level], level_strings[level], COLOR_RESET,
        timestamp, filename, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    
    // Write to file if set
    if (log_file) {
        fprintf(log_file, "[%s] %s %s:%d - ", 
            level_strings[level], timestamp, filename, line);
        
        va_start(args, fmt);
        vfprintf(log_file, fmt, args);
        va_end(args);
        
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}

void ks_log_cleanup(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}
