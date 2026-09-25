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
#include "umicom/open_tms/reconciliation_book.h"

#include <math.h>
#include <string.h>

void umi_open_tms_reconciliation_book_init(
    UmiOpenTmsReconciliationBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

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
