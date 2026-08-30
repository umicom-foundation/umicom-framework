/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/graph_ordering.h
 *
 * PURPOSE:
 *   Produce deterministic topological service order from dependency graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_GRAPH_ORDERING_H
#define UMICOM_RUNTIME_BOOTSTRAP_GRAPH_ORDERING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_graph_order(const UmiBootstrapServiceGraph *graph,
                                    UmiBootstrapIdList *out_order);

#ifdef __cplusplus
}
#endif

#endif
