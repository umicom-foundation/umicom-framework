/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_node.c
 *
 * PURPOSE:
 *   Implement the graph node behavior for
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
 * File: src/runtime/bootstrap/graph_node.c
 *
 * PURPOSE:
 *   Construct service-graph nodes with explicit priority and enabled state.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_node.h"


#include <string.h>

/*
 * Initialise bootstrap graph node from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_bootstrap_graph_node_init(UmiBootstrapGraphNode *node,
                                        const char *id,
                                        int32_t priority,
                                        bool enabled) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || !umi_bootstrap_id_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(node, 0, sizeof(*node));
    status = umi_bootstrap_copy_text(node->id, sizeof(node->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    node->priority = priority;
    node->enabled = enabled;
    return UMI_STATUS_OK;
}
