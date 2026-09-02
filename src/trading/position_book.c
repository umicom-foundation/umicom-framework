/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/position_book.c
 *
 * PURPOSE:
 *   Maintain a bounded collection of instrument positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of position book. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/position_book.h"
#include "umicom/trading/instrument.h"

/*
 * Initialise position book from caller-provided values so later operations receive a known
 * state.
 */
void umi_position_book_init(UmiPositionBook *book)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (book != NULL) {
        book->count = 0U;
    }
}

/* Provide the position book get operation used by this module and its client applications. */
UmiStatus umi_position_book_get(UmiPositionBook *book,
                                const UmiInstrument *instrument,
                                int create_if_missing,
                                UmiPosition **out_position)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (book == NULL || instrument == NULL || out_position == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_position = NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t index = 0U; index < book->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_instrument_same(&book->positions[index].instrument,
                                instrument)) {
            *out_position = &book->positions[index];
            return UMI_STATUS_OK;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!create_if_missing) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (book->count >= UMI_TRADING_MAX_POSITIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    UmiPosition *position = &book->positions[book->count];
    *position = (UmiPosition){0};
    position->instrument = *instrument;
    book->count++;
    *out_position = position;
    return UMI_STATUS_OK;
}
