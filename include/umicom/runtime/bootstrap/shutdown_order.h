/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/shutdown_order.h
 *
 * PURPOSE:
 *   Derive reverse dependency order for safe service shutdown.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SHUTDOWN_ORDER_H
#define UMICOM_RUNTIME_BOOTSTRAP_SHUTDOWN_ORDER_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_shutdown_order(const UmiBootstrapServiceGraph *graph,
                                       UmiBootstrapIdList *out_order);

#ifdef __cplusplus
}
#endif

#endif
