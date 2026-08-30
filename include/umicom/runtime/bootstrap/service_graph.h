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


void umi_bootstrap_service_graph_init(UmiBootstrapServiceGraph *graph);
UmiStatus umi_bootstrap_service_graph_add_node(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphNode *node);
UmiStatus umi_bootstrap_service_graph_add_edge(
    UmiBootstrapServiceGraph *graph,
    const UmiBootstrapGraphEdge *edge);
const UmiBootstrapGraphNode *umi_bootstrap_service_graph_find_node(
    const UmiBootstrapServiceGraph *graph,
    const char *id);

#ifdef __cplusplus
}
#endif

#endif
