/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/journal_digest.h
 *
 * PURPOSE:
 *   Calculate a deterministic lightweight digest for replay evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates journal digest behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_JOURNAL_DIGEST_H
#define INCLUDE_UMICOM_TRADING_JOURNAL_DIGEST_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
uint64_t umi_replay_event_digest(const UmiReplayEvent *event);
#ifdef __cplusplus
}
#endif
#endif
