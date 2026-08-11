/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/depth.h
 *
 * PURPOSE:
 *   Validate market-depth levels and obtain best bid/ask.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
int umi_market_depth_valid(const UmiMarketDepth *depth);
double umi_market_depth_best_bid(const UmiMarketDepth *depth);
double umi_market_depth_best_ask(const UmiMarketDepth *depth);
#ifdef __cplusplus
}
#endif
#endif
