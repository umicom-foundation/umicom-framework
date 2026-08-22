/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/session_service.c
 *
 * PURPOSE:
 *   Encode, chunk, restore and remove context-host assignment state without exceeding Session Store value limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/session_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int written = snprintf(suffix, sizeof(suffix), "chunk.%zu", index);
    if (written < 0 || (size_t)written >= sizeof(suffix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return make_key(out_key, capacity, prefix, suffix);
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
    if (encoded == NULL) return UMI_STATUS_OUT_OF_MEMORY;

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

    encoded_length = strlen(encoded);
    while (offset < encoded_length) {
        char chunk[UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY + 1U];
        size_t remaining = encoded_length - offset;
        size_t count = remaining > UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
            ? UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY
            : remaining;

        if (chunk_count >= UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
            free(encoded);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        memcpy(chunk, encoded + offset, count);
        chunk[count] = '\0';
        status = make_chunk_key(
            key, sizeof(key), key_prefix, chunk_count);
        if (status != UMI_STATUS_OK) {
            free(encoded);
            return status;
        }
        status = umi_session_store_set(store, key, chunk);
        if (status != UMI_STATUS_OK) {
            free(encoded);
            return status;
        }

        offset += count;
        ++chunk_count;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status == UMI_STATUS_OK) {
        int written = snprintf(
            count_text, sizeof(count_text), "%zu", chunk_count);
        if (written < 0 || (size_t)written >= sizeof(count_text)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    if (status == UMI_STATUS_OK) {
        status = umi_session_store_set(store, key, count_text);
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
    char *tail = NULL;
    unsigned long parsed;
    size_t chunk_count;
    size_t used = 0U;
    size_t index;
    UmiStatus status;

    if (host == NULL || store == NULL || key_prefix == NULL ||
        out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_restored = false;

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) return status;
    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    parsed = strtoul(count_text, &tail, 10);
    if (tail == count_text || *tail != '\0' ||
        parsed > UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }
    chunk_count = (size_t)parsed;

    encoded = (char *)calloc(
        UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY, 1U);
    if (encoded == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    for (index = 0U; index < chunk_count; ++index) {
        char chunk[UMI_SESSION_VALUE_CAPACITY];
        size_t chunk_length;

        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        if (status != UMI_STATUS_OK) break;
        status = umi_session_store_get(
            store, key, chunk, sizeof(chunk));
        if (status != UMI_STATUS_OK) break;

        chunk_length = strlen(chunk);
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
    if (status == UMI_STATUS_OK) *out_restored = true;

    free(encoded);
    return status;
}

UmiStatus umi_workbench_context_host_session_remove(
    UmiSessionStore *store,
    const char *key_prefix)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    char count_text[32U];
    char *tail = NULL;
    unsigned long parsed = 0UL;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) return status;
    status = umi_session_store_get(
        store, key, count_text, sizeof(count_text));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    parsed = strtoul(count_text, &tail, 10);
    if (tail == count_text || *tail != '\0' ||
        parsed > UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    for (index = 0U; index < (size_t)parsed; ++index) {
        status = make_chunk_key(
            key, sizeof(key), key_prefix, index);
        if (status != UMI_STATUS_OK) return status;
        status = umi_session_store_remove(store, key);
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }

    status = make_key(key, sizeof(key), key_prefix, "count");
    if (status != UMI_STATUS_OK) return status;
    status = umi_session_store_remove(store, key);
    return status == UMI_STATUS_NOT_FOUND
        ? UMI_STATUS_OK
        : status;
}
