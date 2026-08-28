/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/market_integrity_snapshot.h
 *
 * PURPOSE:
 *   Aggregate exchange integrity indicators for operational and surveillance dashboards.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARKET_INTEGRITY_SNAPSHOT_H
#define UMICOM_TRADING_CORE_MARKET_INTEGRITY_SNAPSHOT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingMarketIntegritySnapshot { uint64_t surveillance_alerts; uint64_t rejected_orders; uint64_t volatility_interrupts; uint64_t matched_trades; UmiTradingCoreSeverity severity; } UmiTradingMarketIntegritySnapshot;
/* Derive an aggregate severity from incident and activity counters. */
void umi_trading_market_integrity_snapshot_update(UmiTradingMarketIntegritySnapshot *snapshot,uint64_t alerts,uint64_t rejects,uint64_t interrupts,uint64_t trades);
#ifdef __cplusplus
}
#endif
#endif
