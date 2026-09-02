/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_store.h
 *
 * PURPOSE:
 *   Store a bounded set of orders for deterministic simulations and tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the order store data shared with callers of this public contract.
 */
typedef struct UmiOrderStore { UmiOrder orders[UMI_TRADING_MAX_ORDERS]; size_t count; } UmiOrderStore;
/**
 * Initialise order store from caller-provided values so later operations receive a known
 * state.
 */
void umi_order_store_init(UmiOrderStore *store);
/**
 * Add order store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order);
/**
 * Find order store while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_order_store_find(UmiOrderStore *store, const char *client_order_id, UmiOrder **out_order);
#ifdef __cplusplus
}
#endif
#endif
