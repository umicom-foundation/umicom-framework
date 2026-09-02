/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/node_graph_surface.c
 *
 * PURPOSE:
 *   Implement reusable node-graph state for AI pipelines, compositor graphs, audio routing, workflow design and system topology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/node_graph_surface.h"

/* Provide the node exists operation used by this module and its client applications. */
static bool node_exists(const UmiWsNodeGraphSurface *graph, const char *node_id) {
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(graph->nodes[index].node_id, node_id) == 0) return true;
    return false;
}

/*
 * Initialise ws node graph surface from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ws_node_graph_surface_init(UmiWsNodeGraphSurface *graph, const char *graph_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || !umi_ws_id_valid(graph_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *graph = (UmiWsNodeGraphSurface){0};
    graph->zoom = 1.0;
    return umi_ws_copy_text(graph->graph_id, sizeof(graph->graph_id), graph_id);
}

/*
 * Provide the ws node graph surface add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_node_graph_surface_add_node(UmiWsNodeGraphSurface *graph,
                                             const char *node_id,
                                             const char *label,
                                             double x,
                                             double y) {
    UmiWsGraphNode *node;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || !umi_ws_id_valid(node_id) || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->node_count >= UMI_WS_MAX_GRAPH_NODES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (node_exists(graph, node_id)) return UMI_STATUS_ALREADY_EXISTS;
    node = &graph->nodes[graph->node_count++];
    *node = (UmiWsGraphNode){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(node->node_id, sizeof(node->node_id), node_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(node->label, sizeof(node->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    node->x = x;
    node->y = y;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws node graph surface add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_node_graph_surface_add_edge(UmiWsNodeGraphSurface *graph,
                                             const char *source_id,
                                             const char *target_id) {
    UmiWsGraphEdge *edge;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || !umi_ws_id_valid(source_id) || !umi_ws_id_valid(target_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!node_exists(graph, source_id) || !node_exists(graph, target_id)) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->edge_count >= UMI_WS_MAX_GRAPH_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    edge = &graph->edges[graph->edge_count++];
    *edge = (UmiWsGraphEdge){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(edge->source_id, sizeof(edge->source_id), source_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(edge->target_id, sizeof(edge->target_id), target_id);
}
