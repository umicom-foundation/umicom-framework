/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_graph.c
 *
 * PURPOSE:
 *   Store service nodes and dependency edges for validation and lifecycle planning.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/service_graph.h"


#include <string.h>

void umi_bootstrap_service_graph_init(UmiBootstrapServiceGraph *graph) {
    if (graph != NULL) memset(graph, 0, sizeof(*graph));
}

const UmiBootstrapGraphNode *umi_bootstrap_service_graph_find_node(
    const UmiBootstrapServiceGraph *graph,
    const char *id) {
    size_t index;
    if (graph == NULL || id == NULL) return NULL;
    for (index = 0U; index < graph->node_count; ++index) {
        if (strcmp(graph->nodes[index].id, id) == 0) return &graph->nodes[index];
    }
    return NULL;
}

UmiStatus umi_bootstrap_service_graph_add_node(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphNode *node) {
    if (graph == NULL || node == NULL || !umi_bootstrap_id_valid(node->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_bootstrap_service_graph_find_node(graph, node->id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (graph->node_count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->nodes[graph->node_count++] = *node;
    ++graph->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_bootstrap_service_graph_add_edge(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphEdge *edge) {
    size_t index;
    if (graph == NULL || edge == NULL ||
        !umi_bootstrap_id_valid(edge->dependency_id) ||
        !umi_bootstrap_id_valid(edge->dependent_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(edge->dependency_id, edge->dependent_id) == 0) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < graph->edge_count; ++index) {
        if (strcmp(graph->edges[index].dependency_id, edge->dependency_id) == 0 &&
            strcmp(graph->edges[index].dependent_id, edge->dependent_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (graph->edge_count >= UMI_BOOTSTRAP_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->edges[graph->edge_count++] = *edge;
    ++graph->revision;
    return UMI_STATUS_OK;
}
