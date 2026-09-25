/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_book.c
 *
 * PURPOSE:
 *   Implement monotonic FX-forward lifecycle and simple forward MTM evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/fx_forward_book.h"
#include <string.h>

void umi_open_tms_fx_forward_book_init(UmiOpenTmsFxForwardBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiOpenTmsFxForward *umi_open_tms_fx_forward_book_find(
    const UmiOpenTmsFxForwardBook *book,
    const char *tradeId)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->trades[i].tradeId, tradeId) == 0)
            return &book->trades[i];
    return NULL;
}

UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForward *trade)
{
    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->baseCurrency[0] == '\0' || trade->quoteCurrency[0] == '\0' ||
        trade->baseNotional <= 0.0 || trade->forwardRate <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_fx_forward_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_OPEN_TMS_FX_FORWARD_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book,
    const char *tradeId,
    UmiOpenTmsFxForwardState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiOpenTmsFxForward *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_OPEN_TMS_FX_FORWARD_SETTLED ||
                trade->state == UMI_OPEN_TMS_FX_FORWARD_CANCELLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

double umi_open_tms_fx_forward_mark_to_market(
    const UmiOpenTmsFxForward *trade)
{
    if (trade == NULL || trade->baseNotional <= 0.0 ||
        trade->forwardRate <= 0.0 || trade->marketForwardRate <= 0.0)
        return 0.0;
    return trade->baseNotional *
        (trade->marketForwardRate - trade->forwardRate);
}
