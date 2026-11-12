/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/watchlist.c
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
 * This source implements the small deterministic core of watchlist. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/watchlist.h"
#include "umicom/trading/instrument.h"
#include "umicom/finance/identifier.h"
#include "umicom/base/text.h"
#include "../base/storage_internal.h"

/*
 * Initialise watchlist from caller-provided values so later operations receive a known
 * state.
 */
void umi_watchlist_init(UmiWatchlist *watchlist)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watchlist != NULL) {
        watchlist->count = 0U;
    }
}

/* Add watchlist only after its inputs and available capacity have been checked. */
/* Admit the complete active catalogue before insertion so an early duplicate cannot hide a malformed later record. Shared validation and bounded lookup replace the independent search. The previous implementation is retained for review. */
#if 0
UmiStatus umi_watchlist_add(UmiWatchlist *watchlist,
                            const UmiInstrument *instrument)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (watchlist == NULL || !umi_instrument_valid(instrument)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A caller-owned count must be checked before it controls an array read.
     * A full, valid list is still searched so duplicate insertion keeps its
     * existing result; an impossible count is rejected without mutation. */
    if (watchlist->count > UMI_TRADING_MAX_WATCHLIST) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t index = 0U; index < watchlist->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_instrument_same(&watchlist->instruments[index],
                                instrument)) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (watchlist->count >= UMI_TRADING_MAX_WATCHLIST) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    watchlist->instruments[watchlist->count] = *instrument;
    watchlist->count++;
    return UMI_STATUS_OK;
}
#endif
/* Complete admission makes every consumer observe the same active catalogue.
 * Inactive slots are deliberately ignored; init need only reset the count.
 * Count errors keep the established INVALID_ARGUMENT result. Malformed active
 * records and duplicate identities instead indicate damaged retained state. */
UmiStatus umi_watchlist_validate(const UmiWatchlist *watchlist)
{
    if (watchlist == NULL || watchlist->count > UMI_TRADING_MAX_WATCHLIST)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < watchlist->count; ++index) {
        if (!umi_instrument_valid(&watchlist->instruments[index]))
            return UMI_STATUS_INVALID_STATE;
        for (size_t prior = 0U; prior < index; ++prior)
            if (umi_instrument_same(&watchlist->instruments[index],
                                    &watchlist->instruments[prior]))
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* The returned index belongs to this catalogue until its next edit. Capture
 * bounded query bytes before publishing output, and never write into the
 * owner through a caller-supplied output pointer. This is not a lifetime lock. */
UmiStatus umi_watchlist_find(const UmiWatchlist *watchlist,
                            const char *instrument_id, size_t *out_index)
{
    if (watchlist == NULL || instrument_id == NULL || out_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiStorageOverlaps(watchlist, sizeof(*watchlist), out_index, sizeof(*out_index)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiFinancialId query = {0};
    UmiStatus status = umi_text_replace(query.value, sizeof(query.value), instrument_id);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_financial_id_valid(&query)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_watchlist_validate(watchlist);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < watchlist->count; ++index) {
        if (umi_financial_id_equal(&query, &watchlist->instruments[index].instrument_id)) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_watchlist_add(UmiWatchlist *watchlist, const UmiInstrument *instrument)
{
    if (watchlist == NULL || !umi_instrument_valid(instrument))
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiStatus status = umi_watchlist_validate(watchlist);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < watchlist->count; ++index)
        if (umi_instrument_same(&watchlist->instruments[index], instrument))
            return UMI_STATUS_ALREADY_EXISTS;
    if (watchlist->count == UMI_TRADING_MAX_WATCHLIST)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiInstrument staged = *instrument;
    watchlist->instruments[watchlist->count] = staged;
    watchlist->count += 1U;
    return UMI_STATUS_OK;
}
