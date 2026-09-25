/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/settlement_book.c
 *
 * PURPOSE:
 *   Implement treasury settlement lifecycle, failure and overdue evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/settlement_book.h"

#include <string.h>

void umi_open_tms_settlement_book_init(UmiOpenTmsSettlementBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

UmiStatus umi_open_tms_settlement_book_add(
    UmiOpenTmsSettlementBook *book,
    const UmiOpenTmsSettlementRecord *record)
{
    size_t index;

    if (book == NULL || record == NULL || record->id[0] == '\0' ||
        record->sourceId[0] == '\0' || record->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].id, record->id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (book->count >= UMI_OPEN_TMS_SETTLEMENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_settlement_book_transition(
    UmiOpenTmsSettlementBook *book,
    const char *id,
    UmiOpenTmsSettlementState state,
    uint64_t version)
{
    size_t index;

    if (book == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < book->count; ++index) {
        UmiOpenTmsSettlementRecord *record = &book->records[index];
        if (strcmp(record->id, id) == 0) {
            if (version <= record->version) return UMI_STATUS_INVALID_STATE;
            if (record->state == UMI_OPEN_TMS_SETTLEMENT_SETTLED) {
                return UMI_STATUS_INVALID_STATE;
            }
            record->state = state;
            record->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_open_tms_settlement_book_failed_count(
    const UmiOpenTmsSettlementBook *book)
{
    size_t index;
    size_t count = 0U;

    if (book == NULL) return 0U;
    for (index = 0U; index < book->count; ++index) {
        if (book->records[index].state == UMI_OPEN_TMS_SETTLEMENT_FAILED) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_open_tms_settlement_book_overdue_count(
    const UmiOpenTmsSettlementBook *book,
    uint64_t nowMilliseconds)
{
    size_t index;
    size_t count = 0U;

    if (book == NULL) return 0U;
    for (index = 0U; index < book->count; ++index) {
        const UmiOpenTmsSettlementRecord *record = &book->records[index];
        if (record->state != UMI_OPEN_TMS_SETTLEMENT_SETTLED &&
            record->dueMilliseconds > 0U &&
            nowMilliseconds > record->dueMilliseconds) {
            count += 1U;
        }
    }
    return count;
}
