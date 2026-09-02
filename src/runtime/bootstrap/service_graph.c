/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_graph.c
 *
 * PURPOSE:
 *   Implement the service graph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_graph.c
 *
 * PURPOSE:
 *   Store service nodes and dependency edges for validation and lifecycle planning.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/service_graph.h"


#include <string.h>

/*
 * Initialise bootstrap service graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_service_graph_init(UmiBootstrapServiceGraph *graph) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph != NULL) memset(graph, 0, sizeof(*graph));
}

/*
 * Provide the bootstrap service graph find node operation used by this module and its
 * client applications.
 */
const UmiBootstrapGraphNode *umi_bootstrap_service_graph_find_node(
    const UmiBootstrapServiceGraph *graph,
    const char *id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(graph->nodes[index].id, id) == 0) return &graph->nodes[index];
    }
    return NULL;
}

/*
 * Provide the bootstrap service graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_service_graph_add_node(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphNode *node) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || node == NULL || !umi_bootstrap_id_valid(node->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_bootstrap_service_graph_find_node(graph, node->id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->node_count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->nodes[graph->node_count++] = *node;
    ++graph->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the bootstrap service graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_service_graph_add_edge(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphEdge *edge) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || edge == NULL ||
        !umi_bootstrap_id_valid(edge->dependency_id) ||
        !umi_bootstrap_id_valid(edge->dependent_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(edge->dependency_id, edge->dependent_id) == 0) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->edge_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(graph->edges[index].dependency_id, edge->dependency_id) == 0 &&
            strcmp(graph->edges[index].dependent_id, edge->dependent_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->edge_count >= UMI_BOOTSTRAP_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->edges[graph->edge_count++] = *edge;
    ++graph->revision;
    return UMI_STATUS_OK;
}
