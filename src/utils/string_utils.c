#include "gupt/gupt.h"

char *ks_string_to_lower(const char *str) {
    if (!str) return NULL;
    
    char *lower = strdup(str);
    if (!lower) return NULL;
    
    for (char *p = lower; *p; p++) {
        *p = tolower(*p);
    }
    return lower;
}

char *ks_string_to_upper(const char *str) {
    if (!str) return NULL;
    
    char *upper = strdup(str);
    if (!upper) return NULL;
    
    for (char *p = upper; *p; p++) {
        *p = toupper(*p);
    }
    return upper;
}

char *ks_string_replace(const char *str, const char *old, const char *new_str) {
    if (!str || !old || !new_str) return NULL;
    
    size_t old_len = strlen(old);
    size_t new_len = strlen(new_str);
    
    // Count occurrences
    int count = 0;
    const char *tmp = str;
    while ((tmp = strstr(tmp, old)) != NULL) {
        count++;
        tmp += old_len;
    }
    
    // Allocate result
    size_t result_len = strlen(str) + count * (new_len - old_len) + 1;
    char *result = malloc(result_len);
    if (!result) return NULL;
    
    // Replace
    char *p = result;
    const char *remaining = str;
    const char *match;
    
    while ((match = strstr(remaining, old)) != NULL) {
        size_t prefix_len = match - remaining;
        memcpy(p, remaining, prefix_len);
        p += prefix_len;
        
        memcpy(p, new_str, new_len);
        p += new_len;
        
        remaining = match + old_len;
    }
    
    strcpy(p, remaining);
    
    return result;
}

int ks_string_count(const char *str, char c) {
    if (!str) return 0;
    
    int count = 0;
    for (const char *p = str; *p; p++) {
        if (*p == c) count++;
    }
    return count;
}

bool ks_string_contains(const char *str, const char *substr) {
    if (!str || !substr) return false;
    return strstr(str, substr) != NULL;
}
