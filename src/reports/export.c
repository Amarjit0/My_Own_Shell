#include "kalishell/kalishell.h"

int ks_export_pdf(const char *html, const char *output_path) {
    // TODO: Export to PDF using wkhtmltopdf or similar
    printf("PDF export not yet implemented\n");
    return KS_OK;
}

int ks_export_html(const char *html, const char *output_path) {
    // TODO: Export to HTML file
    FILE *f = fopen(output_path, "w");
    if (!f) {
        return KS_ERROR_IO;
    }
    
    fprintf(f, "%s", html);
    fclose(f);
    
    return KS_OK;
}
