/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/outbox.h
 *
 * PURPOSE:
 *   Declare a bounded transactional outbox model that owns pending messages until delivery is confirmed or dead-lettered.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_OUTBOX_H
#define UMICOM_MESSAGING_OUTBOX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiOutboxState {
    UMI_OUTBOX_PENDING = 1,
    UMI_OUTBOX_IN_FLIGHT = 2,
    UMI_OUTBOX_SENT = 3,
    UMI_OUTBOX_FAILED = 4
} UmiOutboxState;

typedef struct UmiOutbox UmiOutbox;

typedef struct UmiOutboxRecordView {
    uint64_t record_id;
    const UmiMessageEnvelope *message;
    UmiOutboxState state;
    uint32_t attempts;
    UmiStatus last_status;
} UmiOutboxRecordView;

typedef struct UmiOutboxStats {
    size_t records;
    size_t pending;
    size_t sent;
    size_t failed;
} UmiOutboxStats;

UmiStatus umi_outbox_create(size_t capacity, UmiOutbox **out_outbox);
void umi_outbox_destroy(UmiOutbox *outbox);
UmiStatus umi_outbox_enqueue(UmiOutbox *outbox,
                             const UmiMessageEnvelope *message,
                             uint64_t *out_record_id);
UmiStatus umi_outbox_next_pending(UmiOutbox *outbox,
                                  UmiOutboxRecordView *out_record);
UmiStatus umi_outbox_complete(UmiOutbox *outbox,
                              uint64_t record_id,
                              UmiStatus status,
                              int retryable);
UmiStatus umi_outbox_remove_sent(UmiOutbox *outbox, size_t *out_removed);
UmiOutboxStats umi_outbox_stats(const UmiOutbox *outbox);

#ifdef __cplusplus
}
#endif

#endif
