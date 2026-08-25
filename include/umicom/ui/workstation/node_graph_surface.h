/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/node_graph_surface.h
 *
 * PURPOSE:
 *   Define reusable node-graph state for AI pipelines, compositor graphs, audio routing, workflow design and system topology.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_NODE_GRAPH_SURFACE_H
#define UMICOM_UI_WORKSTATION_NODE_GRAPH_SURFACE_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsGraphNode {
    char node_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    double x;
    double y;
} UmiWsGraphNode;

typedef struct UmiWsGraphEdge {
    char source_id[UMI_UI_ID_CAPACITY];
    char target_id[UMI_UI_ID_CAPACITY];
} UmiWsGraphEdge;

typedef struct UmiWsNodeGraphSurface {
    char graph_id[UMI_UI_ID_CAPACITY];
    UmiWsGraphNode nodes[UMI_WS_MAX_GRAPH_NODES];
    UmiWsGraphEdge edges[UMI_WS_MAX_GRAPH_EDGES];
    size_t node_count;
    size_t edge_count;
    double zoom;
} UmiWsNodeGraphSurface;

UmiStatus umi_ws_node_graph_surface_init(UmiWsNodeGraphSurface *graph, const char *graph_id);
UmiStatus umi_ws_node_graph_surface_add_node(UmiWsNodeGraphSurface *graph,
                                             const char *node_id,
                                             const char *label,
                                             double x,
                                             double y);
UmiStatus umi_ws_node_graph_surface_add_edge(UmiWsNodeGraphSurface *graph,
                                             const char *source_id,
                                             const char *target_id);

#ifdef __cplusplus
}
#endif

#endif
