/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/session_service.c
 *
 * PURPOSE:
 *   Encode, chunk, restore and remove context-host assignment state without
 *   exceeding Session Store key/value limits or violating its line-safe value
 *   contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/session_service.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_WCH_SESSION_CHUNK_PREFIX "H1:"
#define UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH 3U

/* Provide the make key operation used by this module and its client applications. */
static UmiStatus make_key(
    char *out_key,
    size_t capacity,
    const char *prefix,
    const char *suffix)
{
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_key == NULL || prefix == NULL || suffix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_key, capacity, "%s.%s", prefix, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the make chunk key operation used by this module and its client applications. */
static UmiStatus make_chunk_key(
    char *out_key,
    size_t capacity,
    const char *prefix,
    size_t index)
{
    char suffix[32U];
    int written;

    written = snprintf(suffix, sizeof(suffix), "chunk.%zu", index);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (input_length >
        (SIZE_MAX - UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH - 1U) / 2U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    required =
        UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH + (input_length * 2U) + 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    memcpy(
        out_text,
        UMI_WCH_SESSION_CHUNK_PREFIX,
        UMI_WCH_SESSION_CHUNK_PREFIX_LENGTH);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < input_length; ++index) {
        const unsigned char value = (unsigned char)input[index];

        out_text[output_index++] = digits[(value >> 4U) & 0x0FU];
        out_text[output_index++] = digits[value & 0x0FU];
    }

    out_text[output_index] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char character)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (character >= '0' && character <= '9') {
        return (int)(character - '0');
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (character >= 'A' && character <= 'F') {
        return 10 + (int)(character - 'A');
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stored_text == NULL || out_chunk == NULL ||
        out_capacity == 0U || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_length = 0U;
    stored_length = strlen(stored_text);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

        /* Apply this branch only when its contract condition is satisfied. */
        if ((hex_length % 2U) != 0U) {
            return UMI_STATUS_PARSE_ERROR;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (decoded_length + 1U > out_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < decoded_length; ++index) {
            const int high = hex_value(hex_text[index * 2U]);
            const int low = hex_value(hex_text[(index * 2U) + 1U]);

            /* Apply this branch only when its contract condition is satisfied. */
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

    /* Apply this branch only when its contract condition is satisfied. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_count == NULL || text[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        const unsigned char character = (unsigned char)text[index];
        size_t digit;

        /* Apply this branch only when its contract condition is satisfied. */
        if (character < (unsigned char)'0' ||
            character > (unsigned char)'9') {
            return UMI_STATUS_PARSE_ERROR;
        }

        digit = (size_t)(character - (unsigned char)'0');

        /* Apply this branch only when its contract condition is satisfied. */
        if (value >
            (UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS - digit) / 10U) {
            return UMI_STATUS_PARSE_ERROR;
        }

        value = (value * 10U) + digit;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (value > UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_count = value;
    return UMI_STATUS_OK;
}

/*
 * Provide the remove partial chunks operation used by this module and its client
 * applications.
 */
static void remove_partial_chunks(
    UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL) {
        return;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chunk_count; ++index) {
        char key[UMI_SESSION_KEY_CAPACITY];

        /* Apply this branch only when its contract condition is satisfied. */
        if (make_chunk_key(
                key, sizeof(key), key_prefix, index) == UMI_STATUS_OK) {
            const UmiStatus status = umi_session_store_remove(store, key);
            (void)status;
        }
    }
}

/*
 * Write workbench context host session in its stable representation and report capacity or
 * input failures to the caller.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    encoded = (char *)calloc(
        UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY, 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_workbench_context_host_session_capture(host, &session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_encode(
            &session,
            encoded,
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(encoded);
        return status;
    }

    encoded_length = strlen(encoded);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset < encoded_length) {
        char stored_chunk[UMI_SESSION_VALUE_CAPACITY];
        size_t remaining = encoded_length - offset;
        size_t count =
            remaining >
                    UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
                ? UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
                : remaining;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count);
            free(encoded);
            return status;
        }

        status = make_chunk_key(
            key, sizeof(key), key_prefix, chunk_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count);
            free(encoded);
            return status;
        }

        status = umi_session_store_set(store, key, stored_chunk);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            remove_partial_chunks(store, key_prefix, chunk_count + 1U);
            free(encoded);
            return status;
        }

        offset += count;
        ++chunk_count;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const int written = snprintf(
            count_text, sizeof(count_text), "%zu", chunk_count);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(count_text)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_session_store_set(store, key, count_text);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        remove_partial_chunks(store, key_prefix, chunk_count);
    }

    free(encoded);
    return status;
}

/*
 * Provide the workbench context host session restore operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0' || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = false;

    status = make_key(key, sizeof(key), key_prefix, "count");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = parse_chunk_count(count_text, &chunk_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    encoded = (char *)calloc(
        UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY, 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = UMI_STATUS_OK;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chunk_count; ++index) {
        char stored_chunk[UMI_SESSION_VALUE_CAPACITY];
        char chunk[
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY + 1U];
        size_t chunk_length = 0U;

        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        status = umi_session_store_get(
            store, key, stored_chunk, sizeof(stored_chunk));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        status = decode_chunk(
            stored_chunk,
            chunk,
            sizeof(chunk),
            &chunk_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (used + chunk_length + 1U >
            UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }

        memcpy(encoded + used, chunk, chunk_length);
        used += chunk_length;
        encoded[used] = '\0';
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_decode(
            encoded, &session);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_session_apply(
            host, &session);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_restored = true;
    }

    free(encoded);
    return status;
}

/*
 * Remove workbench context host session while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_context_host_session_remove(
    UmiSessionStore *store,
    const char *key_prefix)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    char count_text[32U];
    size_t chunk_count = 0U;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL ||
        key_prefix[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = parse_chunk_count(count_text, &chunk_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chunk_count; ++index) {
        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        status = umi_session_store_remove(store, key);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_session_store_remove(store, key);
    return status == UMI_STATUS_NOT_FOUND
        ? UMI_STATUS_OK
        : status;
}
