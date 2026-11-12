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
#include <stdint.h>
#include "umicom/financial_platform/currency_rate_book.h"

#include <string.h>

#include "umicom/base/snapshot_validation.h"
#include "umicom/base/text.h"
#include <math.h>

/* Retained for review: the former rate owner trusted public bounds and could
 * publish non-finite conversions. The active boundary uses shared bounded text
 * admission and validates the complete owner before lookup or identity conversion. */
#if 0
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

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_financial_rate_book_upsert(
    UmiFinancialRateBook *book,
    const UmiFinancialRateRecord *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif

UmiStatus umi_financial_rate_record_validate(const UmiFinancialRateRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"base", offsetof(UmiFinancialRateRecord, base), 8U, 1},
        {"quote", offsetof(UmiFinancialRateRecord, quote), 8U, 1}
    };
    if (record == NULL || UmiSnapshotValidateTextFields(record, sizeof(*record),
            fields, 2U, NULL) != UMI_STATUS_OK || !isfinite(record->rate) || record->rate <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_rate_book_validate(const UmiFinancialRateBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_FINANCIAL_RATE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        if (umi_financial_rate_record_validate(&book->records[i]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(book->records[i].base, book->records[j].base) == 0 &&
                strcmp(book->records[i].quote, book->records[j].quote) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
/* Reuse Framework's bounded replacement contract to admit borrowed queries.
 * A query is terminated readable text, or has at least eight readable bytes. */
static UmiStatus RateQuery(const char *source, char destination[8])
{
    UmiStatus status = umi_text_replace(destination, 8U, source);
    if (status != UMI_STATUS_OK) return status;
    return destination[0] == '\0' ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
void umi_financial_rate_book_init(UmiFinancialRateBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
UmiStatus umi_financial_rate_book_find(const UmiFinancialRateBook *book,
    const char *base, const char *quote, UmiFinancialRateRecord *outRecord)
{
    if (outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char baseKey[8], quoteKey[8];
    UmiStatus status = RateQuery(base, baseKey);
    if (status == UMI_STATUS_OK) status = RateQuery(quote, quoteKey);
    if (status == UMI_STATUS_OK) status = umi_financial_rate_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].base, baseKey) == 0 && strcmp(book->records[i].quote, quoteKey) == 0) {
            *outRecord = book->records[i];
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_financial_rate_book_upsert(UmiFinancialRateBook *book,
    const UmiFinancialRateRecord *record)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_rate_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_financial_rate_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiFinancialRateRecord *current = &book->records[i];
        if (strcmp(current->base, record->base) == 0 && strcmp(current->quote, record->quote) == 0) {
            if (record->eventMilliseconds < current->eventMilliseconds) return UMI_STATUS_INVALID_STATE;
            *current = *record;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    if (book->count == UMI_FINANCIAL_RATE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_rate_book_convert(const UmiFinancialRateBook *book,
    const char *base, const char *quote, double amount, double *outAmount)
{
    if (outAmount == NULL || !isfinite(amount)) return UMI_STATUS_INVALID_ARGUMENT;
    char baseKey[8], quoteKey[8];
    UmiStatus status = RateQuery(base, baseKey);
    if (status == UMI_STATUS_OK) status = RateQuery(quote, quoteKey);
    if (status == UMI_STATUS_OK) status = umi_financial_rate_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(baseKey, quoteKey) == 0) { *outAmount = amount; return UMI_STATUS_OK; }
    UmiFinancialRateRecord rate;
    status = umi_financial_rate_book_find(book, baseKey, quoteKey, &rate);
    if (status != UMI_STATUS_OK) return status;
    double value = amount * rate.rate;
    if (!isfinite(value) || (amount != 0.0 && value == 0.0)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outAmount = value;
    return UMI_STATUS_OK;
}
