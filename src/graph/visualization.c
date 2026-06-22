#include "kalishell/kalishell.h"

// Node type names
static const char *node_type_names[] = {
    "domain", "ip", "url", "cidr", "subdomain", 
    "endpoint", "technology", "screenshot", "certificate"
};

// Forward declarations
void ks_graph_print_tree_node(ks_graph_node_t *node, int indent);
void ks_graph_print_colored_children(ks_graph_node_t *node, const char *prefix, int depth);
void ks_graph_export_node_json(ks_graph_node_t *node, FILE *f, int indent, int is_last);

// Get node type name
const char *ks_graph_get_type_name(ks_asset_type_t type) {
    if (type >= 0 && type <= ASSET_CERTIFICATE) {
        return node_type_names[type];
    }
    return "unknown";
}

// Print graph with colors
int ks_graph_visualize彩色(ks_graph_t *graph) {
    if (!graph || !graph->root) {
        printf("Graph is empty\n");
        return KS_OK;
    }
    
    printf("\n" COLOR_BOLD "Asset Graph" COLOR_RESET "\n\n");
    
    // Print root with color
    printf(COLOR_CYAN "🎯 %s" COLOR_RESET "\n", graph->root->value);
    
    // Print children with colors
    ks_graph_print_colored_children(graph->root, "  ", 1);
    
    printf("\n" COLOR_BOLD "Statistics:" COLOR_RESET "\n");
    printf("  Nodes: " COLOR_GREEN "%d" COLOR_RESET "\n", graph->node_count);
    printf("  Edges: " COLOR_GREEN "%d" COLOR_RESET "\n", graph->edge_count);
    
    return KS_OK;
}

// Print children with colors recursively
void ks_graph_print_colored_children(ks_graph_node_t *node, const char *prefix, int depth) {
    if (!node) return;
    
    for (int i = 0; i < node->child_count; i++) {
        ks_graph_node_t *child = node->children[i];
        int is_last = (i == node->child_count - 1);
        
        // Choose color based on node type
        const char *color;
        const char *icon;
        switch (child->type) {
            case ASSET_SUBDOMAIN:
                color = COLOR_BLUE;
                icon = "🌐";
                break;
            case ASSET_ENDPOINT:
                color = COLOR_GREEN;
                icon = "🔗";
                break;
            case ASSET_TECHNOLOGY:
                color = COLOR_YELLOW;
                icon = "⚙️";
                break;
            case ASSET_SCREENSHOT:
                color = COLOR_MAGENTA;
                icon = "📸";
                break;
            case ASSET_CERTIFICATE:
                color = COLOR_CYAN;
                icon = "🔒";
                break;
            default:
                color = COLOR_WHITE;
                icon = "📌";
                break;
        }
        
        // Print node
        printf("%s%s%s%s %s" COLOR_RESET "\n", 
               prefix,
               is_last ? "└── " : "├── ",
               color, icon, child->value);
        
        // Recursively print children
        char new_prefix[1024];
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
                 is_last ? "    " : "│   ");
        
        ks_graph_print_colored_children(child, new_prefix, depth + 1);
    }
}

// Print graph as tree (alternative format)
int ks_graph_print_tree(ks_graph_t *graph) {
    if (!graph || !graph->root) {
        printf("Graph is empty\n");
        return KS_OK;
    }
    
    printf("\n" COLOR_BOLD "🌳 Asset Tree" COLOR_RESET "\n\n");
    
    // Print root
    printf("🎯 " COLOR_BOLD "%s" COLOR_RESET "\n", graph->root->value);
    
    // Print with indentation
    ks_graph_print_tree_node(graph->root, 0);
    
    return KS_OK;
}

// Print tree node with indentation
void ks_graph_print_tree_node(ks_graph_node_t *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < node->child_count; i++) {
        ks_graph_node_t *child = node->children[i];
        
        // Print indentation
        for (int j = 0; j < indent; j++) {
            printf("  ");
        }
        
        // Choose color based on type
        const char *color;
        switch (child->type) {
            case ASSET_SUBDOMAIN: color = COLOR_BLUE; break;
            case ASSET_ENDPOINT: color = COLOR_GREEN; break;
            case ASSET_TECHNOLOGY: color = COLOR_YELLOW; break;
            default: color = COLOR_WHITE; break;
        }
        
        printf("├── %s%s" COLOR_RESET "\n", color, child->value);
        
        // Recursively print children
        ks_graph_print_tree_node(child, indent + 1);
    }
}

// Export graph to JSON
int ks_graph_export_json(ks_graph_t *graph, const char *filename) {
    if (!graph) return KS_ERROR_INVALID;
    
    FILE *f = fopen(filename, "w");
    if (!f) return KS_ERROR_IO;
    
    fprintf(f, "{\n");
    fprintf(f, "  \"nodes\": [\n");
    
    // Export nodes recursively
    if (graph->root) {
        ks_graph_export_node_json(graph->root, f, 2, 1);
    }
    
    fprintf(f, "  ],\n");
    fprintf(f, "  \"edges\": [\n");
    
    // Export edges
    ks_graph_edge_t *edge = graph->edges;
    int first = 1;
    while (edge) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    {\"source\": %lld, \"target\": %lld, \"relationship\": \"%s\"}",
                (long long)edge->source_id, (long long)edge->target_id, edge->relationship);
        first = 0;
        edge = edge->next;
    }
    
    fprintf(f, "\n  ]\n");
    fprintf(f, "}\n");
    
    fclose(f);
    printf("[+] Graph exported to %s\n", filename);
    
    return KS_OK;
}

// Export node to JSON
void ks_graph_export_node_json(ks_graph_node_t *node, FILE *f, int indent, int is_last) {
    if (!node || !f) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) fprintf(f, "  ");
    
    fprintf(f, "{");
    fprintf(f, "\"id\": %lld, ", (long long)node->id);
    fprintf(f, "\"value\": \"%s\", ", node->value);
    fprintf(f, "\"type\": \"%s\"", ks_graph_get_type_name(node->type));
    fprintf(f, "}");
    
    // Export children
    for (int i = 0; i < node->child_count; i++) {
        fprintf(f, ",\n");
        ks_graph_export_node_json(node->children[i], f, indent, i == node->child_count - 1);
    }
}
