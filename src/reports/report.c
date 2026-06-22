#include "gupt/gupt.h"

extern ks_shell_t shell;

// Report structure
typedef struct {
    char *title;
    char *target;
    char *date;
    char *author;
    char *executive_summary;
    char *scope;
    char *methodology;
    char *findings;
    char *recommendations;
    char *conclusion;
} ks_report_t;

// Create a new report
ks_report_t *ks_report_create_new(const char *title, const char *target) {
    ks_report_t *report = calloc(1, sizeof(ks_report_t));
    if (!report) return NULL;
    
    report->title = strdup(title);
    report->target = strdup(target);
    
    // Get current date
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char date_buf[64];
    strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", tm);
    report->date = strdup(date_buf);
    
    report->author = strdup("Gupt User");
    
    return report;
}

// Free a report
void ks_report_free(ks_report_t *report) {
    if (!report) return;
    
    free(report->title);
    free(report->target);
    free(report->date);
    free(report->author);
    free(report->executive_summary);
    free(report->scope);
    free(report->methodology);
    free(report->findings);
    free(report->recommendations);
    free(report->conclusion);
    free(report);
}

// Generate markdown report
char *ks_report_generate_markdown(ks_report_t *report) {
    if (!report) return NULL;
    
    char *markdown = malloc(8192);
    if (!markdown) return NULL;
    
    snprintf(markdown, 8192,
        "# %s\n\n"
        "**Target:** %s\n\n"
        "**Date:** %s\n\n"
        "**Author:** %s\n\n"
        "---\n\n"
        "## Executive Summary\n\n"
        "%s\n\n"
        "## Scope\n\n"
        "%s\n\n"
        "## Methodology\n\n"
        "%s\n\n"
        "## Findings\n\n"
        "%s\n\n"
        "## Recommendations\n\n"
        "%s\n\n"
        "## Conclusion\n\n"
        "%s\n\n",
        report->title ? report->title : "Security Assessment Report",
        report->target ? report->target : "N/A",
        report->date ? report->date : "N/A",
        report->author ? report->author : "N/A",
        report->executive_summary ? report->executive_summary : "This report presents the findings of a security assessment.",
        report->scope ? report->scope : "The assessment covered the target system.",
        report->methodology ? report->methodology : "Standard penetration testing methodology was followed.",
        report->findings ? report->findings : "No findings to report.",
        report->recommendations ? report->recommendations : "Implement security best practices.",
        report->conclusion ? report->conclusion : "The assessment has been completed."
    );
    
    return markdown;
}

// Generate HTML report
char *ks_report_generate_html(ks_report_t *report) {
    char *markdown = ks_report_generate_markdown(report);
    if (!markdown) return NULL;
    
    // Simple markdown to HTML conversion
    char *html = malloc(strlen(markdown) * 2 + 1024);
    if (!html) {
        free(markdown);
        return NULL;
    }
    
    snprintf(html, strlen(markdown) * 2 + 1024,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <title>%s</title>\n"
        "  <style>\n"
        "    body { font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }\n"
        "    h1 { color: #333; border-bottom: 2px solid #007bff; padding-bottom: 10px; }\n"
        "    h2 { color: #007bff; margin-top: 30px; }\n"
        "    .meta { color: #666; margin-bottom: 20px; }\n"
        "    .finding { background: #f8f9fa; padding: 15px; border-left: 4px solid #007bff; margin: 10px 0; }\n"
        "    .critical { border-left-color: #dc3545; }\n"
        "    .high { border-left-color: #fd7e14; }\n"
        "    .medium { border-left-color: #ffc107; }\n"
        "    .low { border-left-color: #17a2b8; }\n"
        "  </style>\n"
        "</head>\n"
        "<body>\n"
        "  <h1>%s</h1>\n"
        "  <div class=\"meta\">\n"
        "    <p><strong>Target:</strong> %s</p>\n"
        "    <p><strong>Date:</strong> %s</p>\n"
        "    <p><strong>Author:</strong> %s</p>\n"
        "  </div>\n"
        "  <hr>\n"
        "  <h2>Executive Summary</h2>\n"
        "  <p>%s</p>\n"
        "  <h2>Findings</h2>\n"
        "  %s\n"
        "</body>\n"
        "</html>",
        report->title ? report->title : "Security Report",
        report->title ? report->title : "Security Report",
        report->target ? report->target : "N/A",
        report->date ? report->date : "N/A",
        report->author ? report->author : "N/A",
        report->executive_summary ? report->executive_summary : "Summary",
        report->findings ? report->findings : "No findings"
    );
    
    free(markdown);
    return html;
}

// Generate report
int ks_report_generate(int64_t target_id, const char *format) {
    if (!shell.current_workspace) {
        fprintf(stderr, "No workspace open\n");
        return KS_ERROR;
    }
    
    printf("[*] Generating report...\n");
    
    // Create report
    ks_report_t *report = ks_report_create_new("Security Assessment Report", "example.com");
    if (!report) {
        return KS_ERROR_NOMEM;
    }
    
    // Generate in requested format
    char *content = NULL;
    char *extension = NULL;
    
    if (format && strcmp(format, "html") == 0) {
        content = ks_report_generate_html(report);
        extension = "html";
    } else {
        content = ks_report_generate_markdown(report);
        extension = "md";
    }
    
    if (!content) {
        ks_report_free(report);
        return KS_ERROR;
    }
    
    // Save to file
    char *reports_dir = ks_path_join(shell.workspace_dir, "reports");
    if (!ks_path_exists(reports_dir)) {
        ks_path_mkdir(reports_dir, 0755);
    }
    
    char filename[256];
    snprintf(filename, sizeof(filename), "report_%s.%s", report->date, extension);
    char *filepath = ks_path_join(reports_dir, filename);
    
    ks_file_write(filepath, content);
    
    printf("[+] Report generated: %s\n", filepath);
    
    free(content);
    free(reports_dir);
    free(filepath);
    ks_report_free(report);
    
    return KS_OK;
}

// Preview report
int ks_report_preview(int64_t target_id) {
    printf("Report preview not yet implemented\n");
    return KS_OK;
}

// Export report
int ks_report_export(int64_t target_id, const char *format) {
    return ks_report_generate(target_id, format);
}

// Report command handler
int ks_cmd_report(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: report <generate|preview|export> [format]\n");
        return 1;
    }
    
    if (strcmp(argv[1], "generate") == 0) {
        const char *format = argc > 2 ? argv[2] : "markdown";
        return ks_report_generate(0, format);
    }
    
    if (strcmp(argv[1], "preview") == 0) {
        return ks_report_preview(0);
    }
    
    if (strcmp(argv[1], "export") == 0) {
        const char *format = argc > 2 ? argv[2] : "markdown";
        return ks_report_export(0, format);
    }
    
    fprintf(stderr, "Unknown report command: %s\n", argv[1]);
    return 1;
}
