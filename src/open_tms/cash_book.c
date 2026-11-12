/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash_book.c
 *
 * PURPOSE:
 *   Implement monotonic treasury cash snapshots and forecast/headroom calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/cash_book.h"
#include <string.h>

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
void umi_open_tms_cash_book_init(UmiOpenTmsCashBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiOpenTmsCashRecord *umi_open_tms_cash_book_find(
    const UmiOpenTmsCashBook *book,
    const char *accountId,
    const char *currency)
{
    size_t i;
    if (book == NULL || accountId == NULL || currency == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].accountId, accountId) == 0 &&
            strcmp(book->records[i].currency, currency) == 0)
            return &book->records[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_cash_book_upsert(
    UmiOpenTmsCashBook *book,
    const UmiOpenTmsCashRecord *record)
{
    size_t i;
    if (book == NULL || record == NULL ||
        record->accountId[0] == '\0' || record->currency[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiOpenTmsCashRecord *current = &book->records[i];
        if (strcmp(current->accountId, record->accountId) == 0 &&
            strcmp(current->currency, record->currency) == 0) {
            if (record->eventMilliseconds < current->eventMilliseconds)
                return UMI_STATUS_INVALID_STATE;
            *current = *record;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_OPEN_TMS_CASH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_cash_book_upsert(
    UmiOpenTmsCashBook *book,
    const UmiOpenTmsCashRecord *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || record == NULL ||
        record->accountId[0] == '\0' || record->currency[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiOpenTmsCashRecord *current = &book->records[i];
        if (strcmp(current->accountId, record->accountId) == 0 &&
            strcmp(current->currency, record->currency) == 0) {
            if (record->eventMilliseconds < current->eventMilliseconds)
                return UMI_STATUS_INVALID_STATE;
            *current = *record;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_OPEN_TMS_CASH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_open_tms_cash_record_net_forecast(const UmiOpenTmsCashRecord *record)
{
    return record != NULL ? record->projected + record->inflows - record->outflows : 0.0;
}

double umi_open_tms_cash_record_headroom(const UmiOpenTmsCashRecord *record)
{
    return record != NULL
        ? umi_open_tms_cash_record_net_forecast(record) - record->minimum
        : 0.0;
}

#endif

UmiStatus umi_open_tms_cash_record_validate(const UmiOpenTmsCashRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"accountId", offsetof(UmiOpenTmsCashRecord, accountId), 64U, 1},
        {"currency", offsetof(UmiOpenTmsCashRecord, currency), 8U, 1}
    };
    if (record == NULL || UmiSnapshotValidateTextFields(record, sizeof(*record),
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(record->settled) || !isfinite(record->projected) ||
        !isfinite(record->minimum) || !isfinite(record->inflows) ||
        !isfinite(record->outflows)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_cash_book_validate(const UmiOpenTmsCashBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_CASH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsCashRecord *row = &book->records[i];
        if (umi_open_tms_cash_record_validate(row) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(row->accountId, book->records[j].accountId) == 0 &&
                strcmp(row->currency, book->records[j].currency) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_open_tms_cash_book_init(UmiOpenTmsCashBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
const UmiOpenTmsCashRecord *umi_open_tms_cash_book_find(
    const UmiOpenTmsCashBook *book, const char *accountId, const char *currency)
{
    if (!TmsOpsQuery(accountId, 64U) || !TmsOpsQuery(currency, 8U) ||
        umi_open_tms_cash_book_validate(book) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].accountId, accountId) == 0 &&
            strcmp(book->records[i].currency, currency) == 0) return &book->records[i];
    return NULL;
}
UmiStatus umi_open_tms_cash_book_upsert(UmiOpenTmsCashBook *book,
    const UmiOpenTmsCashRecord *record)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_cash_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_cash_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsCashRecord *current = &book->records[i];
        if (strcmp(current->accountId, record->accountId) == 0 &&
            strcmp(current->currency, record->currency) == 0) {
            if (record->eventMilliseconds < current->eventMilliseconds)
                return UMI_STATUS_INVALID_STATE;
            *current = *record;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    if (book->count == UMI_OPEN_TMS_CASH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_cash_record_measure(const UmiOpenTmsCashRecord *record,
    UmiOpenTmsCashMeasures *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_cash_record_validate(record);
    UmiOpenTmsCashMeasures value = {0};
    if (status == UMI_STATUS_OK) status = TmsOpsAdd(record->projected, record->inflows, &value.netForecast);
    if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.netForecast, -record->outflows, &value.netForecast);
    if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.netForecast, -record->minimum, &value.headroom);
    if (status == UMI_STATUS_OK) *out = value;
    return status;
}
UmiStatus umi_open_tms_cash_book_headroom_checked(const UmiOpenTmsCashBook *book,
    double *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_cash_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    double total = 0.0;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsCashMeasures row;
        if (strcmp(book->records[i].currency, book->records[0].currency) != 0)
            return UMI_STATUS_INVALID_STATE;
        status = umi_open_tms_cash_record_measure(&book->records[i], &row);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(total, row.headroom, &total);
        if (status != UMI_STATUS_OK) return status;
    }
    *out = total;
    return UMI_STATUS_OK;
}
double umi_open_tms_cash_record_net_forecast(const UmiOpenTmsCashRecord *record)
{
    UmiOpenTmsCashMeasures value;
    return umi_open_tms_cash_record_measure(record, &value) == UMI_STATUS_OK ? value.netForecast : 0.0;
}
double umi_open_tms_cash_record_headroom(const UmiOpenTmsCashRecord *record)
{
    UmiOpenTmsCashMeasures value;
    return umi_open_tms_cash_record_measure(record, &value) == UMI_STATUS_OK ? value.headroom : 0.0;
}
