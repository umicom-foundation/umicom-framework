/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/node_graph_surface.c
 *
 * PURPOSE:
 *   Implement reusable node-graph state for AI pipelines, compositor graphs, audio routing, workflow design and system topology.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/node_graph_surface.h"

static bool node_exists(const UmiWsNodeGraphSurface *graph, const char *node_id) {
    size_t index;
    for (index = 0U; index < graph->node_count; ++index) if (strcmp(graph->nodes[index].node_id, node_id) == 0) return true;
    return false;
}

UmiStatus umi_ws_node_graph_surface_init(UmiWsNodeGraphSurface *graph, const char *graph_id) {
    if (graph == NULL || !umi_ws_id_valid(graph_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *graph = (UmiWsNodeGraphSurface){0};
    graph->zoom = 1.0;
    return umi_ws_copy_text(graph->graph_id, sizeof(graph->graph_id), graph_id);
}

UmiStatus umi_ws_node_graph_surface_add_node(UmiWsNodeGraphSurface *graph,
                                             const char *node_id,
                                             const char *label,
                                             double x,
                                             double y) {
    UmiWsGraphNode *node;
    if (graph == NULL || !umi_ws_id_valid(node_id) || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (graph->node_count >= UMI_WS_MAX_GRAPH_NODES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (node_exists(graph, node_id)) return UMI_STATUS_ALREADY_EXISTS;
    node = &graph->nodes[graph->node_count++];
    *node = (UmiWsGraphNode){0};
    if (umi_ws_copy_text(node->node_id, sizeof(node->node_id), node_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(node->label, sizeof(node->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    node->x = x;
    node->y = y;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_node_graph_surface_add_edge(UmiWsNodeGraphSurface *graph,
                                             const char *source_id,
                                             const char *target_id) {
    UmiWsGraphEdge *edge;
    if (graph == NULL || !umi_ws_id_valid(source_id) || !umi_ws_id_valid(target_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!node_exists(graph, source_id) || !node_exists(graph, target_id)) return UMI_STATUS_NOT_FOUND;
    if (graph->edge_count >= UMI_WS_MAX_GRAPH_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    edge = &graph->edges[graph->edge_count++];
    *edge = (UmiWsGraphEdge){0};
    if (umi_ws_copy_text(edge->source_id, sizeof(edge->source_id), source_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(edge->target_id, sizeof(edge->target_id), target_id);
}
