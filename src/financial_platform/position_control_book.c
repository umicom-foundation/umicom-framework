/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position_control_book.c
 *
 * PURPOSE:
 *   Implement monotonic position-control snapshots and account market value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position_control_book.h"
#include <string.h>

void umi_financial_position_control_book_init(UmiFinancialPositionControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialPositionControl *umi_financial_position_control_book_find(
    const UmiFinancialPositionControlBook *book,
    const char *accountId,
    const char *instrumentId)
{
    size_t i;
    if (book == NULL || accountId == NULL || instrumentId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->positions[i].accountId, accountId) == 0 &&
            strcmp(book->positions[i].instrumentId, instrumentId) == 0)
            return &book->positions[i];
    return NULL;
}

UmiStatus umi_financial_position_control_book_upsert(
    UmiFinancialPositionControlBook *book,
    const UmiFinancialPositionControl *position)
{
    size_t i;
    if (book == NULL || position == NULL || position->accountId[0] == '\0' ||
        position->instrumentId[0] == '\0' || position->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPositionControl *current = &book->positions[i];
        if (strcmp(current->accountId, position->accountId) == 0 &&
            strcmp(current->instrumentId, position->instrumentId) == 0) {
            if (position->sourceVersion < current->sourceVersion)
                return UMI_STATUS_INVALID_STATE;
            *current = *position;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_FINANCIAL_POSITION_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++] = *position;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_financial_position_control_book_market_value(
    const UmiFinancialPositionControlBook *book,
    const char *accountId)
{
    size_t i;
    double total = 0.0;
    if (book == NULL || accountId == NULL) return 0.0;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->positions[i].accountId, accountId) == 0)
            total += book->positions[i].quantity * book->positions[i].marketPrice;
    return total;
}
