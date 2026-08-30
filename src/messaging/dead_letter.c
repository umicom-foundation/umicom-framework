/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/dead_letter.c
 *
 * PURPOSE:
 *   Implement bounded owned dead-letter retention, indexed inspection and explicit removal after repair or replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/dead_letter.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

typedef struct UmiDeadLetterRecord {
    uint64_t id;
    UmiOwnedMessage message;
    UmiStatus status;
    char *reason;
    uint32_t attempts;
    uint64_t failed_at;
} UmiDeadLetterRecord;

struct UmiDeadLetterStore {
    UmiDeadLetterRecord *records;
    size_t capacity;
    size_t count;
    uint64_t next_id;
};

UmiStatus umi_dead_letter_store_create(size_t capacity,
                                       UmiDeadLetterStore **out_store)
{
    UmiDeadLetterStore *store;
    if (capacity == 0U || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiDeadLetterStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    store->records = (UmiDeadLetterRecord *)calloc(capacity,
                                                   sizeof(*store->records));
    if (store->records == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    store->capacity = capacity;
    store->next_id = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

static void record_dispose(UmiDeadLetterRecord *record)
{
    umi_message_dispose(&record->message);
    free(record->reason);
    (void)memset(record, 0, sizeof(*record));
}

void umi_dead_letter_store_destroy(UmiDeadLetterStore *store)
{
    size_t index;
    if (store == NULL) return;
    for (index = 0U; index < store->count; ++index) {
        record_dispose(&store->records[index]);
    }
    free(store->records);
    free(store);
}

UmiStatus umi_dead_letter_store_add(UmiDeadLetterStore *store,
                                    const UmiMessageEnvelope *message,
                                    UmiStatus status,
                                    const char *reason,
                                    uint32_t attempts,
                                    uint64_t failed_at_nanoseconds,
                                    uint64_t *out_dead_letter_id)
{
    UmiDeadLetterRecord record;
    UmiStatus result;
    if (store == NULL || message == NULL || store->count >= store->capacity) {
        return store != NULL && store->count >= store->capacity
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&record, 0, sizeof(record));
    result = umi_message_copy(message, &record.message);
    if (result != UMI_STATUS_OK) return result;
    record.reason = umi_message_strdup(reason != NULL ? reason :
                                       umi_status_text(status));
    if (record.reason == NULL) {
        umi_message_dispose(&record.message);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    record.id = store->next_id++;
    record.status = status;
    record.attempts = attempts;
    record.failed_at = failed_at_nanoseconds;
    store->records[store->count++] = record;
    if (out_dead_letter_id != NULL) *out_dead_letter_id = record.id;
    return UMI_STATUS_OK;
}

UmiStatus umi_dead_letter_store_get(const UmiDeadLetterStore *store,
                                    size_t index,
                                    UmiDeadLetterView *out_record)
{
    const UmiDeadLetterRecord *record;
    if (store == NULL || out_record == NULL || index >= store->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    record = &store->records[index];
    out_record->dead_letter_id = record->id;
    out_record->message = &record->message.envelope;
    out_record->status = record->status;
    out_record->reason = record->reason;
    out_record->attempts = record->attempts;
    out_record->failed_at_nanoseconds = record->failed_at;
    return UMI_STATUS_OK;
}

UmiStatus umi_dead_letter_store_remove(UmiDeadLetterStore *store,
                                       uint64_t dead_letter_id)
{
    size_t index;
    if (store == NULL || dead_letter_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) {
        if (store->records[index].id == dead_letter_id) {
            record_dispose(&store->records[index]);
            if (index + 1U < store->count) {
                (void)memmove(&store->records[index],
                              &store->records[index + 1U],
                              (store->count - index - 1U) *
                                  sizeof(store->records[0]));
            }
            store->count--;
            (void)memset(&store->records[store->count],
                         0,
                         sizeof(store->records[0]));
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_dead_letter_store_count(const UmiDeadLetterStore *store)
{
    return store != NULL ? store->count : 0U;
}
