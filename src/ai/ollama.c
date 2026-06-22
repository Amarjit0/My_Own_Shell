#include "gupt/gupt.h"

// Ollama client stub
typedef struct {
    char *host;
    int port;
    char *model;
} ks_ollama_config_t;

static ks_ollama_config_t ollama_config = {
    .host = "localhost",
    .port = 11434,
    .model = "llama3"
};

int ks_ollama_init(const char *host, int port, const char *model) {
    if (host) ollama_config.host = strdup(host);
    if (port > 0) ollama_config.port = port;
    if (model) ollama_config.model = strdup(model);
    
    return KS_OK;
}

int ks_ollama_generate(const char *prompt, char *output, size_t output_size) {
    // TODO: Call Ollama API
    printf("[*] Ollama integration not yet implemented\n");
    return KS_OK;
}
