#ifndef GUPT_PLATFORM_H
#define GUPT_PLATFORM_H

#ifdef _WIN32
    #define KS_PLATFORM_WINDOWS 1
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #include <process.h>
    #include <conio.h>
    
    // Windows compatibility macros
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    #define getcwd _getcwd
    #define chdir _chdir
    #define mkdir _mkdir
    #define access _access
    #define popen _popen
    #define pclose _pclose
    
    // Path separator
    #define KS_PATH_SEP '\\'
    #define KS_PATH_SEP_STR "\\"
    
    // File operations
    #define KS_STAT _stat
    #define KS_S_IFDIR _S_IFDIR
    
#else
    #define KS_PLATFORM_UNIX 1
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <dirent.h>
    #include <dlfcn.h>
    
    // Path separator
    #define KS_PATH_SEP '/'
    #define KS_PATH_SEP_STR "/"
    
    // File operations
    #define KS_STAT stat
    #define KS_S_IFDIR S_IFDIR
    
#endif

// Cross-platform types
#ifdef _WIN32
    typedef __int64 ks_int64_t;
    typedef unsigned __int64 ks_uint64_t;
#else
    typedef int64_t ks_int64_t;
    typedef uint64_t ks_uint64_t;
#endif

// Cross-platform functions
#ifdef __cplusplus
extern "C" {
#endif

// Platform detection
const char *ks_platform_get_name(void);
int ks_platform_get_bits(void);

// Path utilities
char ks_platform_get_path_separator(void);
char *ks_platform_normalize_path(const char *path);

// Execute command
int ks_platform_execute(const char *cmd, char *output, size_t output_size);
int ks_platform_execute_background(const char *cmd);
int ks_platform_command_exists(const char *cmd);

// Directory operations
char *ks_platform_get_cwd(void);
int ks_platform_set_cwd(const char *path);
char *ks_platform_get_home(void);
int ks_platform_mkdir_p(const char *path);

// File operations
int ks_platform_file_exists(const char *path);
int ks_platform_is_dir(const char *path);
long ks_platform_file_size(const char *path);
time_t ks_platform_file_mtime(const char *path);

// Sleep
void ks_platform_sleep_ms(int ms);

// Process
int ks_platform_get_pid(void);

#ifdef __cplusplus
}
#endif

#endif // GUPT_PLATFORM_H
