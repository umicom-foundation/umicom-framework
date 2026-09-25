/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency_rate_book.c
 *
 * PURPOSE:
 *   Implement bounded monotonic currency-rate retention and conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency_rate_book.h"

#include <string.h>

void umi_financial_rate_book_init(UmiFinancialRateBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

UmiStatus umi_financial_rate_book_find(
    const UmiFinancialRateBook *book,
    const char *base,
    const char *quote,
    UmiFinancialRateRecord *outRecord)
{
    size_t index;
    if (book == NULL || base == NULL || quote == NULL || outRecord == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].base, base) == 0 &&
            strcmp(book->records[index].quote, quote) == 0) {
            *outRecord = book->records[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_financial_rate_book_upsert(
    UmiFinancialRateBook *book,
    const UmiFinancialRateRecord *record)
{
    size_t index;
    if (book == NULL || record == NULL || record->base[0] == '\0' ||
        record->quote[0] == '\0' || record->rate <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        UmiFinancialRateRecord *current = &book->records[index];
        if (strcmp(current->base, record->base) == 0 &&
            strcmp(current->quote, record->quote) == 0) {
            if (record->eventMilliseconds < current->eventMilliseconds) {
                return UMI_STATUS_INVALID_STATE;
            }
            *current = *record;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_FINANCIAL_RATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_rate_book_convert(
    const UmiFinancialRateBook *book,
    const char *base,
    const char *quote,
    double amount,
    double *outAmount)
{
    UmiFinancialRateRecord rate;
    if (book == NULL || base == NULL || quote == NULL || outAmount == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(base, quote) == 0) {
        *outAmount = amount;
        return UMI_STATUS_OK;
    }
    if (umi_financial_rate_book_find(book, base, quote, &rate) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outAmount = amount * rate.rate;
    return UMI_STATUS_OK;
}
