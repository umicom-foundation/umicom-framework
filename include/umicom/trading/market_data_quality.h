/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_data_quality.h
 *
 * PURPOSE:
 *   Score basic quote freshness and crossed-market quality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates market data quality behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_DATA_QUALITY_H
#define INCLUDE_UMICOM_TRADING_MARKET_DATA_QUALITY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_market_data_quality_score(const UmiQuote *quote, int64_t now_ms, int64_t max_age_ms);
#ifdef __cplusplus
}
#endif
#endif
