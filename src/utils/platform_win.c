#include "gupt/gupt.h"
#include "gupt/platform.h"

#ifdef KS_PLATFORM_WINDOWS

// Get platform name
const char *ks_platform_get_name(void) {
    return "Windows";
}

// Get platform bits (32 or 64)
int ks_platform_get_bits(void) {
    #ifdef _WIN64
        return 64;
    #else
        return 32;
    #endif
}

// Get path separator
char ks_platform_get_path_separator(void) {
    return '\\';
}

// Normalize path (convert / to \)
char *ks_platform_normalize_path(const char *path) {
    if (!path) return NULL;
    
    char *normalized = strdup(path);
    if (!normalized) return NULL;
    
    for (char *p = normalized; *p; p++) {
        if (*p == '/') *p = '\\';
    }
    
    return normalized;
}

// Execute command and capture output
int ks_platform_execute(const char *cmd, char *output, size_t output_size) {
    if (!cmd || !output || output_size == 0) return -1;
    
    FILE *pipe = _popen(cmd, "r");
    if (!pipe) return -1;
    
    size_t bytes_read = fread(output, 1, output_size - 1, pipe);
    output[bytes_read] = '\0';
    
    int status = _pclose(pipe);
    return status;
}

// Execute command in background
int ks_platform_execute_background(const char *cmd) {
    if (!cmd) return -1;
    
    char full_cmd[4096];
    snprintf(full_cmd, sizeof(full_cmd), "start /b %s", cmd);
    return system(full_cmd);
}

// Check if command exists
int ks_platform_command_exists(const char *cmd) {
    if (!cmd) return 0;
    
    char check_cmd[1024];
    snprintf(check_cmd, sizeof(check_cmd), "where %s >nul 2>nul", cmd);
    return system(check_cmd) == 0;
}

// Get current working directory
char *ks_platform_get_cwd(void) {
    char *cwd = malloc(MAX_PATH);
    if (!cwd) return NULL;
    
    if (_getcwd(cwd, MAX_PATH) == NULL) {
        free(cwd);
        return NULL;
    }
    
    return cwd;
}

// Set current working directory
int ks_platform_set_cwd(const char *path) {
    if (!path) return -1;
    return _chdir(path) == 0 ? 0 : -1;
}

// Get home directory
char *ks_platform_get_home(void) {
    char *home = getenv("USERPROFILE");
    if (home) return strdup(home);
    
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
    
    return strdup("C:\\");
}

// Create directory recursively
int ks_platform_mkdir_p(const char *path) {
    if (!path) return -1;
    
    char *tmp = strdup(path);
    if (!tmp) return -1;
    
    // Convert forward slashes to backslashes
    for (char *p = tmp; *p; p++) {
        if (*p == '/') *p = '\\';
    }
    
    // Create each directory component
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '\\') {
            *p = '\0';
            _mkdir(tmp);
            *p = '\\';
        }
    }
    
    _mkdir(tmp);
    free(tmp);
    
    return 0;
}

// Check if file exists
int ks_platform_file_exists(const char *path) {
    struct _stat st;
    return _stat(path, &st) == 0;
}

// Check if path is directory
int ks_platform_is_dir(const char *path) {
    struct _stat st;
    if (_stat(path, &st) != 0) return 0;
    return (st.st_mode & _S_IFDIR) != 0;
}

// Get file size
long ks_platform_file_size(const char *path) {
    struct _stat st;
    if (_stat(path, &st) != 0) return -1;
    return st.st_size;
}

// Get file modification time
time_t ks_platform_file_mtime(const char *path) {
    struct _stat st;
    if (_stat(path, &st) != 0) return 0;
    return st.st_mtime;
}

// Sleep milliseconds
void ks_platform_sleep_ms(int ms) {
    Sleep(ms);
}

// Get process ID
int ks_platform_get_pid(void) {
    return (int)_getpid();
}

// Directory listing for Windows
typedef struct {
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle;
    char *path;
} ks_dir_iter_t;

void *ks_platform_opendir(const char *path) {
    if (!path) return NULL;
    
    char pattern[MAX_PATH];
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    
    ks_dir_iter_t *iter = malloc(sizeof(ks_dir_iter_t));
    if (!iter) return NULL;
    
    iter->find_handle = FindFirstFileA(pattern, &iter->find_data);
    iter->path = strdup(path);
    
    if (iter->find_handle == INVALID_HANDLE_VALUE) {
        free(iter->path);
        free(iter);
        return NULL;
    }
    
    return iter;
}

const char *ks_platform_readdir(void *dir) {
    if (!dir) return NULL;
    
    ks_dir_iter_t *iter = (ks_dir_iter_t *)dir;
    return iter->find_data.cFileName;
}

int ks_platform_readdir_next(void *dir) {
    if (!dir) return 0;
    
    ks_dir_iter_t *iter = (ks_dir_iter_t *)dir;
    return FindNextFileA(iter->find_handle, &iter->find_data) != 0;
}

void ks_platform_closedir(void *dir) {
    if (!dir) return;
    
    ks_dir_iter_t *iter = (ks_dir_iter_t *)dir;
    FindClose(iter->find_handle);
    free(iter->path);
    free(iter);
}

#endif // KS_PLATFORM_WINDOWS
