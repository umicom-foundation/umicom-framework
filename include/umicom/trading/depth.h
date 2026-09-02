/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/depth.h
 *
 * PURPOSE:
 *   Validate market-depth levels and obtain best bid/ask.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates depth behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_DEPTH_H
#define INCLUDE_UMICOM_TRADING_DEPTH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that market depth satisfies its contract before another service relies on it.
 */
int umi_market_depth_valid(const UmiMarketDepth *depth);
/**
 * Provide the market depth best bid operation used by this module and its client
 * applications.
 */
double umi_market_depth_best_bid(const UmiMarketDepth *depth);
/**
 * Provide the market depth best ask operation used by this module and its client
 * applications.
 */
double umi_market_depth_best_ask(const UmiMarketDepth *depth);
#ifdef __cplusplus
}
#endif
#endif
