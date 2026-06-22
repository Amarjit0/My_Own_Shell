#include "gupt/gupt.h"

// AI configuration
typedef struct {
    char *provider;
    char *host;
    int port;
    char *model;
    bool enabled;
} ks_ai_config_t;

static ks_ai_config_t ai_config = {
    .provider = NULL,
    .host = NULL,
    .port = 11434,
    .model = NULL,
    .enabled = false
};

// Initialize AI system
int ks_ai_init(void) {
    // Load config from gupt.yaml
    const char *provider = ks_config_get("ai.provider");
    const char *host = ks_config_get("ai.host");
    const char *model = ks_config_get("ai.model");
    int port = 11434;
    
    const char *port_str = ks_config_get("ai.port");
    if (port_str) {
        port = atoi(port_str);
    }
    
    if (provider) ai_config.provider = strdup(provider);
    if (host) ai_config.host = strdup(host);
    if (model) ai_config.model = strdup(model);
    ai_config.port = port;
    
    const char *enabled = ks_config_get("ai.enabled");
    if (enabled && strcmp(enabled, "true") == 0) {
        ai_config.enabled = true;
    }
    
    return KS_OK;
}

// Cleanup AI system
void ks_ai_cleanup(void) {
    free(ai_config.provider);
    free(ai_config.host);
    free(ai_config.model);
}

// Enable/disable AI
void ks_ai_set_enabled(bool enabled) {
    ai_config.enabled = enabled;
}

// Check if AI is enabled
bool ks_ai_is_enabled(void) {
    return ai_config.enabled;
}

// Analyze input using AI
int ks_ai_analyze(const char *input, char *output, size_t output_size) {
    if (!ai_config.enabled || !ai_config.provider) {
        snprintf(output, output_size, "AI not configured");
        return KS_OK;
    }
    
    // TODO: Implement AI analysis based on provider
    if (strcmp(ai_config.provider, "ollama") == 0) {
        return ks_ollama_generate(input, output, output_size);
    }
    
    snprintf(output, output_size, "Unknown AI provider: %s", ai_config.provider);
    return KS_OK;
}

// Get AI suggestions
int ks_ai_suggest(const char *context, char *output, size_t output_size) {
    if (!ai_config.enabled) {
        snprintf(output, output_size, "AI not enabled");
        return KS_OK;
    }
    
    char prompt[4096];
    snprintf(prompt, sizeof(prompt),
        "Based on the following context, suggest next steps for security testing:\n\n%s",
        context);
    
    return ks_ai_analyze(prompt, output, output_size);
}

// AI command handler
int ks_cmd_ai(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: ai <analyze|suggest|enable|disable|status>\n");
        return 1;
    }
    
    if (strcmp(argv[1], "analyze") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: ai analyze <input>\n");
            return 1;
        }
        
        char output[4096] = {0};
        ks_ai_analyze(argv[2], output, sizeof(output));
        printf("%s\n", output);
        return 0;
    }
    
    if (strcmp(argv[1], "suggest") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: ai suggest <context>\n");
            return 1;
        }
        
        char output[4096] = {0};
        ks_ai_suggest(argv[2], output, sizeof(output));
        printf("%s\n", output);
        return 0;
    }
    
    if (strcmp(argv[1], "enable") == 0) {
        ks_ai_set_enabled(true);
        printf("[+] AI enabled\n");
        return 0;
    }
    
    if (strcmp(argv[1], "disable") == 0) {
        ks_ai_set_enabled(false);
        printf("[-] AI disabled\n");
        return 0;
    }
    
    if (strcmp(argv[1], "status") == 0) {
        printf("AI Status:\n");
        printf("  Enabled: %s\n", ai_config.enabled ? "yes" : "no");
        printf("  Provider: %s\n", ai_config.provider ? ai_config.provider : "not set");
        printf("  Host: %s\n", ai_config.host ? ai_config.host : "not set");
        printf("  Port: %d\n", ai_config.port);
        printf("  Model: %s\n", ai_config.model ? ai_config.model : "not set");
        return 0;
    }
    
    fprintf(stderr, "Unknown AI command: %s\n", argv[1]);
    return 1;
}
