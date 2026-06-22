#include "gupt/gupt.h"

char *ks_file_read(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    
    fread(content, 1, size, f);
    content[size] = '\0';
    
    fclose(f);
    return content;
}

int ks_file_write(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    if (!f) return KS_ERROR_IO;
    
    fputs(content, f);
    fclose(f);
    return KS_OK;
}

int ks_file_append(const char *path, const char *content) {
    FILE *f = fopen(path, "a");
    if (!f) return KS_ERROR_IO;
    
    fputs(content, f);
    fclose(f);
    return KS_OK;
}

long ks_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }
    return st.st_size;
}

char **ks_file_list_dir(const char *path, int *count) {
    DIR *dir = opendir(path);
    if (!dir) return NULL;
    
    int capacity = 16;
    char **entries = malloc(sizeof(char *) * capacity);
    *count = 0;
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        if (*count >= capacity) {
            capacity *= 2;
            entries = realloc(entries, sizeof(char *) * capacity);
        }
        
        entries[*count] = strdup(entry->d_name);
        (*count)++;
    }
    
    closedir(dir);
    return entries;
}

void ks_file_list_free(char **list, int count) {
    for (int i = 0; i < count; i++) {
        free(list[i]);
    }
    free(list);
}
