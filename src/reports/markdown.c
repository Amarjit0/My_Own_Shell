#include "kalishell/kalishell.h"

char *ks_markdown_to_html(const char *markdown) {
    // TODO: Convert markdown to HTML
    // Simple placeholder
    char *html = malloc(strlen(markdown) + 100);
    if (html) {
        snprintf(html, strlen(markdown) + 100, 
            "<html><body><pre>%s</pre></body></html>", markdown);
    }
    return html;
}

char *ks_markdown_to_docx(const char *markdown) {
    // TODO: Convert markdown to DOCX
    printf("DOCX conversion not yet implemented\n");
    return NULL;
}
