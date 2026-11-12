/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/reconciliation_book.c
 *
 * PURPOSE:
 *   Implement tolerance matching, break tracking and resolution for treasury.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/reconciliation_book.h"

#include <math.h>
#include <string.h>

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
void umi_open_tms_reconciliation_book_init(
    UmiOpenTmsReconciliationBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_reconciliation_book_add(
    UmiOpenTmsReconciliationBook *book,
    const UmiOpenTmsReconciliationItem *item)
{
    UmiOpenTmsReconciliationItem copy;

    if (book == NULL || item == NULL || item->id[0] == '\0' ||
        item->sourceId[0] == '\0' || item->targetId[0] == '\0' ||
        item->tolerance < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (book->count >= UMI_OPEN_TMS_RECONCILIATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *item;
    copy.matched = fabs(copy.sourceValue - copy.targetValue) <= copy.tolerance;
    if (copy.matched) {
        book->matchedCount += 1U;
    } else {
        book->breakCount += 1U;
    }
    book->items[book->count++] = copy;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_reconciliation_book_add(
    UmiOpenTmsReconciliationBook *book,
    const UmiOpenTmsReconciliationItem *item)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiOpenTmsReconciliationItem copy;

    if (book == NULL || item == NULL || item->id[0] == '\0' ||
        item->sourceId[0] == '\0' || item->targetId[0] == '\0' ||
        item->tolerance < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (book->count >= UMI_OPEN_TMS_RECONCILIATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *item;
    copy.matched = fabs(copy.sourceValue - copy.targetValue) <= copy.tolerance;
    if (copy.matched) {
        book->matchedCount += 1U;
    } else {
        book->breakCount += 1U;
    }
    book->items[book->count++] = copy;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_reconciliation_book_resolve(
    UmiOpenTmsReconciliationBook *book,
    const char *id)
{
    size_t index;

    if (book == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < book->count; ++index) {
        UmiOpenTmsReconciliationItem *item = &book->items[index];
        if (strcmp(item->id, id) == 0) {
            if (item->resolved) return UMI_STATUS_ALREADY_EXISTS;
            item->resolved = 1;
            if (!item->matched && book->breakCount > 0U) {
                book->breakCount -= 1U;
            }
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

UmiStatus umi_open_tms_reconciliation_book_resolve(
    UmiOpenTmsReconciliationBook *book,
    const char *id)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t index;

    if (book == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < book->count; ++index) {
        UmiOpenTmsReconciliationItem *item = &book->items[index];
        if (strcmp(item->id, id) == 0) {
            if (item->resolved) return UMI_STATUS_ALREADY_EXISTS;
            item->resolved = 1;
            if (!item->matched && book->breakCount > 0U) {
                book->breakCount -= 1U;
            }
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

double umi_open_tms_reconciliation_book_coverage(
    const UmiOpenTmsReconciliationBook *book)
{
    size_t index;
    size_t resolved = 0U;

    if (book == NULL || book->count == 0U) return 0.0;
    for (index = 0U; index < book->count; ++index) {
        if (book->items[index].matched || book->items[index].resolved) {
            resolved += 1U;
        }
    }
    return ((double)resolved / (double)book->count) * 100.0;
}

#endif

UmiStatus umi_open_tms_reconciliation_item_validate(const UmiOpenTmsReconciliationItem *item)
{
    static const UmiSnapshotTextField fields[] = {
        {"id", offsetof(UmiOpenTmsReconciliationItem, id), 64U, 1},
        {"sourceId", offsetof(UmiOpenTmsReconciliationItem, sourceId), 64U, 1},
        {"targetId", offsetof(UmiOpenTmsReconciliationItem, targetId), 64U, 1}
    };
    if (item == NULL || UmiSnapshotValidateTextFields(item, sizeof(*item),
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(item->sourceValue) || !isfinite(item->targetValue) ||
        !isfinite(item->tolerance) || item->tolerance < 0.0 ||
        (item->matched != 0 && item->matched != 1) ||
        (item->resolved != 0 && item->resolved != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus ReconciliationMatch(const UmiOpenTmsReconciliationItem *item, int *out)
{
    double difference;
    UmiStatus status = TmsOpsAdd(item->sourceValue, -item->targetValue, &difference);
    if (status == UMI_STATUS_OK) *out = fabs(difference) <= item->tolerance;
    return status;
}
UmiStatus umi_open_tms_reconciliation_book_validate(const UmiOpenTmsReconciliationBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_RECONCILIATION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    size_t matched = 0U, breaks = 0U;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsReconciliationItem *row = &book->items[i];
        int isMatched;
        if (umi_open_tms_reconciliation_item_validate(row) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        UmiStatus status = ReconciliationMatch(row, &isMatched);
        if (status != UMI_STATUS_OK) return status;
        if (row->matched != isMatched) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(row->id, book->items[j].id) == 0) return UMI_STATUS_INVALID_STATE;
        if (row->matched) ++matched;
        else if (!row->resolved) ++breaks;
    }
    if (matched != book->matchedCount || breaks != book->breakCount) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
void umi_open_tms_reconciliation_book_init(UmiOpenTmsReconciliationBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
UmiStatus umi_open_tms_reconciliation_book_add(UmiOpenTmsReconciliationBook *book,
    const UmiOpenTmsReconciliationItem *item)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_reconciliation_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_reconciliation_item_validate(item);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->items[i].id, item->id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (book->count == UMI_OPEN_TMS_RECONCILIATION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiOpenTmsReconciliationItem copy = *item;
    status = ReconciliationMatch(&copy, &copy.matched);
    if (status != UMI_STATUS_OK) return status;
    /* Recompute matching from observations. A restored resolved break remains
     * resolved; never add it to the unresolved counter merely because it differs. */
    if (copy.matched) ++book->matchedCount;
    else if (!copy.resolved) ++book->breakCount;
    book->items[book->count++] = copy;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_reconciliation_book_resolve(UmiOpenTmsReconciliationBook *book,
    const char *id)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!TmsOpsQuery(id, 64U)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_reconciliation_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsReconciliationItem *row = &book->items[i];
        if (strcmp(row->id, id) == 0) {
            if (row->resolved) return UMI_STATUS_ALREADY_EXISTS;
            row->resolved = 1;
            if (!row->matched) --book->breakCount;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_open_tms_reconciliation_book_coverage_checked(const UmiOpenTmsReconciliationBook *book,
    double *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_reconciliation_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    *out = book->count == 0U ? 0.0 :
        ((double)(book->count - book->breakCount) / (double)book->count) * 100.0;
    return UMI_STATUS_OK;
}
double umi_open_tms_reconciliation_book_coverage(const UmiOpenTmsReconciliationBook *book)
{
    double value;
    return umi_open_tms_reconciliation_book_coverage_checked(book, &value) == UMI_STATUS_OK ? value : 0.0;
}
