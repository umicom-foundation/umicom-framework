/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/outbox.h
 *
 * PURPOSE:
 *   Declare a bounded transactional outbox model that owns pending messages until delivery is confirmed or dead-lettered.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named outbox state values accepted by this public contract.
 */
typedef enum UmiOutboxState {
    UMI_OUTBOX_PENDING = 1,
    UMI_OUTBOX_IN_FLIGHT = 2,
    UMI_OUTBOX_SENT = 3,
    UMI_OUTBOX_FAILED = 4
} UmiOutboxState;

/**
 * Represent the outbox data shared with callers of this public contract.
 */
typedef struct UmiOutbox UmiOutbox;

/**
 * Represent the outbox record view data shared with callers of this public contract.
 */
typedef struct UmiOutboxRecordView {
    uint64_t record_id;
    const UmiMessageEnvelope *message;
    UmiOutboxState state;
    uint32_t attempts;
    UmiStatus last_status;
} UmiOutboxRecordView;

/**
 * Represent the outbox stats data shared with callers of this public contract.
 */
typedef struct UmiOutboxStats {
    size_t records;
    size_t pending;
    size_t sent;
    size_t failed;
} UmiOutboxStats;

/**
 * Initialise outbox from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_outbox_create(size_t capacity, UmiOutbox **out_outbox);
/**
 * Release or reset state held by outbox so the same storage can be reused safely.
 */
void umi_outbox_destroy(UmiOutbox *outbox);
/**
 * Provide the outbox enqueue operation used by this module and its client applications.
 */
UmiStatus umi_outbox_enqueue(UmiOutbox *outbox,
                             const UmiMessageEnvelope *message,
                             uint64_t *out_record_id);
/**
 * Provide the outbox next pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_outbox_next_pending(UmiOutbox *outbox,
                                  UmiOutboxRecordView *out_record);
/**
 * Provide the outbox complete operation used by this module and its client applications.
 */
UmiStatus umi_outbox_complete(UmiOutbox *outbox,
                              uint64_t record_id,
                              UmiStatus status,
                              int retryable);
/**
 * Provide the outbox remove sent operation used by this module and its client
 * applications.
 */
UmiStatus umi_outbox_remove_sent(UmiOutbox *outbox, size_t *out_removed);
/**
 * Provide the outbox stats operation used by this module and its client applications.
 */
UmiOutboxStats umi_outbox_stats(const UmiOutbox *outbox);

#ifdef __cplusplus
}
#endif

#endif
