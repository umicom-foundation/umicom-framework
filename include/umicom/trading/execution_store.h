/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/execution_store.h
 *
 * PURPOSE:
 *   Maintain a bounded in-memory execution history for deterministic tests and replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates execution store behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_EXECUTION_STORE_H
#define INCLUDE_UMICOM_TRADING_EXECUTION_STORE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiExecutionStore { UmiExecutionReport reports[UMI_TRADING_MAX_ORDERS]; size_t count; } UmiExecutionStore;
void umi_execution_store_init(UmiExecutionStore *store);
UmiStatus umi_execution_store_add(UmiExecutionStore *store, const UmiExecutionReport *report);
#ifdef __cplusplus
}
#endif
#endif
