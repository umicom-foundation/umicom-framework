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

void umi_position_book_init(UmiPositionBook *book)
{
    if (book != NULL) {
        book->count = 0U;
    }
}

UmiStatus umi_position_book_get(UmiPositionBook *book,
                                const UmiInstrument *instrument,
                                int create_if_missing,
                                UmiPosition **out_position)
{
    if (book == NULL || instrument == NULL || out_position == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_position = NULL;
    for (size_t index = 0U; index < book->count; ++index) {
        if (umi_instrument_same(&book->positions[index].instrument,
                                instrument)) {
            *out_position = &book->positions[index];
            return UMI_STATUS_OK;
        }
    }

    if (!create_if_missing) {
        return UMI_STATUS_NOT_FOUND;
    }
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
