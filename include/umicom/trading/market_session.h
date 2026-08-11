/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_session.h
 *
 * PURPOSE:
 *   Represent a simple daily market-session window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates market session behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_SESSION_H
#define INCLUDE_UMICOM_TRADING_MARKET_SESSION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_market_session_contains(int64_t open_ms, int64_t close_ms, int64_t value_ms);
#ifdef __cplusplus
}
#endif
#endif
