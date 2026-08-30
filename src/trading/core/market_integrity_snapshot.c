/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_integrity_snapshot.c
 *
 * PURPOSE:
 *   Aggregate exchange integrity indicators for operational and surveillance dashboards.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_integrity_snapshot.h"

void umi_trading_market_integrity_snapshot_update(UmiTradingMarketIntegritySnapshot *snapshot,uint64_t alerts,uint64_t rejects,uint64_t interrupts,uint64_t trades){if(snapshot==NULL)return;snapshot->surveillance_alerts=alerts;snapshot->rejected_orders=rejects;snapshot->volatility_interrupts=interrupts;snapshot->matched_trades=trades;snapshot->severity=(alerts>10U||interrupts>5U)?UMI_TRADING_CORE_CRITICAL:(alerts>0U||rejects>20U||interrupts>0U)?UMI_TRADING_CORE_WARNING:UMI_TRADING_CORE_INFO;}
