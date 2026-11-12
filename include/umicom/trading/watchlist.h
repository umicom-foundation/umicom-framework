/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/watchlist.h
 *
 * PURPOSE:
 *   Maintain a bounded watchlist of canonical instruments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates watchlist behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_WATCHLIST_H
#define INCLUDE_UMICOM_TRADING_WATCHLIST_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the watchlist data shared with callers of this public contract.
 */
typedef struct UmiWatchlist { UmiInstrument instruments[UMI_TRADING_MAX_WATCHLIST]; size_t count; } UmiWatchlist;
/**
 * Initialise watchlist from caller-provided values so later operations receive a known
 * state.
 */
void umi_watchlist_init(UmiWatchlist *watchlist);
/**
 * Add watchlist only after its inputs and available capacity have been checked.
 * Initialise the list before use. A count above the array capacity is invalid;
 * rejection leaves the list unchanged. A duplicate in a full valid list returns
 * ALREADY_EXISTS, while a new instrument returns CAPACITY_EXCEEDED.
 * This structure is caller-owned and requires serial access.
 */
UmiStatus umi_watchlist_add(UmiWatchlist *watchlist, const UmiInstrument *instrument);
/* Inspect all active records and unique identities. Invalid count/NULL owner
 * returns INVALID_ARGUMENT; malformed active records or repeated identities
 * return INVALID_STATE. Inactive bytes do not participate. Insertion performs
 * this check before duplicate/capacity decisions and preserves bytes on refusal. */
UmiStatus umi_watchlist_validate(const UmiWatchlist *watchlist);
/* Copy a stable index on success only; every refusal preserves out_index.
 * Query storage must be readable through its terminator or ID capacity. An
 * unterminated query returns CAPACITY_EXCEEDED; empty/NULL is INVALID_ARGUMENT.
 * Output storage must be writable, aligned and separate from the owner.
 * Query/output may share separate storage: query capture precedes publication.
 * The index expires on catalogue edits. Callers serialize access. */
UmiStatus umi_watchlist_find(const UmiWatchlist *watchlist,
                            const char *instrument_id, size_t *out_index);
#ifdef __cplusplus
}
#endif
#endif
