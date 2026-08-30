/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/persistence_codec.c
 *
 * PURPOSE:
 *   Implement line-safe hexadecimal chunk persistence through Session Store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/persistence_codec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char HEX[] = "0123456789abcdef";

static int hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

static UmiStatus chunk_key(
    const char *prefix,
    size_t index,
    char *out_key,
    size_t capacity)
{
    int written;

    written = snprintf(
        out_key,
        capacity,
        "%s.chunk.%zu",
        prefix,
        index);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus encode_chunk(
    const unsigned char *data,
    size_t length,
    char *out_value,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;

    if (out_value == NULL || capacity < 4U ||
        (data == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (3U + length * 2U + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    out_value[used++] = 'H';
    out_value[used++] = '1';
    out_value[used++] = ':';

    for (index = 0U; index < length; ++index) {
        out_value[used++] = HEX[(data[index] >> 4U) & 0x0fU];
        out_value[used++] = HEX[data[index] & 0x0fU];
    }

    out_value[used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus decode_chunk(
    const char *value,
    unsigned char *out_data,
    size_t capacity,
    size_t *out_length)
{
    size_t encoded_length;
    size_t length;
    size_t index;

    if (value == NULL || out_data == NULL || out_length == NULL ||
        strncmp(value, "H1:", 3U) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    encoded_length = strlen(value + 3U);
    if ((encoded_length & 1U) != 0U) return UMI_STATUS_PARSE_ERROR;

    length = encoded_length / 2U;
    if (length > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    for (index = 0U; index < length; ++index) {
        const int high = hex_value(value[3U + index * 2U]);
        const int low = hex_value(value[4U + index * 2U]);

        if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;

        out_data[index] =
            (unsigned char)(((unsigned)high << 4U) | (unsigned)low);
    }

    *out_length = length;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_persistence_save_text(
    UmiSessionStore *store,
    const char *key_prefix,
    const char *text,
    size_t length,
    size_t *out_chunk_count)
{
    size_t offset = 0U;
    size_t chunk_count = 0U;

    if (store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0' ||
        (text == NULL && length > 0U) ||
        out_chunk_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (offset < length || (length == 0U && chunk_count == 0U)) {
        const size_t remaining = length - offset;
        const size_t count =
            remaining > UMI_AI_DEVELOPER_PERSISTENCE_CHUNK_BYTES
                ? UMI_AI_DEVELOPER_PERSISTENCE_CHUNK_BYTES
                : remaining;
        char key[UMI_SESSION_KEY_CAPACITY];
        char value[UMI_SESSION_VALUE_CAPACITY];
        UmiStatus status;

        if (chunk_count >= UMI_AI_DEVELOPER_PERSISTENCE_MAX_CHUNKS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = chunk_key(
            key_prefix,
            chunk_count,
            key,
            sizeof(key));
        if (status != UMI_STATUS_OK) return status;

        status = encode_chunk(
            (const unsigned char *)(text != NULL ? text + offset : ""),
            count,
            value,
            sizeof(value));
        if (status != UMI_STATUS_OK) return status;

        status = umi_session_store_set(store, key, value);
        if (status != UMI_STATUS_OK) return status;

        chunk_count += 1U;
        offset += count;

        if (length == 0U) break;
    }

    *out_chunk_count = chunk_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_persistence_load_text(
    const UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    size_t index;
    size_t used = 0U;

    if (store == NULL || key_prefix == NULL ||
        out_text == NULL || capacity == 0U ||
        out_length == NULL ||
        chunk_count > UMI_AI_DEVELOPER_PERSISTENCE_MAX_CHUNKS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';
    *out_length = 0U;

    for (index = 0U; index < chunk_count; ++index) {
        char key[UMI_SESSION_KEY_CAPACITY];
        char value[UMI_SESSION_VALUE_CAPACITY];
        unsigned char decoded[UMI_AI_DEVELOPER_PERSISTENCE_CHUNK_BYTES];
        size_t decoded_length = 0U;
        UmiStatus status;

        status = chunk_key(key_prefix, index, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;

        status = umi_session_store_get(
            store,
            key,
            value,
            sizeof(value));
        if (status != UMI_STATUS_OK) return status;

        status = decode_chunk(
            value,
            decoded,
            sizeof(decoded),
            &decoded_length);
        if (status != UMI_STATUS_OK) return status;

        if (used + decoded_length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        if (decoded_length > 0U) {
            (void)memcpy(out_text + used, decoded, decoded_length);
            used += decoded_length;
        }
    }

    out_text[used] = '\0';
    *out_length = used;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_persistence_remove_text(
    UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count)
{
    size_t index;

    if (store == NULL || key_prefix == NULL ||
        chunk_count > UMI_AI_DEVELOPER_PERSISTENCE_MAX_CHUNKS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < chunk_count; ++index) {
        char key[UMI_SESSION_KEY_CAPACITY];
        UmiStatus status = chunk_key(
            key_prefix,
            index,
            key,
            sizeof(key));

        if (status != UMI_STATUS_OK) return status;

        status = umi_session_store_remove(store, key);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_persistence_set_uint64(
    UmiSessionStore *store,
    const char *key,
    uint64_t value)
{
    char text[32];
    int written;

    if (store == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        text,
        sizeof(text),
        "%llu",
        (unsigned long long)value);

    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return umi_session_store_set(store, key, text);
}

UmiStatus umi_ai_developer_persistence_get_uint64(
    const UmiSessionStore *store,
    const char *key,
    uint64_t default_value,
    uint64_t *out_value)
{
    char text[32];
    char *end = NULL;
    unsigned long long value;
    UmiStatus status;

    if (store == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_session_store_get(store, key, text, sizeof(text));
    if (status == UMI_STATUS_NOT_FOUND) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;

    value = strtoull(text, &end, 10);
    if (end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;

    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}
