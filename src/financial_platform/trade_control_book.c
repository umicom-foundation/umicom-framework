/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade_control_book.c
 *
 * PURPOSE:
 *   Implement monotonic trade-control lifecycle references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/trade_control_book.h"
#include <string.h>

void umi_financial_trade_control_book_init(UmiFinancialTradeControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialTradeControl *umi_financial_trade_control_book_find(
    const UmiFinancialTradeControlBook *book,
    const char *tradeId)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->trades[i].tradeId, tradeId) == 0)
            return &book->trades[i];
    return NULL;
}

UmiStatus umi_financial_trade_control_book_add(
    UmiFinancialTradeControlBook *book,
    const UmiFinancialTradeControl *trade)
{
    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->instrumentId[0] == '\0' || trade->currency[0] == '\0' ||
        trade->quantity == 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_trade_control_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_TRADE_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_trade_control_book_transition(
    UmiFinancialTradeControlBook *book,
    const char *tradeId,
    UmiFinancialTradeControlState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialTradeControl *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_FINANCIAL_TRADE_CANCELLED ||
                trade->state == UMI_FINANCIAL_TRADE_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
