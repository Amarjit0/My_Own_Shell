#include "gupt/gupt.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <process.h>
#else
#include <sys/wait.h>
#include <sys/types.h>
#endif

// Platform detection
#ifdef _WIN32
#define KS_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define KS_PLATFORM_MACOS 1
#else
#define KS_PLATFORM_LINUX 1
#endif

// Get platform name
const char *ks_platform_get_name(void) {
#ifdef KS_PLATFORM_WINDOWS
    return "Windows";
#elif defined(KS_PLATFORM_MACOS)
    return "macOS";
#else
    return "Linux";
#endif
}

// Get platform-specific path separator
char ks_platform_get_path_separator(void) {
#ifdef KS_PLATFORM_WINDOWS
    return '\\';
#else
    return '/';
#endif
}

// Execute a command and capture output
int ks_platform_execute(const char *command, char *output, size_t output_size) {
    if (!command || !output || output_size == 0) {
        return KS_ERROR_INVALID;
    }
    
#ifdef KS_PLATFORM_WINDOWS
    // Windows implementation
    FILE *pipe = _popen(command, "r");
    if (!pipe) {
        return KS_ERROR_IO;
    }
    
    size_t bytes_read = fread(output, 1, output_size - 1, pipe);
    output[bytes_read] = '\0';
    
    int status = _pclose(pipe);
    return WEXITSTATUS(status);
#else
    // Unix implementation
    FILE *pipe = popen(command, "r");
    if (!pipe) {
        return KS_ERROR_IO;
    }
    
    size_t bytes_read = fread(output, 1, output_size - 1, pipe);
    output[bytes_read] = '\0';
    
    int status = pclose(pipe);
    return WEXITSTATUS(status);
#endif
}

// Execute a command in background
int ks_platform_execute_background(const char *command) {
    if (!command) {
        return KS_ERROR_INVALID;
    }
    
#ifdef KS_PLATFORM_WINDOWS
    // Windows implementation
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "start %s", command);
    return system(cmd);
#else
    // Unix implementation
    pid_t pid = fork();
    if (pid == -1) {
        return KS_ERROR_IO;
    }
    
    if (pid == 0) {
        // Child process
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }
    
    // Parent process
    return KS_OK;
#endif
}

// Check if a command exists in PATH
bool ks_platform_command_exists(const char *command) {
    if (!command) {
        return false;
    }
    
#ifdef KS_PLATFORM_WINDOWS
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "where %s >nul 2>nul");
    return system(cmd) == 0;
#else
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "which %s > /dev/null 2>&1", command);
    return system(cmd) == 0;
#endif
}

// Get current working directory
char *ks_platform_get_cwd(void) {
    char *cwd = malloc(MAX_PATH_SIZE);
    if (!cwd) {
        return NULL;
    }
    
    if (getcwd(cwd, MAX_PATH_SIZE) == NULL) {
        free(cwd);
        return NULL;
    }
    
    return cwd;
}

// Set current working directory
int ks_platform_set_cwd(const char *path) {
    if (!path) {
        return KS_ERROR_INVALID;
    }
    
    return chdir(path) == 0 ? KS_OK : KS_ERROR_IO;
}

// Get home directory
char *ks_platform_get_home(void) {
    char *home = getenv("HOME");
    if (home) {
        return strdup(home);
    }
    
#ifdef KS_PLATFORM_WINDOWS
    home = getenv("USERPROFILE");
    if (home) {
        return strdup(home);
    }
    
    char *drive = getenv("HOMEDRIVE");
    char *path = getenv("HOMEPATH");
    if (drive && path) {
        char *full_home = malloc(strlen(drive) + strlen(path) + 1);
        if (full_home) {
            strcpy(full_home, drive);
            strcat(full_home, path);
        }
        return full_home;
    }
#endif
    
    return strdup("/tmp");
}

// Create directory recursively
int ks_platform_mkdir_recursive(const char *path, mode_t mode) {
    if (!path) {
        return KS_ERROR_INVALID;
    }
    
    char *tmp = strdup(path);
    if (!tmp) {
        return KS_ERROR_NOMEM;
    }
    
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
#ifdef KS_PLATFORM_WINDOWS
            _mkdir(tmp);
#else
            mkdir(tmp, mode);
#endif
            *p = '/';
        }
    }
    
#ifdef KS_PLATFORM_WINDOWS
    int result = _mkdir(tmp) == 0 ? KS_OK : KS_ERROR_IO;
#else
    int result = mkdir(tmp, mode) == 0 ? KS_OK : KS_ERROR_IO;
#endif
    
    free(tmp);
    return result;
}

// Get file modification time
time_t ks_platform_get_mtime(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return st.st_mtime;
}

// Check if path is executable
bool ks_platform_is_executable(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    
#ifdef KS_PLATFORM_WINDOWS
    // On Windows, check file extension
    const char *ext = strrchr(path, '.');
    if (ext) {
        return strcasecmp(ext, ".exe") == 0 ||
               strcasecmp(ext, ".cmd") == 0 ||
               strcasecmp(ext, ".bat") == 0;
    }
    return false;
#else
    return (st.st_mode & S_IXUSR) != 0;
#endif
}

// Get process ID
int ks_platform_get_pid(void) {
#ifdef KS_PLATFORM_WINDOWS
    return (int)_getpid();
#else
    return (int)getpid();
#endif
}

// Sleep for milliseconds
void ks_platform_sleep_ms(int ms) {
#ifdef KS_PLATFORM_WINDOWS
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
