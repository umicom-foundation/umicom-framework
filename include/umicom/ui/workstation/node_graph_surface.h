/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/node_graph_surface.h
 *
 * PURPOSE:
 *   Define reusable node-graph state for AI pipelines, compositor graphs, audio routing, workflow design and system topology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_NODE_GRAPH_SURFACE_H
#define UMICOM_UI_WORKSTATION_NODE_GRAPH_SURFACE_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws graph node data shared with callers of this public contract.
 */
typedef struct UmiWsGraphNode {
    char node_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    double x;
    double y;
} UmiWsGraphNode;

/**
 * Represent the ws graph edge data shared with callers of this public contract.
 */
typedef struct UmiWsGraphEdge {
    char source_id[UMI_UI_ID_CAPACITY];
    char target_id[UMI_UI_ID_CAPACITY];
} UmiWsGraphEdge;

/**
 * Represent the ws node graph surface data shared with callers of this public contract.
 */
typedef struct UmiWsNodeGraphSurface {
    char graph_id[UMI_UI_ID_CAPACITY];
    UmiWsGraphNode nodes[UMI_WS_MAX_GRAPH_NODES];
    UmiWsGraphEdge edges[UMI_WS_MAX_GRAPH_EDGES];
    size_t node_count;
    size_t edge_count;
    double zoom;
} UmiWsNodeGraphSurface;

/**
 * Initialise ws node graph surface from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ws_node_graph_surface_init(UmiWsNodeGraphSurface *graph, const char *graph_id);
/**
 * Provide the ws node graph surface add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_node_graph_surface_add_node(UmiWsNodeGraphSurface *graph,
                                             const char *node_id,
                                             const char *label,
                                             double x,
                                             double y);
/**
 * Provide the ws node graph surface add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_node_graph_surface_add_edge(UmiWsNodeGraphSurface *graph,
                                             const char *source_id,
                                             const char *target_id);

#ifdef __cplusplus
}
#endif

#endif
