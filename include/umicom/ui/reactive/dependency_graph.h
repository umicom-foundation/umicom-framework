/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/dependency_graph.h
 *
 * PURPOSE:
 *   Store bounded dependency nodes and directed edges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEPENDENCY_GRAPH_H
#define UMICOM_UI_REACTIVE_DEPENDENCY_GRAPH_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive dependency graph data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveDependencyGraph {
    char node_ids[UMI_UI_REACTIVE_MAX_ITEMS][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t node_count;
    uint16_t from_index[UMI_UI_REACTIVE_MAX_EDGES];
    uint16_t to_index[UMI_UI_REACTIVE_MAX_EDGES];
    size_t edge_count;
} UmiUiReactiveDependencyGraph;
/**
 * Initialise ui reactive dependency graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_dependency_graph_init(UmiUiReactiveDependencyGraph *item);
/**
 * Provide the ui reactive dependency graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_dependency_graph_add_node(UmiUiReactiveDependencyGraph *item,const char *id,uint16_t *out_index);
/**
 * Provide the ui reactive dependency graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_dependency_graph_add_edge(UmiUiReactiveDependencyGraph *item,uint16_t from,uint16_t to);
#ifdef __cplusplus
}
#endif
#endif
