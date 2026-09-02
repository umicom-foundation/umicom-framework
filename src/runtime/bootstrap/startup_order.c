/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/startup_order.c
 *
 * PURPOSE:
 *   Implement the startup order behavior for
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
 * File: src/runtime/bootstrap/startup_order.c
 *
 * PURPOSE:
 *   Derive service start order from the dependency graph.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/startup_order.h"
#include "umicom/runtime/bootstrap/graph_ordering.h"


/*
 * Provide the bootstrap startup order operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_startup_order(const UmiBootstrapServiceGraph *graph,
                                      UmiBootstrapIdList *out_order) {
    return umi_bootstrap_graph_order(graph, out_order);
}
