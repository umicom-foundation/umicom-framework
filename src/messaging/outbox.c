/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/outbox.c
 *
 * PURPOSE:
 *   Implement an owned-message outbox with pending selection, attempt tracking, completion state and sent-record compaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/outbox.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

typedef struct UmiOutboxRecord {
    uint64_t record_id;
    UmiOwnedMessage message;
    UmiOutboxState state;
    uint32_t attempts;
    UmiStatus last_status;
} UmiOutboxRecord;

struct UmiOutbox {
    UmiSpinLock lock;
    UmiOutboxRecord *records;
    size_t capacity;
    size_t count;
    uint64_t next_id;
};

UmiStatus umi_outbox_create(size_t capacity, UmiOutbox **out_outbox)
{
    UmiOutbox *outbox;
    if (out_outbox == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_outbox = NULL;
    outbox = (UmiOutbox *)calloc(1U, sizeof(*outbox));
    if (outbox == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    outbox->records = (UmiOutboxRecord *)calloc(capacity,
                                                 sizeof(*outbox->records));
    if (outbox->records == NULL) {
        free(outbox);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_spin_init(&outbox->lock);
    outbox->capacity = capacity;
    outbox->next_id = 1U;
    *out_outbox = outbox;
    return UMI_STATUS_OK;
}

void umi_outbox_destroy(UmiOutbox *outbox)
{
    size_t index;
    if (outbox == NULL) return;
    for (index = 0U; index < outbox->count; ++index) {
        umi_message_dispose(&outbox->records[index].message);
    }
    free(outbox->records);
    free(outbox);
}

UmiStatus umi_outbox_enqueue(UmiOutbox *outbox,
                             const UmiMessageEnvelope *message,
                             uint64_t *out_record_id)
{
    UmiOwnedMessage copy;
    UmiStatus status;
    UmiOutboxRecord *record;
    if (outbox == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_message_copy(message, &copy);
    if (status != UMI_STATUS_OK) return status;
    umi_spin_lock(&outbox->lock);
    if (outbox->count >= outbox->capacity) {
        umi_spin_unlock(&outbox->lock);
        umi_message_dispose(&copy);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record = &outbox->records[outbox->count++];
    record->record_id = outbox->next_id++;
    record->message = copy;
    record->state = UMI_OUTBOX_PENDING;
    record->last_status = UMI_STATUS_OK;
    if (out_record_id != NULL) *out_record_id = record->record_id;
    umi_spin_unlock(&outbox->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_outbox_next_pending(UmiOutbox *outbox,
                                  UmiOutboxRecordView *out_record)
{
    size_t index;
    if (outbox == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_spin_lock(&outbox->lock);
    for (index = 0U; index < outbox->count; ++index) {
        UmiOutboxRecord *record = &outbox->records[index];
        if (record->state == UMI_OUTBOX_PENDING) {
            record->state = UMI_OUTBOX_IN_FLIGHT;
            record->attempts++;
            out_record->record_id = record->record_id;
            out_record->message = &record->message.envelope;
            out_record->state = record->state;
            out_record->attempts = record->attempts;
            out_record->last_status = record->last_status;
            umi_spin_unlock(&outbox->lock);
            return UMI_STATUS_OK;
        }
    }
    umi_spin_unlock(&outbox->lock);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_outbox_complete(UmiOutbox *outbox,
                              uint64_t record_id,
                              UmiStatus status,
                              int retryable)
{
    size_t index;
    if (outbox == NULL || record_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_spin_lock(&outbox->lock);
    for (index = 0U; index < outbox->count; ++index) {
        UmiOutboxRecord *record = &outbox->records[index];
        if (record->record_id == record_id) {
            record->last_status = status;
            record->state = status == UMI_STATUS_OK
                ? UMI_OUTBOX_SENT
                : (retryable ? UMI_OUTBOX_PENDING : UMI_OUTBOX_FAILED);
            umi_spin_unlock(&outbox->lock);
            return UMI_STATUS_OK;
        }
    }
    umi_spin_unlock(&outbox->lock);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_outbox_remove_sent(UmiOutbox *outbox, size_t *out_removed)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t removed = 0U;
    if (outbox == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_spin_lock(&outbox->lock);
    for (read_index = 0U; read_index < outbox->count; ++read_index) {
        UmiOutboxRecord *record = &outbox->records[read_index];
        if (record->state == UMI_OUTBOX_SENT) {
            umi_message_dispose(&record->message);
            removed++;
            continue;
        }
        if (write_index != read_index) {
            outbox->records[write_index] = *record;
            (void)memset(record, 0, sizeof(*record));
        }
        write_index++;
    }
    outbox->count = write_index;
    umi_spin_unlock(&outbox->lock);
    if (out_removed != NULL) *out_removed = removed;
    return UMI_STATUS_OK;
}

UmiOutboxStats umi_outbox_stats(const UmiOutbox *outbox)
{
    UmiOutboxStats stats;
    size_t index;
    (void)memset(&stats, 0, sizeof(stats));
    if (outbox == NULL) return stats;
    stats.records = outbox->count;
    for (index = 0U; index < outbox->count; ++index) {
        switch (outbox->records[index].state) {
        case UMI_OUTBOX_SENT: stats.sent++; break;
        case UMI_OUTBOX_FAILED: stats.failed++; break;
        default: stats.pending++; break;
        }
    }
    return stats;
}
