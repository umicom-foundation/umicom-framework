/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/message_store.c
 *
 * PURPOSE:
 *   Implement ordered durable message serialisation using hexadecimal payloads and expose the store through the generic replay-source contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/message_store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/data_internal.h"

struct UmiMessageStore {
    UmiStore store;
    char prefix[160];
    uint64_t next_sequence;
    size_t count;
};

static UmiStatus metadata_key(const UmiMessageStore *store,
                              const char *name,
                              char *out_key,
                              size_t capacity)
{
    return umi_data_key(out_key, capacity, store->prefix, name);
}

static void load_message_count(UmiMessageStore *store)
{
    char key[320];
    char value[64];
    char *end = NULL;
    unsigned long long parsed;
    if (metadata_key(store, "meta.count", key, sizeof(key)) != UMI_STATUS_OK ||
        umi_store_get(&store->store, key, value, sizeof(value)) != UMI_STATUS_OK) {
        return;
    }
    parsed = strtoull(value, &end, 10);
    if (end != value && *end == '\0') {
        store->count = (size_t)parsed;
        store->next_sequence = (uint64_t)store->count + 1U;
    }
}

static UmiStatus save_message_count(UmiMessageStore *store)
{
    char key[320];
    char value[64];
    UmiStatus status = metadata_key(store, "meta.count", key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(value, sizeof(value), "%zu", store->count);
    return umi_store_set(&store->store, key, value);
}

static UmiStatus message_key(const UmiMessageStore *store,
                             uint64_t sequence,
                             char *out_key,
                             size_t capacity)
{
    char suffix[32];
    (void)snprintf(suffix, sizeof(suffix), "%020llu",
                   (unsigned long long)sequence);
    return umi_data_key(out_key, capacity, store->prefix, suffix);
}

UmiStatus umi_message_store_create(const UmiStore *store,
                                   const char *namespace_name,
                                   UmiMessageStore **out_message_store)
{
    UmiMessageStore *message_store;
    int written;
    if (store == NULL || namespace_name == NULL || out_message_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_message_store = NULL;
    message_store = (UmiMessageStore *)calloc(1U, sizeof(*message_store));
    if (message_store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    message_store->store = *store;
    written = snprintf(message_store->prefix,
                       sizeof(message_store->prefix),
                       "message.%s.",
                       namespace_name);
    if (written < 0 || (size_t)written >= sizeof(message_store->prefix)) {
        free(message_store);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    message_store->next_sequence = 1U;
    load_message_count(message_store);
    *out_message_store = message_store;
    return UMI_STATUS_OK;
}

void umi_message_store_destroy(UmiMessageStore *message_store)
{
    free(message_store);
}

UmiStatus umi_message_store_append(UmiMessageStore *message_store,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence)
{
    char key[320];
    char *payload_hex = NULL;
    char *text;
    size_t text_capacity;
    int written;
    uint64_t sequence;
    UmiStatus status;
    const void *payload_data;
    size_t payload_size;
    if (message_store == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_message_validate(message);
    if (status != UMI_STATUS_OK) return status;
    sequence = message_store->next_sequence++;
    status = message_key(message_store, sequence, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    payload_data = message->payload_data != NULL
        ? message->payload_data : (const void *)message->payload;
    payload_size = message->payload_size;
    if (payload_size == 0U && message->payload != NULL) {
        payload_size = strlen(message->payload);
    }
    status = umi_hex_encode(payload_data, payload_size, &payload_hex);
    if (status != UMI_STATUS_OK) return status;
    text_capacity = strlen(payload_hex) + 2048U;
    text = (char *)malloc(text_capacity);
    if (text == NULL) {
        free(payload_hex);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    written = snprintf(text,
                       text_capacity,
                       "%llu\t%u\t%u\t%llu\t%llu\t%llu\t%llu\t%u\t%s\t%s\t%s\t%s\t%s\t%s",
                       (unsigned long long)message->message_id,
                       (unsigned)message->kind,
                       message->schema_version,
                       (unsigned long long)message->correlation_id,
                       (unsigned long long)message->causation_id,
                       (unsigned long long)message->created_at_nanoseconds,
                       (unsigned long long)message->available_at_nanoseconds,
                       message->flags,
                       message->name != NULL ? message->name : "",
                       message->schema_id != NULL ? message->schema_id : "",
                       message->source != NULL ? message->source : "",
                       message->destination != NULL ? message->destination : "",
                       message->partition_key != NULL ? message->partition_key : "",
                       payload_hex);
    if (written < 0 || (size_t)written >= text_capacity) {
        free(payload_hex);
        free(text);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_store_set(&message_store->store, key, text);
    if (status == UMI_STATUS_OK) {
        message_store->count++;
        status = save_message_count(message_store);
        if (status == UMI_STATUS_OK && out_sequence != NULL) {
            *out_sequence = sequence;
        }
    }
    free(payload_hex);
    free(text);
    return status;
}

static UmiStatus parse_message(char *text,
                               uint64_t sequence,
                               UmiOwnedMessage *out_message)
{
    char *fields[14];
    size_t count = 0U;
    char *cursor = text;
    UmiMessageEnvelope message;
    unsigned char *payload = NULL;
    size_t payload_size = 0U;
    UmiStatus status;
    while (count < 14U) {
        char *separator;
        fields[count++] = cursor;
        if (count == 14U) break;
        separator = strchr(cursor, '\t');
        if (separator == NULL) return UMI_STATUS_PARSE_ERROR;
        *separator = '\0';
        cursor = separator + 1;
    }
    if (strchr(fields[13], '\t') != NULL) return UMI_STATUS_PARSE_ERROR;
    (void)memset(&message, 0, sizeof(message));
    message.structure_size = (uint32_t)sizeof(message);
    message.message_id = (uint64_t)strtoull(fields[0], NULL, 10);
    message.kind = (UmiMessageKind)strtoul(fields[1], NULL, 10);
    message.schema_version = (uint32_t)strtoul(fields[2], NULL, 10);
    message.correlation_id = (uint64_t)strtoull(fields[3], NULL, 10);
    message.causation_id = (uint64_t)strtoull(fields[4], NULL, 10);
    message.created_at_nanoseconds = (uint64_t)strtoull(fields[5], NULL, 10);
    message.available_at_nanoseconds = (uint64_t)strtoull(fields[6], NULL, 10);
    message.flags = (uint32_t)strtoul(fields[7], NULL, 10);
    message.name = fields[8];
    message.schema_id = fields[9];
    message.source = fields[10];
    message.destination = fields[11];
    message.partition_key = fields[12];
    message.sequence = sequence;
    status = umi_hex_decode(fields[13], &payload, &payload_size);
    if (status != UMI_STATUS_OK) return status;
    message.payload_data = payload;
    message.payload_size = payload_size;
    message.payload = payload_size > 0U ? (const char *)payload : "";
    status = umi_message_copy(&message, out_message);
    free(payload);
    return status;
}

UmiStatus umi_message_store_read(const UmiMessageStore *message_store,
                                 size_t index,
                                 UmiOwnedMessage *out_message)
{
    char key[320];
    char *text;
    UmiStatus status;
    uint64_t sequence;
    if (message_store == NULL || out_message == NULL ||
        index >= message_store->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    sequence = (uint64_t)index + 1U;
    status = message_key(message_store, sequence, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    text = (char *)malloc(16384U);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_store_get(&message_store->store, key, text, 16384U);
    if (status == UMI_STATUS_OK) {
        status = parse_message(text, sequence, out_message);
    }
    free(text);
    return status;
}

size_t umi_message_store_count(const UmiMessageStore *message_store)
{
    return message_store != NULL ? message_store->count : 0U;
}

static size_t replay_count(void *instance)
{
    return umi_message_store_count((UmiMessageStore *)instance);
}

static UmiStatus replay_read(void *instance,
                             size_t index,
                             UmiOwnedMessage *out_message)
{
    return umi_message_store_read((UmiMessageStore *)instance,
                                  index,
                                  out_message);
}

UmiReplaySource umi_message_store_replay_source(UmiMessageStore *message_store)
{
    UmiReplaySource source;
    source.instance = message_store;
    source.count = replay_count;
    source.read = replay_read;
    return source;
}
