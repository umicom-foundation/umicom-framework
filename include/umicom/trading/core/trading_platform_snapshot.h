/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/trading_platform_snapshot.h
 *
 * PURPOSE:
 *   Publish a compact deterministic snapshot of brokerage and exchange platform health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TRADING_PLATFORM_SNAPSHOT_H
#define UMICOM_TRADING_CORE_TRADING_PLATFORM_SNAPSHOT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingPlatformSnapshot { uint64_t open_orders; uint64_t executions; uint64_t venues; uint64_t alerts; bool brokerage_ready; bool exchange_ready; UmiTradingCoreSeverity severity; } UmiTradingPlatformSnapshot;
/* Populate aggregate platform health and severity from bounded counters. */
void umi_trading_platform_snapshot_build(UmiTradingPlatformSnapshot *snapshot,uint64_t open_orders,uint64_t executions,uint64_t venues,uint64_t alerts,bool brokerage_ready,bool exchange_ready);
/* Return whether the trading platform is operational and not critically degraded. */
bool umi_trading_platform_snapshot_ready(const UmiTradingPlatformSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
