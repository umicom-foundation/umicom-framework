/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/health.h
 *
 * PURPOSE:
 *   Combine market-data, broker and risk readiness into one trading health decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates health behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_HEALTH_H
#define INCLUDE_UMICOM_TRADING_HEALTH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_trading_health_ready(int market_data_ready, int broker_ready, int risk_ready, UmiTradingEnvironment environment);
#ifdef __cplusplus
}
#endif
#endif
