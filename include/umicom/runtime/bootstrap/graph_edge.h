/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/graph_edge.h
 *
 * PURPOSE:
 *   Construct directed dependency edges for service-graph analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_GRAPH_EDGE_H
#define UMICOM_RUNTIME_BOOTSTRAP_GRAPH_EDGE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap graph edge from caller-provided values so later operations receive
 * a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_graph_edge_init(UmiBootstrapGraphEdge *edge,
                                        const char *dependency_id,
                                        const char *dependent_id,
                                        bool required);

#ifdef __cplusplus
}
#endif

#endif
