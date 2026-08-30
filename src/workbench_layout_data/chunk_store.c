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

UmiStatus umi_workbench_layout_chunk_manifest_encode(
    const UmiWorkbenchLayoutDataChunkManifest *manifest,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status = UMI_STATUS_OK;
    if (manifest == NULL ||
        manifest->structure_size < sizeof(*manifest)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(&fields);
    status = umi_workbench_layout_data_field_set_put(
        &fields, "aggregate_id", manifest->aggregate_id);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "manifest_kind", (uint32_t)manifest->manifest_kind);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "chunk_kind", (uint32_t)manifest->chunk_kind);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "chunk_count", (uint64_t)manifest->chunk_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "byte_count", (uint64_t)manifest->byte_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "content_hash", manifest->content_hash);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", manifest->revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "modified_at_ms", manifest->modified_at_ms);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_chunk_manifest_decode(
    const char *value,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    const char *aggregate_id;
    uint32_t parsed = 0U;
    uint64_t count = 0U;
    UmiStatus status;
    if (value == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_manifest, 0, sizeof(*out_manifest));
    out_manifest->structure_size = sizeof(*out_manifest);
    status = umi_workbench_layout_data_value_decode(value, &fields);
    aggregate_id = umi_workbench_layout_data_field_set_get(
        &fields, "aggregate_id");
    if (status == UMI_STATUS_OK) {
        status = aggregate_id != NULL
            ? umi_workbench_layout_data_copy_text(
                out_manifest->aggregate_id,
                sizeof(out_manifest->aggregate_id),
                aggregate_id, false)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "manifest_kind", &parsed);
        out_manifest->manifest_kind =
            (UmiWorkbenchLayoutDataRecordKind)parsed;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "chunk_kind", &parsed);
        out_manifest->chunk_kind =
            (UmiWorkbenchLayoutDataRecordKind)parsed;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "chunk_count", &count);
        if (count > UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        } else {
            out_manifest->chunk_count = (size_t)count;
        }
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "byte_count", &count);
        out_manifest->byte_count = (size_t)count;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "content_hash", &out_manifest->content_hash);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_manifest->revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "modified_at_ms", &out_manifest->modified_at_ms);
    }
    return status;
}

UmiStatus umi_workbench_layout_chunk_store_init(
    UmiWorkbenchLayoutChunkStore *store,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind manifest_kind,
    UmiWorkbenchLayoutDataRecordKind chunk_kind)
{
    if (store == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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

static UmiStatus load_manifest(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *value = NULL;
    UmiStatus status = manifest_key(
        store, aggregate_id, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_store_get_allocated(
        store->server, key, &value, NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_chunk_manifest_decode(
            value, out_manifest);
    }
    free(value);
    return status;
}

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
    if (count > UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(&previous, 0, sizeof(previous));
    if (load_manifest(store, aggregate_id, &previous) != UMI_STATUS_OK) {
        previous.structure_size = sizeof(previous);
    }

    status = umi_workbench_layout_data_transaction_begin(
        store->server, &transaction_started);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const size_t offset = index * UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD;
        const size_t remaining = length > offset ? length - offset : 0U;
        const size_t amount = remaining > UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD
            ? UMI_WORKBENCH_LAYOUT_CHUNK_PAYLOAD : remaining;
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        if (status != UMI_STATUS_OK) break;
        if (amount > 0U) (void)memcpy(value, text + offset, amount);
        value[amount] = '\0';
        status = umi_workbench_layout_data_store_set(
            store->server, key, value);
    }
    for (index = count;
         status == UMI_STATUS_OK && index < previous.chunk_count;
         ++index) {
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
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
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            manifest.aggregate_id, sizeof(manifest.aggregate_id),
            aggregate_id, false);
    }
    if (status == UMI_STATUS_OK) {
        size_t required = 0U;
        status = umi_workbench_layout_chunk_manifest_encode(
            &manifest, value, sizeof(value), &required);
    }
    if (status == UMI_STATUS_OK) {
        status = manifest_key(store, aggregate_id, key, sizeof(key));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_set(
            store->server, key, value);
    }
    status = umi_workbench_layout_data_transaction_finish(
        store->server, transaction_started, status);
    if (status == UMI_STATUS_OK && out_manifest != NULL) {
        *out_manifest = manifest;
    }
    return status;
}

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
    if (store == NULL || aggregate_id == NULL || out_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    status = load_manifest(store, aggregate_id, &manifest);
    if (status != UMI_STATUS_OK) return status;
    text = (char *)calloc(manifest.byte_count + 1U, sizeof(char));
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < manifest.chunk_count; ++index) {
        char *chunk = NULL;
        size_t chunk_size = 0U;
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_data_store_get_allocated(
                store->server, key, &chunk, &chunk_size);
        }
        if (status == UMI_STATUS_OK) {
            if (offset + chunk_size > manifest.byte_count) {
                status = UMI_STATUS_PARSE_ERROR;
            } else {
                (void)memcpy(text + offset, chunk, chunk_size);
                offset += chunk_size;
            }
        }
        free(chunk);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK && offset != manifest.byte_count) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_data_hash_bytes(
            text, manifest.byte_count) != manifest.content_hash) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    if (status != UMI_STATUS_OK) {
        free(text);
        return status;
    }
    text[manifest.byte_count] = '\0';
    *out_text = text;
    if (out_manifest != NULL) *out_manifest = manifest;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_chunk_store_delete(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    size_t index;
    bool transaction_started = false;
    UmiStatus status;
    if (store == NULL || aggregate_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = load_manifest(store, aggregate_id, &manifest);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_transaction_begin(
        store->server, &transaction_started);
    for (index = 0U;
         status == UMI_STATUS_OK && index < manifest.chunk_count;
         ++index) {
        status = chunk_key(store, aggregate_id, index, key, sizeof(key));
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_data_store_delete_if_present(
                store->server, key);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = manifest_key(store, aggregate_id, key, sizeof(key));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_delete_if_present(
            store->server, key);
    }
    return umi_workbench_layout_data_transaction_finish(
        store->server, transaction_started, status);
}

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
    if (status == UMI_STATUS_OK && out_manifest != NULL) {
        *out_manifest = manifest;
    }
    return status;
}
