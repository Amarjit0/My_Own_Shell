#include "gupt/gupt.h"

extern ks_shell_t shell;

static ks_graph_t *current_graph = NULL;

// Create a new graph node
ks_graph_node_t *ks_graph_node_create(int64_t asset_id, const char *value, ks_asset_type_t type) {
    ks_graph_node_t *node = calloc(1, sizeof(ks_graph_node_t));
    if (!node) return NULL;
    
    node->id = asset_id;
    node->asset_id = asset_id;
    node->value = strdup(value);
    node->type = type;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    
    return node;
}

// Free a graph node
void ks_graph_node_free(ks_graph_node_t *node) {
    if (!node) return;
    
    free(node->value);
    for (int i = 0; i < node->child_count; i++) {
        ks_graph_node_free(node->children[i]);
    }
    free(node->children);
    free(node);
}

// Add a child to a node
int ks_graph_node_add_child(ks_graph_node_t *parent, ks_graph_node_t *child) {
    if (!parent || !child) return KS_ERROR_INVALID;
    
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity = parent->child_capacity ? parent->child_capacity * 2 : 8;
        parent->children = realloc(parent->children, sizeof(ks_graph_node_t *) * parent->child_capacity);
    }
    
    parent->children[parent->child_count++] = child;
    return KS_OK;
}

// Create a new graph
ks_graph_t *ks_graph_create(void) {
    ks_graph_t *graph = calloc(1, sizeof(ks_graph_t));
    if (!graph) return NULL;
    
    graph->root = NULL;
    graph->edges = NULL;
    graph->node_count = 0;
    graph->edge_count = 0;
    
    return graph;
}

// Free a graph
void ks_graph_free(ks_graph_t *graph) {
    if (!graph) return;
    
    ks_graph_node_free(graph->root);
    
    ks_graph_edge_t *edge = graph->edges;
    while (edge) {
        ks_graph_edge_t *next = edge->next;
        free(edge->relationship);
        free(edge);
        edge = next;
    }
    
    free(graph);
}

// Add an edge to the graph
int ks_graph_add_edge(ks_graph_t *graph, int64_t source_id, int64_t target_id, const char *relationship) {
    if (!graph || !relationship) return KS_ERROR_INVALID;
    
    ks_graph_edge_t *edge = calloc(1, sizeof(ks_graph_edge_t));
    if (!edge) return KS_ERROR_NOMEM;
    
    edge->source_id = source_id;
    edge->target_id = target_id;
    edge->relationship = strdup(relationship);
    edge->next = graph->edges;
    
    graph->edges = edge;
    graph->edge_count++;
    
    return KS_OK;
}

// Build graph from database
int ks_graph_build_from_db(int64_t target_id) {
    if (!shell.current_workspace) {
        return KS_ERROR;
    }
    
    // Free existing graph
    if (current_graph) {
        ks_graph_free(current_graph);
    }
    
    current_graph = ks_graph_create();
    if (!current_graph) return KS_ERROR_NOMEM;
    
    // TODO: Load assets from database and build graph
    // For now, create a placeholder structure
    
    printf("[*] Building asset graph...\n");
    
    return KS_OK;
}

// Find a node by value
ks_graph_node_t *ks_graph_find_node(ks_graph_t *graph, const char *value) {
    if (!graph || !graph->root || !value) return NULL;
    
    // BFS search
    ks_graph_node_t **queue = malloc(sizeof(ks_graph_node_t *) * 1024);
    int front = 0, back = 0;
    
    queue[back++] = graph->root;
    
    while (front < back) {
        ks_graph_node_t *node = queue[front++];
        
        if (strcmp(node->value, value) == 0) {
            free(queue);
            return node;
        }
        
        for (int i = 0; i < node->child_count; i++) {
            queue[back++] = node->children[i];
        }
    }
    
    free(queue);
    return NULL;
}

// Visualize graph
int ks_graph_visualize(ks_graph_t *graph) {
    if (!graph || !graph->root) {
        printf("Graph is empty\n");
        return KS_OK;
    }
    
    printf("Asset Graph:\n\n");
    
    // Print root
    printf("%s\n", graph->root->value);
    
    // Print children recursively
    ks_graph_print_children(graph->root, "", 1);
    
    printf("\nStatistics:\n");
    printf("  Nodes: %d\n", graph->node_count);
    printf("  Edges: %d\n", graph->edge_count);
    
    return KS_OK;
}

// Print children recursively
void ks_graph_print_children(ks_graph_node_t *node, const char *prefix, int is_last) {
    if (!node) return;
    
    for (int i = 0; i < node->child_count; i++) {
        ks_graph_node_t *child = node->children[i];
        
        printf("%s%s%s\n", prefix, 
               (i == node->child_count - 1) ? "└── " : "├── ",
               child->value);
        
        // Recursively print children
        char new_prefix[1024];
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
                 (i == node->child_count - 1) ? "    " : "│   ");
        
        ks_graph_print_children(child, new_prefix, i == node->child_count - 1);
    }
}

// Export graph to DOT format
int ks_graph_export_dot(ks_graph_t *graph, const char *filename) {
    if (!graph) return KS_ERROR_INVALID;
    
    FILE *f = fopen(filename, "w");
    if (!f) return KS_ERROR_IO;
    
    fprintf(f, "digraph AssetGraph {\n");
    fprintf(f, "  rankdir=TB;\n");
    fprintf(f, "  node [shape=box];\n\n");
    
    // Export nodes recursively
    if (graph->root) {
        ks_graph_export_node_dot(graph->root, f, NULL);
    }
    
    fprintf(f, "}\n");
    
    fclose(f);
    printf("[+] Graph exported to %s\n", filename);
    
    return KS_OK;
}

// Export node to DOT format
void ks_graph_export_node_dot(ks_graph_node_t *node, FILE *f, const char *parent_id) {
    if (!node || !f) return;
    
    char node_id[64];
    snprintf(node_id, sizeof(node_id), "node_%lld", (long long)node->id);
    
    // Print node
    fprintf(f, "  %s [label=\"%s\"];\n", node_id, node->value);
    
    // Print edge from parent
    if (parent_id) {
        fprintf(f, "  %s -> %s;\n", parent_id, node_id);
    }
    
    // Export children
    for (int i = 0; i < node->child_count; i++) {
        ks_graph_export_node_dot(node->children[i], f, node_id);
    }
}

// Get current graph
ks_graph_t *ks_graph_get_current(void) {
    return current_graph;
}

// Graph command handler
int ks_cmd_graph(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: graph <show|export|stats>\n");
        return 1;
    }
    
    if (strcmp(argv[1], "show") == 0) {
        if (!current_graph) {
            // Build graph from current target
            if (ks_graph_build_from_db(0) != KS_OK) {
                fprintf(stderr, "Failed to build graph\n");
                return 1;
            }
        }
        return ks_graph_visualize(current_graph);
    }
    
    if (strcmp(argv[1], "export") == 0) {
        if (!current_graph) {
            fprintf(stderr, "No graph to export\n");
            return 1;
        }
        
        const char *filename = argc > 2 ? argv[2] : "graph.dot";
        return ks_graph_export_dot(current_graph, filename);
    }
    
    if (strcmp(argv[1], "stats") == 0) {
        if (!current_graph) {
            printf("No graph loaded\n");
            return 0;
        }
        
        printf("Graph Statistics:\n");
        printf("  Nodes: %d\n", current_graph->node_count);
        printf("  Edges: %d\n", current_graph->edge_count);
        return 0;
    }
    
    fprintf(stderr, "Unknown graph command: %s\n", argv[1]);
    return 1;
}
