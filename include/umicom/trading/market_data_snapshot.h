/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_data_snapshot.h
 *
 * PURPOSE:
 *   Determine whether quote and bar evidence are time-aligned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates market data snapshot behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_DATA_SNAPSHOT_H
#define INCLUDE_UMICOM_TRADING_MARKET_DATA_SNAPSHOT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_market_data_snapshot_aligned(const UmiQuote *quote, const UmiBar *bar, int64_t tolerance_ms);
#ifdef __cplusplus
}
#endif
#endif
