#ifndef GUPT_PORTABLE_H
#define GUPT_PORTABLE_H

/*
 * Gupt Portable Platform Abstraction Layer
 * Works on: Linux, Windows, macOS, FreeBSD, and more
 * Uses only C standard library + minimal OS APIs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

/* ========== Platform Detection ========== */
#if defined(_WIN32) || defined(_WIN64)
    #define KS_OS_WINDOWS 1
    #define KS_OS_NAME "Windows"
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #include <process.h>
    #define KS_PATH_SEP '\\'
    #define KS_PATH_SEP_STR "\\"
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    #define getcwd _getcwd
    #define chdir _chdir
    #define mkdir(d, p) _mkdir(d)
    #define popen _popen
    #define pclose _pclose
    #define access _access
    #define F_OK 0
    #define sleep_ms(ms) Sleep(ms)
#elif defined(__APPLE__)
    #define KS_OS_MACOS 1
    #define KS_OS_NAME "macOS"
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <pwd.h>
    #define KS_PATH_SEP '/'
    #define KS_PATH_SEP_STR "/"
    #define sleep_ms(ms) usleep((ms) * 1000)
#elif defined(__linux__)
    #define KS_OS_LINUX 1
    #define KS_OS_NAME "Linux"
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <pwd.h>
    #define KS_PATH_SEP '/'
    #define KS_PATH_SEP_STR "/"
    #define sleep_ms(ms) usleep((ms) * 1000)
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    #define KS_OS_BSD 1
    #define KS_OS_NAME "BSD"
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <pwd.h>
    #define KS_PATH_SEP '/'
    #define KS_PATH_SEP_STR "/"
    #define sleep_ms(ms) usleep((ms) * 1000)
#else
    #define KS_OS_UNKNOWN 1
    #define KS_OS_NAME "Unknown"
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #define KS_PATH_SEP '/'
    #define KS_PATH_SEP_STR "/"
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

/* ========== Type Aliases ========== */
typedef int64_t ks_int64;
typedef uint64_t ks_uint64;

/* ========== String Utilities ========== */
static inline int ks_strcasecmp(const char *s1, const char *s2) {
#ifdef KS_OS_WINDOWS
    return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}

static inline char *ks_strdup(const char *s) {
    if (!s) return NULL;
#ifdef KS_OS_WINDOWS
    return _strdup(s);
#else
    return strdup(s);
#endif
}

/* ========== Path Utilities ========== */
static inline char ks_path_sep(void) {
    return KS_PATH_SEP;
}

static inline char *ks_path_join(char *buf, size_t bufsz, const char *a, const char *b) {
    if (!buf || bufsz == 0) return NULL;
    snprintf(buf, bufsz, "%s%s%s", a, KS_PATH_SEP_STR, b);
    return buf;
}

static inline bool ks_file_exists(const char *path) {
    if (!path) return false;
#ifdef KS_OS_WINDOWS
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat st;
    return stat(path, &st) == 0;
#endif
}

static inline bool ks_is_dir(const char *path) {
    if (!path) return false;
#ifdef KS_OS_WINDOWS
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

static inline int ks_mkdir(const char *path) {
    if (!path) return -1;
#ifdef KS_OS_WINDOWS
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

static inline int ks_mkdir_p(const char *path) {
    if (!path) return -1;
    
    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s", path);
    
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = '\0';
            ks_mkdir(tmp);
            *p = c;
        }
    }
    return ks_mkdir(tmp);
}

static inline const char *ks_get_home(void) {
#ifdef KS_OS_WINDOWS
    const char *home = getenv("USERPROFILE");
    if (home) return home;
    const char *drive = getenv("HOMEDRIVE");
    const char *hpath = getenv("HOMEPATH");
    if (drive && hpath) {
        static char full[1024];
        snprintf(full, sizeof(full), "%s%s", drive, hpath);
        return full;
    }
    return "C:\\";
#else
    const char *home = getenv("HOME");
    if (home) return home;
    struct passwd *pw = getpwuid(getuid());
    if (pw) return pw->pw_dir;
    return "/tmp";
#endif
}

static inline const char *ks_get_cwd(char *buf, size_t bufsz) {
#ifdef KS_OS_WINDOWS
    return _getcwd(buf, bufsz);
#else
    return getcwd(buf, bufsz);
#endif
}

static inline int ks_set_cwd(const char *path) {
#ifdef KS_OS_WINDOWS
    return _chdir(path);
#else
    return chdir(path);
#endif
}

/* ========== Execute Command ========== */
static inline int ks_exec(const char *cmd, char *out, size_t outsz) {
    if (!cmd || !out || outsz == 0) return -1;
    
    FILE *fp;
#ifdef KS_OS_WINDOWS
    fp = _popen(cmd, "r");
#else
    fp = popen(cmd, "r");
#endif
    if (!fp) return -1;
    
    size_t n = fread(out, 1, outsz - 1, fp);
    out[n] = '\0';
    
#ifdef KS_OS_WINDOWS
    int status = _pclose(fp);
#else
    int status = pclose(fp);
#endif
    return status;
}

static inline bool ks_cmd_exists(const char *cmd) {
    if (!cmd) return false;
    char buf[256];
#ifdef KS_OS_WINDOWS
    snprintf(buf, sizeof(buf), "where %s >nul 2>nul", cmd);
#else
    snprintf(buf, sizeof(buf), "which %s >/dev/null 2>&1", cmd);
#endif
    return system(buf) == 0;
}

/* ========== File I/O ========== */
static inline char *ks_read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *buf = malloc(sz + 1);
    if (buf) {
        fread(buf, 1, sz, f);
        buf[sz] = '\0';
    }
    fclose(f);
    return buf;
}

static inline int ks_write_file(const char *path, const char *data) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fputs(data, f);
    fclose(f);
    return 0;
}

/* ========== Directory Listing ========== */
#ifdef KS_OS_WINDOWS
typedef struct {
    HANDLE handle;
    WIN32_FIND_DATAA fdata;
    int first;
} ks_dir_t;

static inline ks_dir_t *ks_opendir(const char *path) {
    char pattern[4096];
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    
    ks_dir_t *d = malloc(sizeof(ks_dir_t));
    if (!d) return NULL;
    
    d->handle = FindFirstFileA(pattern, &d->fdata);
    d->first = 1;
    
    if (d->handle == INVALID_HANDLE_VALUE) {
        free(d);
        return NULL;
    }
    return d;
}

static inline const char *ks_readdir(ks_dir_t *d) {
    if (!d) return NULL;
    if (d->first) {
        d->first = 0;
        return d->fdata.cFileName;
    }
    if (FindNextFileA(d->handle, &d->fdata))
        return d->fdata.cFileName;
    return NULL;
}

static inline void ks_closedir(ks_dir_t *d) {
    if (!d) return;
    FindClose(d->handle);
    free(d);
}
#else
typedef DIR ks_dir_t;

static inline ks_dir_t *ks_opendir(const char *path) {
    return opendir(path);
}

static inline const char *ks_readdir(ks_dir_t *d) {
    struct dirent *ent = readdir(d);
    return ent ? ent->d_name : NULL;
}

static inline void ks_closedir(ks_dir_t *d) {
    if (d) closedir(d);
}
#endif

/* ========== Logging ========== */
typedef enum { KS_LOG_TRACE, KS_LOG_DEBUG, KS_LOG_INFO, KS_LOG_WARN, KS_LOG_ERROR } ks_loglevel;

static const char *ks_log_colors[] = {
    "\033[37m", "\033[34m", "\033[32m", "\033[33m", "\033[31m"
};

static inline void ks_log(ks_loglevel lvl, const char *file, int line, const char *fmt, ...) {
    const char *level_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
    fprintf(stderr, "%s[%s]\033[0m %s:%d: ", ks_log_colors[lvl], level_names[lvl], file, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

#define KS_LOG(lvl, ...) ks_log(lvl, __FILE__, __LINE__, __VA_ARGS__)

#endif /* GUPT_PORTABLE_H */
