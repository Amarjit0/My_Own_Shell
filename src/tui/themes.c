#include "kalishell/kalishell.h"

typedef struct {
    char *name;
    char *primary;
    char *secondary;
    char *accent;
    char *background;
} ks_theme_t;

static ks_theme_t current_theme = {
    .name = "default",
    .primary = COLOR_CYAN,
    .secondary = COLOR_GREEN,
    .accent = COLOR_YELLOW,
    .background = COLOR_RESET
};

int ks_theme_set(const char *name) {
    // TODO: Load theme by name
    return KS_OK;
}

ks_theme_t *ks_theme_get_current(void) {
    return &current_theme;
}
