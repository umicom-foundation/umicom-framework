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
typedef struct UmiReplayCursor { uint64_t next_sequence; } UmiReplayCursor;
void umi_replay_cursor_init(UmiReplayCursor *cursor, uint64_t first_sequence);
int umi_replay_cursor_accept(UmiReplayCursor *cursor, const UmiReplayEvent *event);
#ifdef __cplusplus
}
#endif
#endif
