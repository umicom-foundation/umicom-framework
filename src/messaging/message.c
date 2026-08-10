/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/message.c
 *
 * PURPOSE:
 *   Implement canonical message initialisation, validation, unique identifiers and owned deep copies for safe queueing, replay and persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/message.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

static atomic_uint_fast64_t g_next_message_id = 1U;

void umi_message_envelope_init(UmiMessageEnvelope *message,
                               UmiMessageKind kind,
                               const char *name,
                               const char *payload)
{
    if (message == NULL) return;
    (void)memset(message, 0, sizeof(*message));
    message->structure_size = (uint32_t)sizeof(*message);
    message->schema_version = 1U;
    message->kind = kind;
    message->message_id = umi_message_next_id();
    message->name = name != NULL ? name : "";
    message->payload = payload != NULL ? payload : "";
    message->payload_data = message->payload;
    message->payload_size = strlen(message->payload);
}

uint64_t umi_message_next_id(void)
{
    return atomic_fetch_add_explicit(&g_next_message_id,
                                     1U,
                                     memory_order_relaxed);
}

const char *umi_message_kind_text(UmiMessageKind kind)
{
    switch (kind) {
    case UMI_MESSAGE_COMMAND: return "command";
    case UMI_MESSAGE_EVENT: return "event";
    case UMI_MESSAGE_QUERY: return "query";
    case UMI_MESSAGE_REPLY: return "reply";
    case UMI_MESSAGE_STREAM: return "stream";
    case UMI_MESSAGE_JOB: return "job";
    case UMI_MESSAGE_WORKFLOW: return "workflow";
    default: return "unknown";
    }
}

UmiStatus umi_message_validate(const UmiMessageEnvelope *message)
{
    if (message == NULL || message->name == NULL || message->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (message->structure_size < offsetof(UmiMessageEnvelope, message_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (message->kind < UMI_MESSAGE_COMMAND ||
        message->kind > UMI_MESSAGE_WORKFLOW) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (message->payload_size > 0U && message->payload_data == NULL &&
        message->payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus copy_string(const char *source, char **out_value)
{
    *out_value = umi_message_strdup(source);
    return *out_value != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

UmiStatus umi_message_copy(const UmiMessageEnvelope *source,
                           UmiOwnedMessage *destination)
{
    UmiStatus status;
    const void *payload_data;
    size_t payload_size;

    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_message_validate(source);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(destination, 0, sizeof(*destination));
    destination->envelope = *source;

#define COPY_TEXT(member, storage)                                           \
    do {                                                                     \
        status = copy_string((member) != NULL ? (member) : "",              \
                             &(storage));                                     \
        if (status != UMI_STATUS_OK) {                                       \
            umi_message_dispose(destination);                                \
            return status;                                                   \
        }                                                                    \
    } while (0)

    COPY_TEXT(source->name, destination->name_storage);
    COPY_TEXT(source->payload, destination->payload_storage);
    COPY_TEXT(source->schema_id, destination->schema_storage);
    COPY_TEXT(source->source, destination->source_storage);
    COPY_TEXT(source->destination, destination->destination_storage);
    COPY_TEXT(source->partition_key, destination->partition_storage);
#undef COPY_TEXT

    payload_data = source->payload_data != NULL
        ? source->payload_data
        : (const void *)source->payload;
    payload_size = source->payload_size;
    if (payload_size == 0U && source->payload != NULL) {
        payload_size = strlen(source->payload);
    }
    if (payload_size > 0U) {
        destination->payload_bytes =
            (unsigned char *)umi_message_memdup(payload_data, payload_size);
        if (destination->payload_bytes == NULL) {
            umi_message_dispose(destination);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }

    destination->envelope.name = destination->name_storage;
    destination->envelope.payload = destination->payload_storage;
    destination->envelope.schema_id = destination->schema_storage;
    destination->envelope.source = destination->source_storage;
    destination->envelope.destination = destination->destination_storage;
    destination->envelope.partition_key = destination->partition_storage;
    destination->envelope.payload_data = payload_size > 0U
        ? destination->payload_bytes
        : (const void *)destination->payload_storage;
    destination->envelope.payload_size = payload_size;
    destination->envelope.structure_size =
        (uint32_t)sizeof(destination->envelope);
    return UMI_STATUS_OK;
}

void umi_message_dispose(UmiOwnedMessage *message)
{
    if (message == NULL) return;
    free(message->name_storage);
    free(message->payload_storage);
    free(message->schema_storage);
    free(message->source_storage);
    free(message->destination_storage);
    free(message->partition_storage);
    free(message->payload_bytes);
    (void)memset(message, 0, sizeof(*message));
}
