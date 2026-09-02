/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/chunk_store.c
 *
 * PURPOSE:
 *   Implement transactional chunk storage, stale-chunk cleanup and integrity verification over the bounded Data Server value contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/chunk_store.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"




#define UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD 3600U

/*
 * Write workbench layout chunk manifest in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_chunk_manifest_encode(
    const UmiWorkbenchLayoutDataChunkManifest *manifest,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL ||
        manifest->structure_size < sizeof(*manifest)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(&fields);
    status = umi_workbench_layout_data_field_set_put(
        &fields, "aggregate_id", manifest->aggregate_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "manifest_kind", (uint32_t)manifest->manifest_kind);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "chunk_kind", (uint32_t)manifest->chunk_kind);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "chunk_count", (uint64_t)manifest->chunk_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "byte_count", (uint64_t)manifest->byte_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "content_hash", manifest->content_hash);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", manifest->revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "modified_at_ms", manifest->modified_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/*
 * Read workbench layout chunk manifest into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_chunk_manifest_decode(
    const char *value,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    const char *aggregate_id;
    uint32_t parsed = 0U;
    uint64_t count = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_manifest, 0, sizeof(*out_manifest));
    out_manifest->structure_size = sizeof(*out_manifest);
    status = umi_workbench_layout_data_value_decode(value, &fields);
    aggregate_id = umi_workbench_layout_data_field_set_get(
        &fields, "aggregate_id");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = aggregate_id != NULL
            ? umi_workbench_layout_data_copy_text(
                out_manifest->aggregate_id,
                sizeof(out_manifest->aggregate_id),
                aggregate_id, false)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "manifest_kind", &parsed);
        out_manifest->manifest_kind =
            (UmiWorkbenchLayoutDataRecordKind)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "chunk_kind", &parsed);
        out_manifest->chunk_kind =
            (UmiWorkbenchLayoutDataRecordKind)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "chunk_count", &count);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count > UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_manifest->chunk_count = (size_t)count;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "byte_count", &count);
        out_manifest->byte_count = (size_t)count;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "content_hash", &out_manifest->content_hash);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_manifest->revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "modified_at_ms", &out_manifest->modified_at_ms);
    }
    return status;
}

/*
 * Initialise workbench layout chunk store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_chunk_store_init(
    UmiWorkbenchLayoutChunkStore *store,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind manifest_kind,
    UmiWorkbenchLayoutDataRecordKind chunk_kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!((manifest_kind ==
               UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST &&
           chunk_kind ==
               UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK) ||
          (manifest_kind ==
               UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST &&
           chunk_kind ==
               UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->server = server;
    store->manifest_kind = manifest_kind;
    store->chunk_kind = chunk_kind;
    return UMI_STATUS_OK;
}

/* Provide the manifest key operation used by this module and its client applications. */
static UmiStatus manifest_key(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    char *key,
    size_t capacity)
{
    return umi_workbench_layout_data_key_build(
        store->manifest_kind, aggregate_id, NULL,
        0U, 0U, key, capacity);
}

/* Provide the chunk key operation used by this module and its client applications. */
static UmiStatus chunk_key(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    size_t index,
    char *key,
    size_t capacity)
{
    return umi_workbench_layout_data_key_build(
        store->chunk_kind, aggregate_id, NULL,
        0U, index, key, capacity);
}

/* Provide the load manifest operation used by this module and its client applications. */
static UmiStatus load_manifest(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *value = NULL;
    UmiStatus status = manifest_key(
        store, aggregate_id, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_store_get_allocated(
        store->server, key, &value, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_chunk_manifest_decode(
            value, out_manifest);
    }
    free(value);
    return status;
}

/*
 * Write workbench layout chunk store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_chunk_store_save(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    const char *text,
    uint64_t revision,
    uint64_t modified_at_ms,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    UmiWorkbenchLayoutDataChunkManifest previous;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t length;
    size_t count;
    size_t index;
    bool transaction_started = false;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || store->structure_size < sizeof(*store) ||
        aggregate_id == NULL || aggregate_id[0] == '\0' ||
        text == NULL || revision == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    count = length == 0U
        ? 1U
        : (length + UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD - 1U) /
          UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(&previous, 0, sizeof(previous));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (load_manifest(store, aggregate_id, &previous) != UMI_STATUS_OK) {
        previous.structure_size = sizeof(previous);
    }

    status = umi_workbench_layout_data_transaction_begin(
        store->server, &transaction_started);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const size_t offset = index * UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD;
        const size_t remaining = length > offset ? length - offset : 0U;
        const size_t amount = remaining > UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD
            ? UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD : remaining;
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Apply this branch only when its contract condition is satisfied. */
        if (amount > 0U) (void)memcpy(value, text + offset, amount);
        value[amount] = '\0';
        status = umi_workbench_layout_data_store_set(
            store->server, key, value);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = count;
         status == UMI_STATUS_OK && index < previous.chunk_count;
         ++index) {
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_data_store_delete_if_present(
                store->server, key);
        }
    }

    (void)memset(&manifest, 0, sizeof(manifest));
    manifest.structure_size = sizeof(manifest);
    manifest.manifest_kind = store->manifest_kind;
    manifest.chunk_kind = store->chunk_kind;
    manifest.chunk_count = count;
    manifest.byte_count = length;
    manifest.content_hash =
        umi_workbench_layout_data_hash_bytes(text, length);
    manifest.revision = revision;
    manifest.modified_at_ms = modified_at_ms;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            manifest.aggregate_id, sizeof(manifest.aggregate_id),
            aggregate_id, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        size_t required = 0U;
        status = umi_workbench_layout_chunk_manifest_encode(
            &manifest, value, sizeof(value), &required);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = manifest_key(store, aggregate_id, key, sizeof(key));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_set(
            store->server, key, value);
    }
    status = umi_workbench_layout_data_transaction_finish(
        store->server, transaction_started, status);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_manifest != NULL) {
        *out_manifest = manifest;
    }
    return status;
}

/*
 * Read workbench layout chunk store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_chunk_store_load(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    char **out_text,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *text;
    size_t offset = 0U;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || aggregate_id == NULL || out_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    status = load_manifest(store, aggregate_id, &manifest);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    text = (char *)calloc(manifest.byte_count + 1U, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest.chunk_count; ++index) {
        char *chunk = NULL;
        size_t chunk_size = 0U;
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_data_store_get_allocated(
                store->server, key, &chunk, &chunk_size);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (offset + chunk_size > manifest.byte_count) {
                status = UMI_STATUS_PARSE_ERROR;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                (void)memcpy(text + offset, chunk, chunk_size);
                offset += chunk_size;
            }
        }
        free(chunk);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && offset != manifest.byte_count) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_data_hash_bytes(
            text, manifest.byte_count) != manifest.content_hash) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(text);
        return status;
    }
    text[manifest.byte_count] = '\0';
    *out_text = text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_manifest != NULL) *out_manifest = manifest;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout chunk store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_chunk_store_delete(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    size_t index;
    bool transaction_started = false;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || aggregate_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = load_manifest(store, aggregate_id, &manifest);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_transaction_begin(
        store->server, &transaction_started);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < manifest.chunk_count;
         ++index) {
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_data_store_delete_if_present(
                store->server, key);
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = manifest_key(store, aggregate_id, key, sizeof(key));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_delete_if_present(
            store->server, key);
    }
    return umi_workbench_layout_data_transaction_finish(
        store->server, transaction_started, status);
}

/*
 * Provide the workbench layout chunk store verify operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_chunk_store_verify(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    char *text = NULL;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status = umi_workbench_layout_chunk_store_load(
        store, aggregate_id, &text, &manifest);
    free(text);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_manifest != NULL) {
        *out_manifest = manifest;
    }
    return status;
}
