/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fill.h
 *
 * PURPOSE:
 *   Apply an execution report to an order's fill state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates fill behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FILL_H
#define INCLUDE_UMICOM_TRADING_FILL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the order apply execution operation used by this module and its client
 * applications.
 */
UmiStatus umi_order_apply_execution(UmiOrder *order, const UmiExecutionReport *report);
#ifdef __cplusplus
}
#endif
#endif
