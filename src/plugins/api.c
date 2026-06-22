#include "kalishell/kalishell.h"

// Plugin API definitions
typedef struct {
    int version;
    const char *(*get_name)(void);
    const char *(*get_version)(void);
    int (*execute)(int argc, char **argv);
} ks_plugin_api_t;

int ks_plugin_api_version(void) {
    return 1;
}
