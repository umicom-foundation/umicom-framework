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
#include <stdint.h>
#include "umicom/open_tms/settlement_book.h"

#include <string.h>

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
void umi_open_tms_settlement_book_init(UmiOpenTmsSettlementBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_open_tms_settlement_book_add(
    UmiOpenTmsSettlementBook *book,
    const UmiOpenTmsSettlementRecord *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_open_tms_settlement_book_transition(
    UmiOpenTmsSettlementBook *book,
    const char *id,
    UmiOpenTmsSettlementState state,
    uint64_t version)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif

static int SettlementStateKnown(UmiOpenTmsSettlementState state)
{
    return state >= UMI_OPEN_TMS_SETTLEMENT_PENDING && state <= UMI_OPEN_TMS_SETTLEMENT_REPAIR;
}
UmiStatus umi_open_tms_settlement_record_validate(const UmiOpenTmsSettlementRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"id", offsetof(UmiOpenTmsSettlementRecord, id), 64U, 1},
        {"sourceId", offsetof(UmiOpenTmsSettlementRecord, sourceId), 64U, 1},
        {"accountId", offsetof(UmiOpenTmsSettlementRecord, accountId), 64U, 0},
        {"currency", offsetof(UmiOpenTmsSettlementRecord, currency), 8U, 1}
    };
    if (record == NULL || UmiSnapshotValidateTextFields(record, sizeof(*record),
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(record->amount) || !SettlementStateKnown(record->state)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_settlement_book_validate(const UmiOpenTmsSettlementBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_SETTLEMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        if (umi_open_tms_settlement_record_validate(&book->records[i]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(book->records[i].id, book->records[j].id) == 0) return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_open_tms_settlement_book_init(UmiOpenTmsSettlementBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
UmiStatus umi_open_tms_settlement_book_add(UmiOpenTmsSettlementBook *book,
    const UmiOpenTmsSettlementRecord *record)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_settlement_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_settlement_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].id, record->id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (book->count == UMI_OPEN_TMS_SETTLEMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_settlement_book_transition(UmiOpenTmsSettlementBook *book,
    const char *id, UmiOpenTmsSettlementState state, uint64_t version)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!TmsOpsQuery(id, 64U) || !SettlementStateKnown(state)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_settlement_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsSettlementRecord *row = &book->records[i];
        if (strcmp(row->id, id) == 0) {
            if (version <= row->version || row->state == UMI_OPEN_TMS_SETTLEMENT_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            /* Preserve the existing permissive transitions between known states.
             * This local state update does not send or settle a payment. */
            row->state = state;
            row->version = version;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_open_tms_settlement_book_measure(const UmiOpenTmsSettlementBook *book,
    uint64_t nowMilliseconds, UmiOpenTmsSettlementMeasures *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_settlement_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    UmiOpenTmsSettlementMeasures value = {0};
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsSettlementRecord *row = &book->records[i];
        if (row->state == UMI_OPEN_TMS_SETTLEMENT_FAILED) ++value.failedCount;
        if (row->state != UMI_OPEN_TMS_SETTLEMENT_SETTLED && row->dueMilliseconds != 0U &&
            nowMilliseconds > row->dueMilliseconds) ++value.overdueCount;
    }
    *out = value;
    return UMI_STATUS_OK;
}
size_t umi_open_tms_settlement_book_failed_count(const UmiOpenTmsSettlementBook *book)
{
    UmiOpenTmsSettlementMeasures value;
    return umi_open_tms_settlement_book_measure(book, 0U, &value) == UMI_STATUS_OK ? value.failedCount : 0U;
}
size_t umi_open_tms_settlement_book_overdue_count(const UmiOpenTmsSettlementBook *book,
    uint64_t nowMilliseconds)
{
    UmiOpenTmsSettlementMeasures value;
    return umi_open_tms_settlement_book_measure(book, nowMilliseconds, &value) == UMI_STATUS_OK ? value.overdueCount : 0U;
}
