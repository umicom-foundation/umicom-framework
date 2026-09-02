/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/bar.h
 *
 * PURPOSE:
 *   Validate OHLCV bars and calculate range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates bar behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_BAR_H
#define INCLUDE_UMICOM_TRADING_BAR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that bar satisfies its contract before another service relies on it.
 */
int umi_bar_valid(const UmiBar *bar);
/**
 * Provide the bar range operation used by this module and its client applications.
 */
double umi_bar_range(const UmiBar *bar);
/**
 * Provide the bar typical price operation used by this module and its client applications.
 */
double umi_bar_typical_price(const UmiBar *bar);
#ifdef __cplusplus
}
#endif
#endif
