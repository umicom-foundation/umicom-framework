/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement_book.c
 *
 * PURPOSE:
 *   Implement bounded settlement-control lifecycle and overdue inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/settlement_book.h"
#include <string.h>

void umi_financial_settlement_book_init(UmiFinancialSettlementBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialSettlementControl *umi_financial_settlement_book_find(
    const UmiFinancialSettlementBook *book,
    const char *settlementId)
{
    size_t i;
    if (book == NULL || settlementId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].settlementId, settlementId) == 0)
            return &book->records[i];
    return NULL;
}

UmiStatus umi_financial_settlement_book_add(
    UmiFinancialSettlementBook *book,
    const UmiFinancialSettlementControl *record)
{
    if (book == NULL || record == NULL || record->settlementId[0] == '\0' ||
        record->tradeOrPaymentId[0] == '\0' || record->accountId[0] == '\0' ||
        record->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_settlement_book_find(book, record->settlementId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_SETTLEMENT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_settlement_book_transition(
    UmiFinancialSettlementBook *book,
    const char *settlementId,
    UmiFinancialSettlementState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || settlementId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialSettlementControl *record = &book->records[i];
        if (strcmp(record->settlementId, settlementId) == 0) {
            if (version <= record->version) return UMI_STATUS_INVALID_STATE;
            if (record->state == UMI_FINANCIAL_SETTLEMENT_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            record->state = state;
            record->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_financial_settlement_book_overdue_count(
    const UmiFinancialSettlementBook *book,
    uint64_t nowMilliseconds)
{
    size_t i, count = 0U;
    if (book == NULL) return 0U;
    for (i = 0U; i < book->count; ++i) {
        const UmiFinancialSettlementControl *record = &book->records[i];
        if (record->state != UMI_FINANCIAL_SETTLEMENT_SETTLED &&
            record->dueMilliseconds > 0U &&
            nowMilliseconds > record->dueMilliseconds) {
            count += 1U;
        }
    }
    return count;
}
