/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/inbox.c
 *
 * PURPOSE:
 *   Implement a bounded processed-message set with duplicate counters and remembered consumer outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Initialise inbox from caller-provided values so later operations receive a known state. */
UmiStatus umi_inbox_create(size_t capacity, UmiInbox **out_inbox)
{
    UmiInbox *inbox;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_inbox == NULL || capacity == 0U || capacity > 1048576U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_inbox = NULL;
    inbox = (UmiInbox *)calloc(1U, sizeof(*inbox));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    inbox->entries = (UmiInboxEntry *)calloc(capacity,
                                             sizeof(*inbox->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox->entries == NULL) {
        free(inbox);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_spin_init(&inbox->lock);
    inbox->capacity = capacity;
    *out_inbox = inbox;
    return UMI_STATUS_OK;
}

/* Release or reset state held by inbox so the same storage can be reused safely. */
void umi_inbox_destroy(UmiInbox *inbox)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL) return;
    free(inbox->entries);
    free(inbox);
}

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(const UmiInbox *inbox, uint64_t message_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inbox->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (inbox->entries[index].message_id == message_id) return index;
    }
    return SIZE_MAX;
}

/* Provide the inbox contains operation used by this module and its client applications. */
int umi_inbox_contains(const UmiInbox *inbox, uint64_t message_id)
{
    return inbox != NULL && message_id != 0U &&
           find_entry(inbox, message_id) != SIZE_MAX;
}

/*
 * Provide the inbox mark processed operation used by this module and its client
 * applications.
 */
UmiStatus umi_inbox_mark_processed(UmiInbox *inbox,
                                   uint64_t message_id,
                                   UmiStatus outcome)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL || message_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_spin_lock(&inbox->lock);
    index = find_entry(inbox, message_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX) {
        inbox->duplicates++;
        umi_spin_unlock(&inbox->lock);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the inbox outcome operation used by this module and its client applications. */
UmiStatus umi_inbox_outcome(const UmiInbox *inbox,
                            uint64_t message_id,
                            UmiStatus *out_outcome)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL || out_outcome == NULL || message_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(inbox, message_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_outcome = inbox->entries[index].outcome;
    return UMI_STATUS_OK;
}

/* Provide the inbox stats operation used by this module and its client applications. */
UmiInboxStats umi_inbox_stats(const UmiInbox *inbox)
{
    UmiInboxStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox != NULL) {
        stats.capacity = inbox->capacity;
        stats.processed = inbox->count;
        stats.duplicates = inbox->duplicates;
    }
    return stats;
}
