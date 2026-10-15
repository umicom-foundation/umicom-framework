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
#include "../../base/value_construction_internal.h"


#include <string.h>

/*
 * Initialise bootstrap graph edge from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_graph_edge_init_build_value(UmiBootstrapGraphEdge *edge,
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_graph_edge_init, UmiBootstrapGraphEdge, edge,
    (UmiBootstrapGraphEdge *edge, const char *dependency_id, const char *dependent_id, bool required),
    umi_bootstrap_graph_edge_init_build_value(candidate, dependency_id, dependent_id, required))
