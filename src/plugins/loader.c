#include "gupt/gupt.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void *ks_loader_open(const char *path) {
#ifdef _WIN32
    return (void *)LoadLibraryA(path);
#else
    return dlopen(path, RTLD_LAZY);
#endif
}

void *ks_loader_symbol(void *handle, const char *name) {
#ifdef _WIN32
    return (void *)GetProcAddress((HMODULE)handle, name);
#else
    return dlsym(handle, name);
#endif
}

int ks_loader_close(void *handle) {
#ifdef _WIN32
    return FreeLibrary((HMODULE)handle) ? 0 : -1;
#else
    return dlclose(handle);
#endif
}

const char *ks_loader_error(void) {
#ifdef _WIN32
    static char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                   0, buf, sizeof(buf), NULL);
    return buf;
#else
    return dlerror();
#endif
}
