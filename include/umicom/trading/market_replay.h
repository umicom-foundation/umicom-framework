/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_replay.h
 *
 * PURPOSE:
 *   Determine whether a replay event falls inside a requested historical window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates market replay behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_REPLAY_H
#define INCLUDE_UMICOM_TRADING_MARKET_REPLAY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the market replay in window operation used by this module and its client
 * applications.
 */
int umi_market_replay_in_window(const UmiReplayEvent *event, int64_t start_ms, int64_t end_ms);
#ifdef __cplusplus
}
#endif
#endif
