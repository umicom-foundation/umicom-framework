/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/inbox.c
 *
 * PURPOSE:
 *   Implement a bounded processed-message set with duplicate counters and remembered consumer outcomes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/inbox.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

typedef struct UmiInboxEntry {
    uint64_t message_id;
    UmiStatus outcome;
} UmiInboxEntry;

struct UmiInbox {
    UmiSpinLock lock;
    UmiInboxEntry *entries;
    size_t capacity;
    size_t count;
    uint64_t duplicates;
};

UmiStatus umi_inbox_create(size_t capacity, UmiInbox **out_inbox)
{
    UmiInbox *inbox;
    if (out_inbox == NULL || capacity == 0U || capacity > 1048576U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_inbox = NULL;
    inbox = (UmiInbox *)calloc(1U, sizeof(*inbox));
    if (inbox == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    inbox->entries = (UmiInboxEntry *)calloc(capacity,
                                             sizeof(*inbox->entries));
    if (inbox->entries == NULL) {
        free(inbox);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_spin_init(&inbox->lock);
    inbox->capacity = capacity;
    *out_inbox = inbox;
    return UMI_STATUS_OK;
}

void umi_inbox_destroy(UmiInbox *inbox)
{
    if (inbox == NULL) return;
    free(inbox->entries);
    free(inbox);
}

static size_t find_entry(const UmiInbox *inbox, uint64_t message_id)
{
    size_t index;
    for (index = 0U; index < inbox->count; ++index) {
        if (inbox->entries[index].message_id == message_id) return index;
    }
    return SIZE_MAX;
}

int umi_inbox_contains(const UmiInbox *inbox, uint64_t message_id)
{
    return inbox != NULL && message_id != 0U &&
           find_entry(inbox, message_id) != SIZE_MAX;
}

UmiStatus umi_inbox_mark_processed(UmiInbox *inbox,
                                   uint64_t message_id,
                                   UmiStatus outcome)
{
    size_t index;
    if (inbox == NULL || message_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_spin_lock(&inbox->lock);
    index = find_entry(inbox, message_id);
    if (index != SIZE_MAX) {
        inbox->duplicates++;
        umi_spin_unlock(&inbox->lock);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (inbox->count >= inbox->capacity) {
        umi_spin_unlock(&inbox->lock);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    inbox->entries[inbox->count].message_id = message_id;
    inbox->entries[inbox->count].outcome = outcome;
    inbox->count++;
    umi_spin_unlock(&inbox->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_inbox_outcome(const UmiInbox *inbox,
                            uint64_t message_id,
                            UmiStatus *out_outcome)
{
    size_t index;
    if (inbox == NULL || out_outcome == NULL || message_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(inbox, message_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_outcome = inbox->entries[index].outcome;
    return UMI_STATUS_OK;
}

UmiInboxStats umi_inbox_stats(const UmiInbox *inbox)
{
    UmiInboxStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    if (inbox != NULL) {
        stats.capacity = inbox->capacity;
        stats.processed = inbox->count;
        stats.duplicates = inbox->duplicates;
    }
    return stats;
}
