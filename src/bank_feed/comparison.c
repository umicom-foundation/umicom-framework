/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/comparison.c
 *
 * PURPOSE:
 *   Match immutable imported observations by their provider identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "comparison_private.h"
#include <stdlib.h>
#include <string.h>

typedef struct BankComparisonKey {
    const UmiBankFeedRow *row;
    size_t index;
} BankComparisonKey;

static int CompareKeys(const void *left, const void *right) {
    const BankComparisonKey *a = left, *b = right;
    int order = strcmp(a->row->account, b->row->account);
    return order != 0 ? order : strcmp(a->row->transaction, b->row->transaction);
}

/* Index references are temporary and point only into owned copies. Sorting
 * them leaves original import order intact for row identities and reporting. */
static UmiStatus Capture(const UmiBankFeed *feed, UmiBankFeedSummary *summary,
                         UmiBankFeedRow **rows) {
    UmiStatus status = UmiBankFeedRead(feed, summary);
    if (status != UMI_STATUS_OK)
        return status;
    if (summary->rows > UMI_BANK_FEED_MAX_ROWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (summary->rows == 0U)
        return UMI_STATUS_OK;
    *rows = calloc(summary->rows, sizeof **rows);
    if (*rows == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary->rows; ++i)
        status = UmiBankFeedAt(feed, i, &(*rows)[i]);
    return status;
}

static UmiStatus Differences(const UmiBankFeedRow *before,
                            const UmiBankFeedRow *after, uint32_t *out) {
    int comparison = 0;
    UmiStatus status = UmiDecimalCompare(before->amount, after->amount, &comparison);
    if (status != UMI_STATUS_OK)
        return status;
    uint32_t changes = comparison == 0 ? 0U : UMI_BANK_FEED_CHANGE_AMOUNT;
#define TEXT(field, flag) do { if (strcmp(before->field, after->field) != 0) changes |= flag; } while (0)
    TEXT(amountText, UMI_BANK_FEED_CHANGE_AMOUNT_TEXT);
    TEXT(currency, UMI_BANK_FEED_CHANGE_CURRENCY);
    TEXT(bookingTime, UMI_BANK_FEED_CHANGE_BOOKING_TIME);
    TEXT(reference, UMI_BANK_FEED_CHANGE_REFERENCE);
    TEXT(information, UMI_BANK_FEED_CHANGE_INFORMATION);
#undef TEXT
    if (before->credit != after->credit) changes |= UMI_BANK_FEED_CHANGE_DIRECTION;
    if (before->booked != after->booked) changes |= UMI_BANK_FEED_CHANGE_STATUS;
    if (before->mutableRecord != after->mutableRecord) changes |= UMI_BANK_FEED_CHANGE_MUTABILITY;
    *out = changes;
    return UMI_STATUS_OK;
}

static void Append(UmiBankFeedComparison *owner, BankComparisonEntry entry) {
    owner->entries[owner->summary.rows++] = entry;
    switch (entry.kind) {
    case UMI_BANK_FEED_SAME: ++owner->summary.same; break;
    case UMI_BANK_FEED_CHANGED: ++owner->summary.changed; break;
    case UMI_BANK_FEED_BEFORE_ONLY: ++owner->summary.beforeOnly; break;
    case UMI_BANK_FEED_AFTER_ONLY: ++owner->summary.afterOnly; break;
    case UMI_BANK_FEED_BEFORE_UNIDENTIFIED: ++owner->summary.beforeUnidentified; break;
    case UMI_BANK_FEED_AFTER_UNIDENTIFIED: ++owner->summary.afterUnidentified; break;
    }
}

void UmiBankFeedComparisonDestroy(UmiBankFeedComparison *owner) {
    if (owner == NULL)
        return;
    free(owner->entries);
    free(owner->after);
    free(owner->before);
    free(owner);
}

UmiStatus UmiBankFeedComparisonCreate(const UmiBankFeed *before,
                                     const UmiBankFeed *after,
                                     UmiBankFeedComparison **out) {
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (before == NULL || after == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankFeedComparison *owner = calloc(1U, sizeof *owner);
    if (owner == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = Capture(before, &owner->summary.before, &owner->before);
    if (status == UMI_STATUS_OK)
        status = Capture(after, &owner->summary.after, &owner->after);
    const size_t count = owner->summary.before.rows + owner->summary.after.rows;
    BankComparisonKey *keys = NULL;
    bool *matched = NULL;
    if (status == UMI_STATUS_OK && count != 0U) {
        owner->entries = calloc(count, sizeof *owner->entries);
        /* Keep empty-after handling uniform without passing a null array to
         * qsort/bsearch. Counts remain zero for the empty import. */
        size_t slots = owner->summary.after.rows == 0U ? 1U : owner->summary.after.rows;
        keys = calloc(slots, sizeof *keys);
        matched = calloc(slots, sizeof *matched);
        if (owner->entries == NULL || keys == NULL || matched == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t keyed = 0U;
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < owner->summary.after.rows; ++i) {
            if (owner->after[i].transaction[0] != '\0')
                keys[keyed++] = (BankComparisonKey){&owner->after[i], i};
        }
        if (keyed > 1U)
            qsort(keys, keyed, sizeof *keys, CompareKeys);
        for (size_t i = 0U; status == UMI_STATUS_OK && i < owner->summary.before.rows; ++i) {
            BankComparisonEntry entry = {i, SIZE_MAX, UMI_BANK_FEED_BEFORE_ONLY, 0U};
            if (owner->before[i].transaction[0] == '\0')
                entry.kind = UMI_BANK_FEED_BEFORE_UNIDENTIFIED;
            else if (keyed != 0U) {
                BankComparisonKey needle = {&owner->before[i], i};
                const BankComparisonKey *found = bsearch(&needle, keys, keyed, sizeof *keys, CompareKeys);
                if (found != NULL) {
                    entry.after = found->index;
                    matched[found->index] = true;
                    status = Differences(&owner->before[i], found->row, &entry.changes);
                    entry.kind = entry.changes == 0U ? UMI_BANK_FEED_SAME : UMI_BANK_FEED_CHANGED;
                }
            }
            if (status == UMI_STATUS_OK)
                Append(owner, entry);
        }
        for (size_t i = 0U; status == UMI_STATUS_OK && i < owner->summary.after.rows; ++i) {
            if (!matched[i]) {
                UmiBankFeedComparisonKind kind = owner->after[i].transaction[0] == '\0'
                    ? UMI_BANK_FEED_AFTER_UNIDENTIFIED : UMI_BANK_FEED_AFTER_ONLY;
                Append(owner, (BankComparisonEntry){SIZE_MAX, i, kind, 0U});
            }
        }
    }
    free(matched);
    free(keys);
    if (status == UMI_STATUS_OK)
        *out = owner;
    else
        UmiBankFeedComparisonDestroy(owner);
    return status;
}

UmiStatus UmiBankFeedComparisonRead(const UmiBankFeedComparison *owner,
                                   UmiBankFeedComparisonSummary *out) {
    if (owner == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = owner->summary;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankFeedComparisonAt(const UmiBankFeedComparison *owner,
                                 size_t index, UmiBankFeedComparisonRow *out) {
    if (owner == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= owner->summary.rows)
        return UMI_STATUS_NOT_FOUND;
    const BankComparisonEntry *entry = &owner->entries[index];
    UmiBankFeedComparisonRow row = {0};
    row.kind = entry->kind;
    row.changes = entry->changes;
    row.beforeIndex = entry->before;
    row.afterIndex = entry->after;
    if (entry->before != SIZE_MAX) row.before = owner->before[entry->before];
    if (entry->after != SIZE_MAX) row.after = owner->after[entry->after];
    *out = row;
    return UMI_STATUS_OK;
}

const char *UmiBankFeedComparisonKindName(UmiBankFeedComparisonKind kind) {
    switch (kind) {
    case UMI_BANK_FEED_SAME: return "same";
    case UMI_BANK_FEED_CHANGED: return "changed";
    case UMI_BANK_FEED_BEFORE_ONLY: return "before-only";
    case UMI_BANK_FEED_AFTER_ONLY: return "after-only";
    case UMI_BANK_FEED_BEFORE_UNIDENTIFIED: return "before-unidentified";
    case UMI_BANK_FEED_AFTER_UNIDENTIFIED: return "after-unidentified";
    }
    return NULL;
}
