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
#include "umicom/open_tms/cash_book.h"
#include <string.h>

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
