#include "kalishell/kalishell.h"
#include <math.h>

// CVSS v3.1 Calculator Implementation

// Base Score Metrics
typedef enum {
    AV_N = 0,   // Network
    AV_A = 1,   // Adjacent
    AV_L = 2,   // Local
    AV_P = 3    // Physical
} ks_cvss_av_t;

typedef enum {
    AC_L = 0,   // Low
    AC_H = 1    // High
} ks_cvss_ac_t;

typedef enum {
    PR_N = 0,   // None
    PR_L = 1,   // Low
    PR_H = 2    // High
} ks_cvss_pr_t;

typedef enum {
    UI_N = 0,   // None
    UI_R = 1    // Required
} ks_cvss_ui_t;

typedef enum {
    S_U = 0,    // Unchanged
    S_C = 1     // Changed
} ks_cvss_s_t;

typedef enum {
    C_N = 0,    // None
    C_L = 1,    // Low
    C_H = 2     // High
} ks_cvss_c_t;

typedef enum {
    I_N = 0,    // None
    I_L = 1,    // Low
    I_H = 2     // High
} ks_cvss_i_t;

typedef enum {
    A_N = 0,    // None
    A_L = 1,    // Low
    A_H = 2     // High
} ks_cvss_a_t;

// Calculate CVSS v3.1 Base Score
float ks_cvss_calculate(int av, int ac, int pr, int ui, int s, int c, int i, int a) {
    // Impact Sub-Score
    float iss = 1.0f - ((1.0f - c * 0.56f) * (1.0f - i * 0.56f) * (1.0f - a * 0.56f));
    
    float impact;
    if (s == S_U) {
        impact = 6.42f * iss;
    } else {
        impact = 7.52f * (iss - 0.029f) - 3.25f * powf(iss - 0.02f, 15.0f);
    }
    
    // Exploitability Sub-Score
    float av_score, ac_score, pr_score, ui_score;
    
    switch (av) {
        case AV_N: av_score = 0.85f; break;
        case AV_A: av_score = 0.62f; break;
        case AV_L: av_score = 0.55f; break;
        case AV_P: av_score = 0.20f; break;
        default: av_score = 0.85f;
    }
    
    switch (ac) {
        case AC_L: ac_score = 0.77f; break;
        case AC_H: ac_score = 0.44f; break;
        default: ac_score = 0.77f;
    }
    
    // PR score depends on Scope
    if (s == S_U) {
        switch (pr) {
            case PR_N: pr_score = 0.85f; break;
            case PR_L: pr_score = 0.62f; break;
            case PR_H: pr_score = 0.27f; break;
            default: pr_score = 0.85f;
        }
    } else {
        switch (pr) {
            case PR_N: pr_score = 0.85f; break;
            case PR_L: pr_score = 0.68f; break;
            case PR_H: pr_score = 0.50f; break;
            default: pr_score = 0.85f;
        }
    }
    
    switch (ui) {
        case UI_N: ui_score = 0.85f; break;
        case UI_R: ui_score = 0.62f; break;
        default: ui_score = 0.85f;
    }
    
    float exploitability = 8.22f * av_score * ac_score * pr_score * ui_score;
    
    // Base Score
    float base_score;
    if (impact <= 0.0f) {
        base_score = 0.0f;
    } else {
        if (s == S_U) {
            base_score = fminf((impact + exploitability), 10.0f);
        } else {
            base_score = fminf(1.08f * (impact + exploitability), 10.0f);
        }
    }
    
    // Round up to nearest tenth
    base_score = ceilf(base_score * 10.0f) / 10.0f;
    
    return base_score;
}

// Get severity from CVSS score
const char *ks_cvss_severity(float score) {
    if (score >= 9.0f) return "critical";
    if (score >= 7.0f) return "high";
    if (score >= 4.0f) return "medium";
    if (score >= 0.1f) return "low";
    return "info";
}

// Get severity color
const char *ks_cvss_severity_color(float score) {
    if (score >= 9.0f) return COLOR_RED;
    if (score >= 7.0f) return COLOR_RED;
    if (score >= 4.0f) return COLOR_YELLOW;
    if (score >= 0.1f) return COLOR_BLUE;
    return COLOR_WHITE;
}

// Parse CVSS vector string
int ks_cvss_parse_vector(const char *vector, int *av, int *ac, int *pr, int *ui, 
                          int *s, int *c, int *i, int *a) {
    if (!vector) return KS_ERROR_INVALID;
    
    // Expected format: CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H
    char *copy = strdup(vector);
    if (!copy) return KS_ERROR_NOMEM;
    
    char *token = strtok(copy, "/");
    while (token) {
        if (strncmp(token, "AV:", 3) == 0) {
            switch (token[3]) {
                case 'N': *av = AV_N; break;
                case 'A': *av = AV_A; break;
                case 'L': *av = AV_L; break;
                case 'P': *av = AV_P; break;
            }
        } else if (strncmp(token, "AC:", 3) == 0) {
            switch (token[3]) {
                case 'L': *ac = AC_L; break;
                case 'H': *ac = AC_H; break;
            }
        } else if (strncmp(token, "PR:", 3) == 0) {
            switch (token[3]) {
                case 'N': *pr = PR_N; break;
                case 'L': *pr = PR_L; break;
                case 'H': *pr = PR_H; break;
            }
        } else if (strncmp(token, "UI:", 3) == 0) {
            switch (token[3]) {
                case 'N': *ui = UI_N; break;
                case 'R': *ui = UI_R; break;
            }
        } else if (strncmp(token, "S:", 2) == 0) {
            switch (token[2]) {
                case 'U': *s = S_U; break;
                case 'C': *s = S_C; break;
            }
        } else if (strncmp(token, "C:", 2) == 0) {
            switch (token[2]) {
                case 'N': *c = C_N; break;
                case 'L': *c = C_L; break;
                case 'H': *c = C_H; break;
            }
        } else if (strncmp(token, "I:", 2) == 0) {
            switch (token[2]) {
                case 'N': *i = I_N; break;
                case 'L': *i = I_L; break;
                case 'H': *i = I_H; break;
            }
        } else if (strncmp(token, "A:", 2) == 0) {
            switch (token[2]) {
                case 'N': *a = A_N; break;
                case 'L': *a = A_L; break;
                case 'H': *a = A_H; break;
            }
        }
        token = strtok(NULL, "/");
    }
    
    free(copy);
    return KS_OK;
}

// Calculate score from vector string
float ks_cvss_calculate_from_vector(const char *vector) {
    int av = AV_N, ac = AC_L, pr = PR_N, ui = UI_N;
    int s = S_U, c = C_H, i = I_H, a = A_H;
    
    ks_cvss_parse_vector(vector, &av, &ac, &pr, &ui, &s, &c, &i, &a);
    
    return ks_cvss_calculate(av, ac, pr, ui, s, c, i, a);
}

// CVSS calculator command
int ks_cmd_cvss(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: cvss <vector>\n");
        printf("Example: cvss CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H\n");
        return 1;
    }
    
    float score = ks_cvss_calculate_from_vector(argv[1]);
    const char *severity = ks_cvss_severity(score);
    const char *color = ks_cvss_severity_color(score);
    
    printf("CVSS Score: %s%.1f%s (%s)\n", color, score, COLOR_RESET, severity);
    
    return 0;
}
