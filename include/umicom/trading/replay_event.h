/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/replay_event.h
 *
 * PURPOSE:
 *   Validate deterministic replay event envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates replay event behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_REPLAY_EVENT_H
#define INCLUDE_UMICOM_TRADING_REPLAY_EVENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_replay_event_valid(const UmiReplayEvent *event);
#ifdef __cplusplus
}
#endif
#endif
