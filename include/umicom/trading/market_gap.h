/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_gap.h
 *
 * PURPOSE:
 *   Measure time gaps between market events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates market gap behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MARKET_GAP_H
#define INCLUDE_UMICOM_TRADING_MARKET_GAP_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int64_t umi_market_event_gap_ms(int64_t previous_ms, int64_t current_ms);
#ifdef __cplusplus
}
#endif
#endif
