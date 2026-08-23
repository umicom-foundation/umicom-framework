/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/session_service.c
 *
 * PURPOSE:
 *   Encode, chunk, restore and remove context-host assignment state without
 *   exceeding Session Store key/value limits or violating its line-safe value
 *   contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/session_service.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_WCH_SESSION_CHUNK_PREFIX "H1:"
#define UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH 3U

static UmiStatus make_key(
    char *out_key,
    size_t capacity,
    const char *prefix,
    const char *suffix)
{
    int written;

    if (out_key == NULL || prefix == NULL || suffix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_key, capacity, "%s.%s", prefix, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus make_chunk_key(
    char *out_key,
    size_t capacity,
    const char *prefix,
    size_t index)
{
    char suffix[32U];
    int written;

    written = snprintf(suffix, sizeof(suffix), "chunk.%zu", index);
    if (written < 0 || (size_t)written >= sizeof(suffix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return make_key(out_key, capacity, prefix, suffix);
}

/*
 * Encode one raw session chunk into a single-line hexadecimal Session Store
 * value. An explicit format marker lets restore validate the representation
 * instead of guessing whether arbitrary text is encoded.
 */
static UmiStatus encode_chunk(
    const char *input,
    size_t input_length,
    char *out_text,
    size_t out_capacity)
{
    static const char digits[] = "0123456789ABCDEF";
    size_t required;
    size_t index;
    size_t output_index = UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH;

    if (input == NULL || out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (input_length >
        (SIZE_MAX - UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH - 1U) / 2U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    required =
        UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH + (input_length * 2U) + 1U;
    if (required > out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    memcpy(
        out_text,
        UMI_WCH_SESSION_CHUNK_PREFIX,
        UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH);

    for (index = 0U; index < input_length; ++index) {
        const unsigned char value = (unsigned char)input[index];

        out_text[output_index++] = digits[(value >> 4U) & 0x0FU];
        out_text[output_index++] = digits[value & 0x0FU];
    }

    out_text[output_index] = '\0';
    return UMI_STATUS_OK;
}

static int hex_value(char character)
{
    if (character >= '0' && character <= '9') {
        return (int)(character - '0');
    }
    if (character >= 'A' && character <= 'F') {
        return 10 + (int)(character - 'A');
    }
    if (character >= 'a' && character <= 'f') {
        return 10 + (int)(character - 'a');
    }
    return -1;
}

/*
 * Decode the current H1 hexadecimal representation.
 *
 * A legacy plain value is accepted defensively. The previous implementation
 * could not persist the normal multi-line session successfully, but accepting
 * a bounded plain chunk avoids rejecting any manually-created historical store.
 */
static UmiStatus decode_chunk(
    const char *stored_text,
    char *out_chunk,
    size_t out_capacity,
    size_t *out_length)
{
    size_t stored_length;

    if (stored_text == NULL || out_chunk == NULL ||
        out_capacity == 0U || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_length = 0U;
    stored_length = strlen(stored_text);

    if (stored_length >= UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH &&
        strncmp(
            stored_text,
            UMI_WCH_SESSION_CHUNK_PREFIX,
            UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH) == 0) {
        const char *hex_text =
            stored_text + UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH;
        const size_t hex_length =
            stored_length - UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH;
        const size_t decoded_length = hex_length / 2U;
        size_t index;

        if ((hex_length % 2U) != 0U) {
            return UMI_STATUS_PARSE_ERROR;
        }
        if (decoded_length + 1U > out_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        for (index = 0U; index < decoded_length; ++index) {
            const int high = hex_value(hex_text[index * 2U]);
            const int low = hex_value(hex_text[(index * 2U) + 1U]);

            if (high < 0 || low < 0) {
                return UMI_STATUS_PARSE_ERROR;
            }

            out_chunk[index] =
                (char)((unsigned int)(high << 4) | (unsigned int)low);
        }

        out_chunk[decoded_length] = '\0';
        *out_length = decoded_length;
        return UMI_STATUS_OK;
    }

    if (stored_length + 1U > out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    memcpy(out_chunk, stored_text, stored_length + 1U);
    *out_length = stored_length;
    return UMI_STATUS_OK;
}

/*
 * Count metadata is generated by this module, but it is still treated as
 * untrusted persistent input during restore/remove. Requiring decimal digits
 * only avoids accepting whitespace, signs or partially parsed values.
 */
static UmiStatus parse_chunk_count(
    const char *text,
    size_t *out_count)
{
    size_t value = 0U;
    size_t index;

    if (text == NULL || out_count == NULL || text[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    for (index = 0U; text[index] != '\0'; ++index) {
        const unsigned char character = (unsigned char)text[index];
        size_t digit;

        if (character < (unsigned char)'0' ||
            character > (unsigned char)'9') {
            return UMI_STATUS_PARSE_ERROR;
        }

        digit = (size_t)(character - (unsigned char)'0');

        if (value >
            (UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS - digit) / 10U) {
            return UMI_STATUS_PARSE_ERROR;
        }

        value = (value * 10U) + digit;
    }

    if (value > UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_count = value;
    return UMI_STATUS_OK;
}

static void remove_partial_chunks(
    UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count)
{
    size_t index;

    if (store == NULL || key_prefix == NULL) {
        return;
    }

    for (index = 0U; index < chunk_count; ++index) {
        char key[UMI_SESSION_KEY_CAPACITY];

        if (make_chunk_key(
                key, sizeof(key), key_prefix, index) == UMI_STATUS_OK) {
            const UmiStatus status = umi_session_store_remove(store, key);
            (void)status;
        }
    }
}

UmiStatus umi_workbench_context_host_session_save(
    const UmiWorkbenchContextHost *host,
    UmiSessionStore *store,
    const char *key_prefix)
{
    UmiWorkbenchContextHostSession session;
    char *encoded;
    size_t encoded_length;
    size_t offset = 0U;
    size_t chunk_count = 0U;
    char key[UMI_SESSION_KEY_CAPACITY];
    char count_text[32U];
    UmiStatus status;

    if (host == NULL || store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    encoded = (char *)calloc(
        UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY, 1U);
    if (encoded == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_workbench_context_host_session_capture(host, &session);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_encode(
            &session,
            encoded,
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY);
    }
    if (status != UMI_STATUS_OK) {
        free(encoded);
        return status;
    }

    /*
     * Remove an older complete value before writing the new generation. The
     * count key is always written last, so a failed save cannot make a partial
     * generation appear complete to restore.
     */
    status = umi_workbench_context_host_session_remove(store, key_prefix);
    if (status != UMI_STATUS_OK) {
        free(encoded);
        return status;
    }

    encoded_length = strlen(encoded);
    while (offset < encoded_length) {
        char stored_chunk[UMI_SESSION_VALUE_CAPACITY];
        size_t remaining = encoded_length - offset;
        size_t count =
            remaining >
                    UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
                ? UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
                : remaining;

        if (chunk_count >=
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
            remove_partial_chunks(store, key_prefix, chunk_count);
            free(encoded);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = encode_chunk(
            encoded + offset,
            count,
            stored_chunk,
            sizeof(stored_chunk));
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count);
            free(encoded);
            return status;
        }

        status = make_chunk_key(
            key, sizeof(key), key_prefix, chunk_count);
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count);
            free(encoded);
            return status;
        }

        status = umi_session_store_set(store, key, stored_chunk);
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count + 1U);
            free(encoded);
            return status;
        }

        offset += count;
        ++chunk_count;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status == UMI_STATUS_OK) {
        const int written = snprintf(
            count_text, sizeof(count_text), "%zu", chunk_count);

        if (written < 0 || (size_t)written >= sizeof(count_text)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    if (status == UMI_STATUS_OK) {
        status = umi_session_store_set(store, key, count_text);
    }

    if (status != UMI_STATUS_OK) {
        remove_partial_chunks(store, key_prefix, chunk_count);
    }

    free(encoded);
    return status;
}

UmiStatus umi_workbench_context_host_session_restore(
    UmiWorkbenchContextHost *host,
    const UmiSessionStore *store,
    const char *key_prefix,
    bool *out_restored)
{
    UmiWorkbenchContextHostSession session;
    char key[UMI_SESSION_KEY_CAPACITY];
    char count_text[32U];
    char *encoded;
    size_t chunk_count = 0U;
    size_t used = 0U;
    size_t index;
    UmiStatus status;

    if (host == NULL || store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0' || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = false;

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    if (status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = parse_chunk_count(count_text, &chunk_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    encoded = (char *)calloc(
        UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY, 1U);
    if (encoded == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = UMI_STATUS_OK;

    for (index = 0U; index < chunk_count; ++index) {
        char stored_chunk[UMI_SESSION_VALUE_CAPACITY];
        char chunk[
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY + 1U];
        size_t chunk_length = 0U;

        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        if (status != UMI_STATUS_OK) {
            break;
        }

        status = umi_session_store_get(
            store, key, stored_chunk, sizeof(stored_chunk));
        if (status != UMI_STATUS_OK) {
            break;
        }

        status = decode_chunk(
            stored_chunk,
            chunk,
            sizeof(chunk),
            &chunk_length);
        if (status != UMI_STATUS_OK) {
            break;
        }

        if (used + chunk_length + 1U >
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }

        memcpy(encoded + used, chunk, chunk_length);
        used += chunk_length;
        encoded[used] = '\0';
    }

    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_decode(
            encoded, &session);
    }

    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_apply(
            host, &session);
    }

    if (status == UMI_STATUS_OK) {
        *out_restored = true;
    }

    free(encoded);
    return status;
}

UmiStatus umi_workbench_context_host_session_remove(
    UmiSessionStore *store,
    const char *key_prefix)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    char count_text[32U];
    size_t chunk_count = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    if (status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = parse_chunk_count(count_text, &chunk_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    for (index = 0U; index < chunk_count; ++index) {
        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        if (status != UMI_STATUS_OK) {
            return status;
        }

        status = umi_session_store_remove(store, key);
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_remove(store, key);
    return status == UMI_STATUS_NOT_FOUND
        ? UMI_STATUS_OK
        : status;
}
