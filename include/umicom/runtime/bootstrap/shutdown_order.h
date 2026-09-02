/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/shutdown_order.h
 *
 * PURPOSE:
 *   Derive reverse dependency order for safe service shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SHUTDOWN_ORDER_H
#define UMICOM_RUNTIME_BOOTSTRAP_SHUTDOWN_ORDER_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap shutdown order operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_shutdown_order(const UmiBootstrapServiceGraph *graph,
                                       UmiBootstrapIdList *out_order);

#ifdef __cplusplus
}
#endif

#endif
