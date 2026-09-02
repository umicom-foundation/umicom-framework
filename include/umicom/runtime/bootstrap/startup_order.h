/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/startup_order.h
 *
 * PURPOSE:
 *   Derive service start order from the dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTUP_ORDER_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTUP_ORDER_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap startup order operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_startup_order(const UmiBootstrapServiceGraph *graph,
                                      UmiBootstrapIdList *out_order);

#ifdef __cplusplus
}
#endif

#endif
