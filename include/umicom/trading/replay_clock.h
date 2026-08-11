/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/replay_clock.h
 *
 * PURPOSE:
 *   Provide a deterministic logical time source for historical playback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates replay clock behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_REPLAY_CLOCK_H
#define INCLUDE_UMICOM_TRADING_REPLAY_CLOCK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiReplayClock { int64_t now_ms; double speed; } UmiReplayClock;
void umi_replay_clock_init(UmiReplayClock *clock, int64_t start_ms, double speed);
void umi_replay_clock_advance(UmiReplayClock *clock, int64_t event_time_ms);
#ifdef __cplusplus
}
#endif
#endif
