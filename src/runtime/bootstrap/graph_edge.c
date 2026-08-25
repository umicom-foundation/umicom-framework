/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_edge.c
 *
 * PURPOSE:
 *   Construct directed dependency edges for service-graph analysis.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_edge.h"


#include <string.h>

UmiStatus umi_bootstrap_graph_edge_init(UmiBootstrapGraphEdge *edge,
                                        const char *dependency_id,
                                        const char *dependent_id,
                                        bool required) {
    UmiStatus status;
    if (edge == NULL || !umi_bootstrap_id_valid(dependency_id) ||
        !umi_bootstrap_id_valid(dependent_id) ||
        strcmp(dependency_id, dependent_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(edge, 0, sizeof(*edge));
    status = umi_bootstrap_copy_text(edge->dependency_id, sizeof(edge->dependency_id),
                                     dependency_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(edge->dependent_id, sizeof(edge->dependent_id),
                                     dependent_id);
    if (status != UMI_STATUS_OK) return status;
    edge->required = required;
    return UMI_STATUS_OK;
}
