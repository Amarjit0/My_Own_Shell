#include "kalishell/kalishell.h"

// Plugin API version
#define KALISHELL_PLUGIN_API_VERSION 1

// Plugin function signatures
typedef int (*plugin_init_func)(void);
typedef int (*plugin_cleanup_func)(void);
typedef int (*plugin_execute_func)(int argc, char **argv);
typedef const char *(*plugin_get_name_func)(void);
typedef const char *(*plugin_get_version_func)(void);

// Forward declarations
int ks_plugin_scan_dir(const char *dir_path);
int ks_plugin_load(const char *path);

// Plugin structure
typedef struct {
    char *name;
    char *version;
    char *description;
    char *author;
    char *path;
    void *handle;
    plugin_init_func init;
    plugin_cleanup_func cleanup;
    plugin_execute_func execute;
    plugin_get_name_func get_name;
    plugin_get_version_func get_version;
    bool loaded;
} ks_plugin_t;

// Plugin registry
static ks_plugin_t *plugins = NULL;
static int plugin_count = 0;
static int plugin_capacity = 0;

// Initialize plugin system
int ks_plugin_init(void) {
    plugin_capacity = 16;
    plugins = calloc(plugin_capacity, sizeof(ks_plugin_t));
    if (!plugins) return KS_ERROR_NOMEM;
    
    // Scan for plugins in plugin directory
    char *home = ks_platform_get_home();
    char *plugin_dir = ks_path_join(home, ".kalishell/plugins");
    
    if (plugin_dir && ks_path_exists(plugin_dir)) {
        ks_plugin_scan_dir(plugin_dir);
    }
    
    free(home);
    free(plugin_dir);
    
    return KS_OK;
}

// Cleanup plugin system
void ks_plugin_cleanup(void) {
    for (int i = 0; i < plugin_count; i++) {
        if (plugins[i].loaded && plugins[i].cleanup) {
            plugins[i].cleanup();
        }
        
#ifdef HAVE_DLOPEN
        if (plugins[i].handle) {
            dlclose(plugins[i].handle);
        }
#endif
        
        free(plugins[i].name);
        free(plugins[i].version);
        free(plugins[i].description);
        free(plugins[i].author);
        free(plugins[i].path);
    }
    
    free(plugins);
    plugins = NULL;
    plugin_count = 0;
    plugin_capacity = 0;
}

// Scan directory for plugins
int ks_plugin_scan_dir(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) return KS_ERROR_IO;
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        // Check for .so or .dll files
        char *ext = strrchr(entry->d_name, '.');
        if (!ext) continue;
        
#ifdef _WIN32
        if (strcasecmp(ext, ".dll") != 0) continue;
#else
        if (strcmp(ext, ".so") != 0) continue;
#endif
        
        char *plugin_path = ks_path_join(dir_path, entry->d_name);
        if (plugin_path) {
            ks_plugin_load(plugin_path);
            free(plugin_path);
        }
    }
    
    closedir(dir);
    return KS_OK;
}

// Load a plugin
int ks_plugin_load(const char *path) {
    if (!path) return KS_ERROR_INVALID;
    
#ifdef HAVE_DLOPEN
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Cannot load plugin: %s\n", dlerror());
        return KS_ERROR;
    }
    
    // Get function pointers
    plugin_init_func init = dlsym(handle, "plugin_init");
    plugin_cleanup_func cleanup = dlsym(handle, "plugin_cleanup");
    plugin_execute_func execute = dlsym(handle, "plugin_execute");
    plugin_get_name_func get_name = dlsym(handle, "plugin_get_name");
    plugin_get_version_func get_version = dlsym(handle, "plugin_get_version");
    
    if (!get_name || !execute) {
        fprintf(stderr, "Invalid plugin: missing required functions\n");
        dlclose(handle);
        return KS_ERROR;
    }
    
    // Create plugin entry
    if (plugin_count >= plugin_capacity) {
        plugin_capacity *= 2;
        plugins = realloc(plugins, sizeof(ks_plugin_t) * plugin_capacity);
    }
    
    ks_plugin_t *plugin = &plugins[plugin_count++];
    plugin->name = strdup(get_name());
    plugin->version = get_version ? strdup(get_version()) : strdup("1.0");
    plugin->description = NULL;
    plugin->author = NULL;
    plugin->path = strdup(path);
    plugin->handle = handle;
    plugin->init = init;
    plugin->cleanup = cleanup;
    plugin->execute = execute;
    plugin->get_name = get_name;
    plugin->get_version = get_version;
    plugin->loaded = false;
    
    // Initialize plugin
    if (init) {
        if (init() == KS_OK) {
            plugin->loaded = true;
            printf("[+] Plugin loaded: %s v%s\n", plugin->name, plugin->version);
        } else {
            fprintf(stderr, "Failed to initialize plugin: %s\n", plugin->name);
        }
    } else {
        plugin->loaded = true;
        printf("[+] Plugin loaded: %s v%s\n", plugin->name, plugin->version);
    }
    
    return KS_OK;
#else
    fprintf(stderr, "Dynamic loading not available\n");
    return KS_ERROR;
#endif
}

// Unload a plugin
int ks_plugin_unload(const char *name) {
    for (int i = 0; i < plugin_count; i++) {
        if (strcmp(plugins[i].name, name) == 0) {
            if (plugins[i].loaded && plugins[i].cleanup) {
                plugins[i].cleanup();
            }
            
#ifdef HAVE_DLOPEN
            if (plugins[i].handle) {
                dlclose(plugins[i].handle);
            }
#endif
            
            free(plugins[i].name);
            free(plugins[i].version);
            free(plugins[i].description);
            free(plugins[i].author);
            free(plugins[i].path);
            
            // Shift array
            for (int j = i; j < plugin_count - 1; j++) {
                plugins[j] = plugins[j + 1];
            }
            plugin_count--;
            
            printf("[+] Plugin unloaded: %s\n", name);
            return KS_OK;
        }
    }
    
    return KS_ERROR_NOTFOUND;
}

// Find plugin by name
ks_plugin_t *ks_plugin_find(const char *name) {
    for (int i = 0; i < plugin_count; i++) {
        if (strcmp(plugins[i].name, name) == 0) {
            return &plugins[i];
        }
    }
    return NULL;
}

// Execute a plugin
int ks_plugin_execute(const char *name, int argc, char **argv) {
    ks_plugin_t *plugin = ks_plugin_find(name);
    if (!plugin) {
        return KS_ERROR_NOTFOUND;
    }
    
    if (!plugin->loaded || !plugin->execute) {
        return KS_ERROR;
    }
    
    return plugin->execute(argc, argv);
}

// List all plugins
int ks_plugin_list(void) {
    printf("Installed plugins:\n\n");
    printf("  %-20s %-10s %s\n", "Name", "Version", "Description");
    printf("  %-20s %-10s %s\n", "----", "-------", "-----------");
    
    for (int i = 0; i < plugin_count; i++) {
        printf("  %-20s %-10s %s\n",
               plugins[i].name,
               plugins[i].version,
               plugins[i].description ? plugins[i].description : "");
    }
    
    printf("\nTotal: %d plugins\n", plugin_count);
    return KS_OK;
}

// Plugin command handler
int ks_cmd_plugin(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: plugin <list|load|unload|execute> [args]\n");
        return 1;
    }
    
    if (strcmp(argv[1], "list") == 0) {
        return ks_plugin_list();
    }
    
    if (strcmp(argv[1], "load") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: plugin load <path>\n");
            return 1;
        }
        return ks_plugin_load(argv[2]);
    }
    
    if (strcmp(argv[1], "unload") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: plugin unload <name>\n");
            return 1;
        }
        return ks_plugin_unload(argv[2]);
    }
    
    if (strcmp(argv[1], "execute") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: plugin execute <name> [args...]\n");
            return 1;
        }
        return ks_plugin_execute(argv[2], argc - 3, argv + 3);
    }
    
    fprintf(stderr, "Unknown plugin command: %s\n", argv[1]);
    return 1;
}
