/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/shutdown_order.c
 *
 * PURPOSE:
 *   Implement the shutdown order behavior for
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
 * File: src/runtime/bootstrap/shutdown_order.c
 *
 * PURPOSE:
 *   Derive reverse dependency order for safe service shutdown.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/shutdown_order.h"
#include "umicom/runtime/bootstrap/graph_ordering.h"


#include <string.h>
/*
 * Provide the bootstrap shutdown order operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_shutdown_order(const UmiBootstrapServiceGraph *graph,
                                       UmiBootstrapIdList *out_order) {
    UmiBootstrapIdList startup;
    size_t i;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_bootstrap_graph_order(graph, &startup);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    memset(out_order, 0, sizeof(*out_order));
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = startup.count; i > 0U; --i) {
        (void)umi_bootstrap_copy_text(out_order->ids[out_order->count++],
            UMI_BOOTSTRAP_ID_CAPACITY, startup.ids[i - 1U]);
    }
    return UMI_STATUS_OK;
}
