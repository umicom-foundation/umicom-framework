/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/message.h
 *
 * PURPOSE:
 *   Define the canonical typed message envelope, ownership flags and deep-copy helpers used by commands, queries, events, streams, jobs and workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_MESSAGE_H
#define UMICOM_MESSAGING_MESSAGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named message kind values accepted by this public contract.
 */
typedef enum UmiMessageKind {
    UMI_MESSAGE_COMMAND = 1,
    UMI_MESSAGE_EVENT = 2,
    UMI_MESSAGE_QUERY = 3,
    UMI_MESSAGE_REPLY = 4,
    UMI_MESSAGE_STREAM = 5,
    UMI_MESSAGE_JOB = 6,
    UMI_MESSAGE_WORKFLOW = 7
} UmiMessageKind;

/**
 * List the named message flags values accepted by this public contract.
 */
typedef enum UmiMessageFlags {
    UMI_MESSAGE_FLAG_NONE = 0U,
    UMI_MESSAGE_FLAG_DURABLE = 1U << 0U,
    UMI_MESSAGE_FLAG_REPLAY = 1U << 1U,
    UMI_MESSAGE_FLAG_IDEMPOTENT = 1U << 2U,
    UMI_MESSAGE_FLAG_REQUIRES_REPLY = 1U << 3U,
    UMI_MESSAGE_FLAG_TEST = 1U << 4U,
    UMI_MESSAGE_FLAG_PAPER = 1U << 5U,
    UMI_MESSAGE_FLAG_LIVE = 1U << 6U
} UmiMessageFlags;

/**
 * Represent the message envelope data shared with callers of this public contract.
 */
typedef struct UmiMessageEnvelope {
    uint32_t structure_size;
    uint32_t schema_version;
    UmiMessageKind kind;
    uint64_t sequence;
    uint64_t correlation_id;
    const char *name;
    const char *payload;

    uint64_t message_id;
    uint64_t causation_id;
    uint64_t created_at_nanoseconds;
    uint64_t available_at_nanoseconds;
    uint32_t delivery_attempt;
    uint32_t flags;
    const char *schema_id;
    const char *source;
    const char *destination;
    const char *partition_key;
    const void *payload_data;
    size_t payload_size;
} UmiMessageEnvelope;

/**
 * Represent the owned message data shared with callers of this public contract.
 */
typedef struct UmiOwnedMessage {
    UmiMessageEnvelope envelope;
    char *name_storage;
    char *payload_storage;
    char *schema_storage;
    char *source_storage;
    char *destination_storage;
    char *partition_storage;
    unsigned char *payload_bytes;
} UmiOwnedMessage;

/**
 * Initialise message envelope from caller-provided values so later operations receive a
 * known state.
 */
void umi_message_envelope_init(UmiMessageEnvelope *message,
                               UmiMessageKind kind,
                               const char *name,
                               const char *payload);
/**
 * Provide the message next id operation used by this module and its client applications.
 */
uint64_t umi_message_next_id(void);
/**
 * Provide the message kind text operation used by this module and its client applications.
 */
const char *umi_message_kind_text(UmiMessageKind kind);
/**
 * Check that message satisfies its contract before another service relies on it.
 */
UmiStatus umi_message_validate(const UmiMessageEnvelope *message);
/**
 * Copy message into module-owned storage so callers keep ownership of their input values.
 */
UmiStatus umi_message_copy(const UmiMessageEnvelope *source,
                           UmiOwnedMessage *destination);
/**
 * Release or reset state held by message so the same storage can be reused safely.
 */
void umi_message_dispose(UmiOwnedMessage *message);

#ifdef __cplusplus
}
#endif

#endif
