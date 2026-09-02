/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_replay.c
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
 * This source implements the small deterministic core of market replay. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_replay.h"
/*
 * Provide the market replay in window operation used by this module and its client
 * applications.
 */
int umi_market_replay_in_window(const UmiReplayEvent *e,int64_t start,int64_t end){return e!=NULL&&start<=e->event_time_ms&&e->event_time_ms<=end;}
