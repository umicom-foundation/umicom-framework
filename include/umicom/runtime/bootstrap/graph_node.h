/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/graph_node.h
 *
 * PURPOSE:
 *   Construct service-graph nodes with explicit priority and enabled state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_GRAPH_NODE_H
#define UMICOM_RUNTIME_BOOTSTRAP_GRAPH_NODE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap graph node from caller-provided values so later operations receive
 * a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_graph_node_init(UmiBootstrapGraphNode *node,
                                        const char *id,
                                        int32_t priority,
                                        bool enabled);

#ifdef __cplusplus
}
#endif

#endif
