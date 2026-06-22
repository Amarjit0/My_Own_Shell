#include "gupt/gupt.h"

ks_shell_t shell;

int main(int argc, char **argv) {
    // Initialize shell
    if (ks_shell_init(&shell) != KS_OK) {
        fprintf(stderr, "Failed to initialize shell\n");
        return 1;
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            shell.verbose = true;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("%s version %s\n", GUPT_NAME, GUPT_VERSION);
            ks_shell_cleanup(&shell);
            return 0;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [OPTIONS]\n", GUPT_NAME);
            printf("  --verbose, -v     Enable verbose output\n");
            printf("  --version         Show version\n");
            printf("  --help, -h        Show this help\n");
            ks_shell_cleanup(&shell);
            return 0;
        } else if (strcmp(argv[i], "--workspace") == 0 || strcmp(argv[i], "-w") == 0) {
            if (i + 1 < argc) {
                shell.current_workspace = strdup(argv[++i]);
            } else {
                fprintf(stderr, "Error: --workspace requires a name\n");
                ks_shell_cleanup(&shell);
                return 1;
            }
        }
    }

    // Run shell
    int result = ks_shell_run(&shell);

    // Cleanup
    ks_shell_cleanup(&shell);

    return result;
}
