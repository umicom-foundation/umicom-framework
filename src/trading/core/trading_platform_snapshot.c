/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/trading_platform_snapshot.c
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
#include "umicom/trading/core/trading_platform_snapshot.h"

/*
 * Provide the trading platform snapshot build operation used by this module and its client
 * applications.
 */
void umi_trading_platform_snapshot_build(UmiTradingPlatformSnapshot *snapshot,uint64_t open_orders,uint64_t executions,uint64_t venues,uint64_t alerts,bool brokerage_ready,bool exchange_ready){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot==NULL)return;snapshot->open_orders=open_orders;snapshot->executions=executions;snapshot->venues=venues;snapshot->alerts=alerts;snapshot->brokerage_ready=brokerage_ready;snapshot->exchange_ready=exchange_ready;snapshot->severity=(!brokerage_ready||!exchange_ready||alerts>10U)?UMI_TRADING_CORE_CRITICAL:alerts>0U?UMI_TRADING_CORE_WARNING:UMI_TRADING_CORE_INFO;}
/*
 * Provide the trading platform snapshot ready operation used by this module and its client
 * applications.
 */
bool umi_trading_platform_snapshot_ready(const UmiTradingPlatformSnapshot *snapshot){return snapshot!=NULL&&snapshot->brokerage_ready&&snapshot->exchange_ready&&snapshot->venues>0U&&snapshot->severity!=UMI_TRADING_CORE_CRITICAL;}
