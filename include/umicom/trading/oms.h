/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/oms.h
 *
 * PURPOSE:
 *   Coordinate order validation, risk evaluation and storage in a small reference OMS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates oms behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_OMS_H
#define INCLUDE_UMICOM_TRADING_OMS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/order_store.h"
#include "umicom/trading/kill_switch.h"
#include "umicom/trading/order_throttle.h"
#include "umicom/trading/risk_limit.h"
#include "umicom/trading/risk_decision.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiOms { UmiOrderStore orders; UmiTradingKillSwitch kill_switch; UmiOrderThrottle throttle; UmiRiskLimit risk_limit; } UmiOms;
void umi_oms_init(UmiOms *oms, UmiRiskLimit risk_limit);
UmiStatus umi_oms_submit(UmiOms *oms, const UmiOrderRequest *request, double current_position, double daily_pnl, int64_t now_ms, UmiRiskDecision *decision);
#ifdef __cplusplus
}
#endif
#endif
