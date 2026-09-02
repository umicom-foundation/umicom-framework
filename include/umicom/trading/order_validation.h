/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_validation.h
 *
 * PURPOSE:
 *   Apply structural and market-state checks before risk evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates order validation behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_VALIDATION_H
#define INCLUDE_UMICOM_TRADING_ORDER_VALIDATION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the order validate for market operation used by this module and its client
 * applications.
 */
UmiStatus umi_order_validate_for_market(const UmiOrderRequest *request, UmiMarketState state);
#ifdef __cplusplus
}
#endif
#endif
