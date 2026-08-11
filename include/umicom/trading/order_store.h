/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_store.h
 *
 * PURPOSE:
 *   Store a bounded set of orders for deterministic simulations and tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates order store behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_STORE_H
#define INCLUDE_UMICOM_TRADING_ORDER_STORE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/order_state.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiOrderStore { UmiOrder orders[UMI_TRADING_MAX_ORDERS]; size_t count; } UmiOrderStore;
void umi_order_store_init(UmiOrderStore *store);
UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order);
UmiStatus umi_order_store_find(UmiOrderStore *store, const char *client_order_id, UmiOrder **out_order);
#ifdef __cplusplus
}
#endif
#endif
