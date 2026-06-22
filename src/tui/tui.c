#include "gupt/gupt.h"

#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif

// TUI state
static int tui_initialized = 0;

// Initialize TUI
int ks_tui_init(void) {
#ifdef HAVE_NCURSES
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Enable colors
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    }
    
    tui_initialized = 1;
#endif
    return KS_OK;
}

// Cleanup TUI
void ks_tui_cleanup(void) {
#ifdef HAVE_NCURSES
    if (tui_initialized) {
        endwin();
        tui_initialized = 0;
    }
#endif
}

// Draw progress bar
int ks_tui_draw_progress(const char *label, int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    
    printf("\r  %s [", label);
    
    int width = 30;
    int filled = (percent * width) / 100;
    
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    
    printf("] %d%%", percent);
    fflush(stdout);
    
    if (percent >= 100) {
        printf("\n");
    }
    
    return KS_OK;
}

// Draw dashboard
int ks_tui_draw_dashboard(void) {
    printf("\n");
    printf(COLOR_BOLD "╔══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_BOLD "║                    Gupt Dashboard                       ║\n" COLOR_RESET);
    printf(COLOR_BOLD "╚══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
    
    // Target info
    printf(COLOR_CYAN "🎯 Current Target:" COLOR_RESET " None\n");
    printf("\n");
    
    // Stats
    printf(COLOR_BOLD "📊 Statistics:\n" COLOR_RESET);
    printf("  Subdomains:  " COLOR_GREEN "0" COLOR_RESET "\n");
    printf("  Live Hosts:  " COLOR_GREEN "0" COLOR_RESET "\n");
    printf("  Endpoints:   " COLOR_GREEN "0" COLOR_RESET "\n");
    printf("  Findings:    " COLOR_GREEN "0" COLOR_RESET "\n");
    printf("\n");
    
    // Recent findings
    printf(COLOR_BOLD "🔍 Recent Findings:\n" COLOR_RESET);
    printf("  No findings yet\n");
    printf("\n");
    
    // Quick actions
    printf(COLOR_BOLD "⚡ Quick Actions:\n" COLOR_RESET);
    printf("  1. Start recon    2. View findings   3. Generate report\n");
    printf("  4. Open workspace 5. Add target      6. View graph\n");
    printf("\n");
    
    return KS_OK;
}

// Draw box with title
int ks_tui_draw_box(const char *title, const char *content) {
    int width = 60;
    
    printf("┌");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┐\n");
    
    printf("│" COLOR_BOLD " %s" COLOR_RESET, title);
    int padding = width - 2 - strlen(title) - 1;
    for (int i = 0; i < padding; i++) printf(" ");
    printf("│\n");
    
    printf("├");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┤\n");
    
    // Content lines
    char *content_copy = strdup(content);
    char *line = strtok(content_copy, "\n");
    while (line) {
        printf("│ %s", line);
        int padding = width - 2 - strlen(line) - 1;
        if (padding > 0) {
            for (int i = 0; i < padding; i++) printf(" ");
        }
        printf("│\n");
        line = strtok(NULL, "\n");
    }
    free(content_copy);
    
    printf("└");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┘\n");
    
    return KS_OK;
}

// Draw table
int ks_tui_draw_table(const char *headers[], const char *data[][8], int rows, int cols) {
    // Calculate column widths
    int widths[8] = {0};
    for (int i = 0; i < cols; i++) {
        widths[i] = strlen(headers[i]);
        for (int j = 0; j < rows; j++) {
            int len = strlen(data[j][i]);
            if (len > widths[i]) widths[i] = len;
        }
        widths[i] += 2;  // Padding
    }
    
    // Print header
    printf("┌");
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < widths[i]; j++) printf("─");
        if (i < cols - 1) printf("┬");
    }
    printf("┐\n");
    
    printf("│");
    for (int i = 0; i < cols; i++) {
        printf(COLOR_BOLD " %-*s" COLOR_RESET, widths[i] - 1, headers[i]);
        if (i < cols - 1) printf("│");
    }
    printf("│\n");
    
    printf("├");
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < widths[i]; j++) printf("─");
        if (i < cols - 1) printf("┼");
    }
    printf("┤\n");
    
    // Print data
    for (int j = 0; j < rows; j++) {
        printf("│");
        for (int i = 0; i < cols; i++) {
            printf(" %-*s", widths[i] - 1, data[j][i]);
            if (i < cols - 1) printf("│");
        }
        printf("│\n");
    }
    
    // Print footer
    printf("└");
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < widths[i]; j++) printf("─");
        if (i < cols - 1) printf("┴");
    }
    printf("┘\n");
    
    return KS_OK;
}

// Draw spinner
int ks_tui_draw_spinner(int spin) {
    const char *spinners[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    printf("\r  %s Working...", spinners[spin % 10]);
    fflush(stdout);
    return KS_OK;
}

// Clear line
int ks_tui_clear_line(void) {
    printf("\r\033[K");
    return KS_OK;
}
