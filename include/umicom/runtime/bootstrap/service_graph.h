/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/service_graph.h
 *
 * PURPOSE:
 *   Store service nodes and dependency edges for validation and lifecycle planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SERVICE_GRAPH_H
#define UMICOM_RUNTIME_BOOTSTRAP_SERVICE_GRAPH_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap service graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_service_graph_init(UmiBootstrapServiceGraph *graph);
/**
 * Provide the bootstrap service graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_service_graph_add_node(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphNode *node);
/**
 * Provide the bootstrap service graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_service_graph_add_edge(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphEdge *edge);
/**
 * Provide the bootstrap service graph find node operation used by this module and its
 * client applications.
 */
const UmiBootstrapGraphNode *umi_bootstrap_service_graph_find_node(
    const UmiBootstrapServiceGraph *graph,
    const char *id);

#ifdef __cplusplus
}
#endif

#endif
