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

void umi_watchlist_init(UmiWatchlist *watchlist)
{
    if (watchlist != NULL) {
        watchlist->count = 0U;
    }
}

UmiStatus umi_watchlist_add(UmiWatchlist *watchlist,
                            const UmiInstrument *instrument)
{
    if (watchlist == NULL || !umi_instrument_valid(instrument)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (size_t index = 0U; index < watchlist->count; ++index) {
        if (umi_instrument_same(&watchlist->instruments[index],
                                instrument)) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    if (watchlist->count >= UMI_TRADING_MAX_WATCHLIST) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    watchlist->instruments[watchlist->count] = *instrument;
    watchlist->count++;
    return UMI_STATUS_OK;
}
