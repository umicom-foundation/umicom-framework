/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_edge.c
 *
 * PURPOSE:
 *   Implement the graph edge behavior for
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
 * File: src/runtime/bootstrap/graph_edge.c
 *
 * PURPOSE:
 *   Construct directed dependency edges for service-graph analysis.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_edge.h"


#include <string.h>

/*
 * Initialise bootstrap graph edge from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_bootstrap_graph_edge_init(UmiBootstrapGraphEdge *edge,
                                        const char *dependency_id,
                                        const char *dependent_id,
                                        bool required) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL || !umi_bootstrap_id_valid(dependency_id) ||
        !umi_bootstrap_id_valid(dependent_id) ||
        strcmp(dependency_id, dependent_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(edge, 0, sizeof(*edge));
    status = umi_bootstrap_copy_text(edge->dependency_id, sizeof(edge->dependency_id),
                                     dependency_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(edge->dependent_id, sizeof(edge->dependent_id),
                                     dependent_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    edge->required = required;
    return UMI_STATUS_OK;
}
