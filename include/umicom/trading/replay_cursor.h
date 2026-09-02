/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/replay_cursor.h
 *
 * PURPOSE:
 *   Track monotonic replay sequence progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates replay cursor behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_REPLAY_CURSOR_H
#define INCLUDE_UMICOM_TRADING_REPLAY_CURSOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the replay cursor data shared with callers of this public contract.
 */
typedef struct UmiReplayCursor { uint64_t next_sequence; } UmiReplayCursor;
/**
 * Initialise replay cursor from caller-provided values so later operations receive a known
 * state.
 */
void umi_replay_cursor_init(UmiReplayCursor *cursor, uint64_t first_sequence);
/**
 * Provide the replay cursor accept operation used by this module and its client
 * applications.
 */
int umi_replay_cursor_accept(UmiReplayCursor *cursor, const UmiReplayEvent *event);
#ifdef __cplusplus
}
#endif
#endif
