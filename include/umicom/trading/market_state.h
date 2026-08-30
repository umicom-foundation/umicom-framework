/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_state.h
 *
 * PURPOSE:
 *   Validate market-state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates market state behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_STATE_H
#define INCLUDE_UMICOM_TRADING_MARKET_STATE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_market_state_transition_allowed(UmiMarketState from, UmiMarketState to);
#ifdef __cplusplus
}
#endif
#endif
