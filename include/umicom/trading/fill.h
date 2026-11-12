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
/* The low-level operation applies one new report; it does not retain execution
 * IDs. Use the trading workspace for idempotent replay and position publication.
 * The represented double quantity must fit the represented remaining amount.
 * No absolute tolerance permits an overfill or completes a nonempty remainder.
 * A positive report that cannot advance the stored double returns
 * CAPACITY_EXCEEDED. Other quantity/state refusals return INVALID_STATE.
 * Every failure preserves the complete order. Callers needing exact lot units
 * should use the existing trading/core integer-lot records; this API does not
 * infer an instrument's lot increment or round an external decimal quantity. */
UmiStatus umi_order_apply_execution(UmiOrder *order, const UmiExecutionReport *report);
#ifdef __cplusplus
}
#endif
#endif
