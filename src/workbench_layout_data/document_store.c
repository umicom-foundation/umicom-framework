/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/document_store.c
 *
 * PURPOSE:
 *   Implement optimistic, chunked and integrity-checked persistence for semantic workbench layout documents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/document_store.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "umicom/workbench_layout/json.h"




UmiStatus umi_workbench_layout_document_store_init(
    UmiWorkbenchLayoutDocumentStore *store,
    UmiDataServer *server)
{
    UmiStatus status;
    if (store == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->server = server;
    status = umi_workbench_layout_chunk_store_init(
        &store->chunks,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK);
    return status;
}

static UmiStatus current_revision(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status;
    if (out_revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_revision = 0U;
    status = umi_workbench_layout_chunk_store_verify(
        &store->chunks, layout_id, &manifest);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    *out_revision = manifest.revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_store_save(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t modified_at_ms,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutJsonOptions options;
    UmiWorkbenchLayoutJsonResult result;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiWorkbenchLayoutDocument copy;
    char *json = NULL;
    uint64_t stored_revision = 0U;
    size_t required = 0U;
    UmiStatus status;
    if (store == NULL || document == NULL ||
        document->identity.layout_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = current_revision(
        store, document->identity.layout_id, &stored_revision);
    if (status != UMI_STATUS_OK) return status;
    if (expected_revision != stored_revision) {
        return expected_revision == 0U && stored_revision > 0U
            ? UMI_STATUS_ALREADY_EXISTS
            : UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_document_copy(&copy, document);
    if (status != UMI_STATUS_OK) return status;
    copy.version.base_revision = stored_revision;
    copy.version.revision = stored_revision + 1U;
    copy.audit.modified_at_ms = modified_at_ms;
    umi_workbench_layout_document_refresh_hash(&copy);

    options = umi_workbench_layout_json_options_default();
    options.pretty = false;
    options.include_audit = true;
    options.include_runtime_geometry = true;
    options.include_content_hash = true;
    status = umi_workbench_layout_json_encode(
        &copy, &options, NULL, 0U, &result);
    if (status == UMI_STATUS_OK ||
        status == UMI_STATUS_CAPACITY_EXCEEDED) {
        required = result.bytes_required;
        status = UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK || required == 0U) return status;
    json = (char *)calloc(required, sizeof(char));
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_encode(
        &copy, &options, json, required, &result);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_chunk_store_save(
            &store->chunks,
            copy.identity.layout_id,
            json,
            copy.version.revision,
            modified_at_ms,
            &manifest);
    }
    free(json);
    if (status == UMI_STATUS_OK && out_revision != NULL) {
        *out_revision = manifest.revision;
    }
    return status;
}

UmiStatus umi_workbench_layout_document_store_load(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutJsonResult result;
    char *json = NULL;
    UmiStatus status;
    if (store == NULL || layout_id == NULL ||
        out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_chunk_store_load(
        &store->chunks, layout_id, &json, NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_json_decode(
            json, strlen(json), out_document, &result);
    }
    free(json);
    return status;
}

UmiStatus umi_workbench_layout_document_store_delete(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    uint64_t expected_revision)
{
    uint64_t stored_revision = 0U;
    UmiStatus status;
    if (store == NULL || layout_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = current_revision(store, layout_id, &stored_revision);
    if (status != UMI_STATUS_OK) return status;
    if (stored_revision == 0U) return UMI_STATUS_NOT_FOUND;
    if (stored_revision != expected_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_workbench_layout_chunk_store_delete(
        &store->chunks, layout_id);
}

typedef struct DocumentManifestRecord {
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiWorkbenchLayoutDataChunkManifest manifest;
} DocumentManifestRecord;

typedef struct DocumentManifestCollector {
    DocumentManifestRecord *records;
    size_t count;
    size_t capacity;
} DocumentManifestCollector;

static bool text_matches(const char *text, const char *query)
{
    return query == NULL || query[0] == '\0' ||
           (text != NULL && strstr(text, query) != NULL);
}

static bool document_matches(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutStoreQuery *query)
{
    if (query == NULL) return true;
    if (query->owner_user_id[0] != '\0' &&
        strcmp(document->identity.owner_user_id,
               query->owner_user_id) != 0) return false;
    if (query->owner_application_id[0] != '\0' &&
        strcmp(document->identity.owner_application_id,
               query->owner_application_id) != 0) return false;
    if (query->workspace_id[0] != '\0' &&
        strcmp(document->identity.workspace_id,
               query->workspace_id) != 0) return false;
    if (query->category[0] != '\0' &&
        strcmp(document->category, query->category) != 0) return false;
    if (!query->include_built_in &&
        (document->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN) != 0U) {
        return false;
    }
    if (!query->include_shared &&
        (document->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED) != 0U) {
        return false;
    }
    if (!query->include_read_only &&
        (document->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY) != 0U) {
        return false;
    }
    if (!text_matches(document->name, query->text) &&
        !text_matches(document->description, query->text)) return false;
    return true;
}

static UmiStatus collect_manifest(const char *key,
                                  const char *value,
                                  void *context)
{
    DocumentManifestCollector *collector = (DocumentManifestCollector *)context;
    UmiWorkbenchLayoutDataKeyParts parts;
    DocumentManifestRecord *record;
    DocumentManifestRecord *expanded;
    size_t new_capacity;
    UmiStatus status;
    if (collector == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_data_key_parse(key, &parts);
    if (status != UMI_STATUS_OK) return status;
    if (collector->count == collector->capacity) {
        new_capacity = collector->capacity == 0U ? 16U : collector->capacity * 2U;
        if (new_capacity < collector->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        expanded = (DocumentManifestRecord *)realloc(
            collector->records, new_capacity * sizeof(collector->records[0]));
        if (expanded == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        collector->records = expanded;
        collector->capacity = new_capacity;
    }
    record = &collector->records[collector->count];
    (void)memset(record, 0, sizeof(*record));
    status = umi_workbench_layout_data_copy_text(
        record->layout_id, sizeof(record->layout_id), parts.aggregate_id, false);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_chunk_manifest_decode(value, &record->manifest);
    if (status != UMI_STATUS_OK) return status;
    collector->count += 1U;
    return UMI_STATUS_OK;
}

static void fill_summary(UmiWorkbenchLayoutRecordSummary *summary,
                         const UmiWorkbenchLayoutDocument *document,
                         const UmiWorkbenchLayoutDataChunkManifest *manifest)
{
    (void)memset(summary, 0, sizeof(*summary));
    summary->structure_size = sizeof(*summary);
    (void)umi_workbench_layout_data_copy_text(
        summary->layout_id, sizeof(summary->layout_id),
        document->identity.layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        summary->name, sizeof(summary->name), document->name, true);
    (void)umi_workbench_layout_data_copy_text(
        summary->category, sizeof(summary->category), document->category, true);
    (void)umi_workbench_layout_data_copy_text(
        summary->owner_user_id, sizeof(summary->owner_user_id),
        document->identity.owner_user_id, true);
    (void)umi_workbench_layout_data_copy_text(
        summary->owner_application_id, sizeof(summary->owner_application_id),
        document->identity.owner_application_id, true);
    (void)umi_workbench_layout_data_copy_text(
        summary->workspace_id, sizeof(summary->workspace_id),
        document->identity.workspace_id, true);
    summary->revision = manifest->revision;
    summary->modified_at_ms = manifest->modified_at_ms;
    summary->content_hash = manifest->content_hash;
    summary->flags = document->flags;
}

UmiStatus umi_workbench_layout_document_store_list(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    char prefix[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    DocumentManifestCollector collector = {0};
    size_t offset;
    size_t limit;
    size_t index;
    UmiStatus status;
    if (store == NULL || out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_list, 0, sizeof(*out_list));
    out_list->structure_size = sizeof(*out_list);
    status = umi_workbench_layout_data_key_prefix(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        NULL, prefix, sizeof(prefix));
    if (status != UMI_STATUS_OK) return status;

    /* The Data Server visit callback executes while the backend protects its
     * record set. It must never re-enter the Data Server. Collect identifiers
     * first; document loading begins only after visit_prefix has returned. */
    status = umi_workbench_layout_data_store_visit_prefix(
        store->server, prefix, collect_manifest, &collector, NULL);
    if (status != UMI_STATUS_OK) {
        free(collector.records);
        return status;
    }

    offset = query != NULL ? query->offset : 0U;
    limit = query != NULL && query->limit > 0U &&
        query->limit <= UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS
            ? query->limit
            : UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;

    for (index = 0U; index < collector.count; ++index) {
        UmiWorkbenchLayoutDocument document;
        status = umi_workbench_layout_document_store_load(
            store, collector.records[index].layout_id, &document);
        if (status != UMI_STATUS_OK) break;
        if (!document_matches(&document, query)) continue;
        out_list->total_available += 1U;
        if (out_list->total_available <= offset) {
            out_list->truncated = true;
            continue;
        }
        if (out_list->count >= limit) {
            out_list->truncated = true;
            continue;
        }
        fill_summary(&out_list->records[out_list->count++],
                     &document, &collector.records[index].manifest);
    }
    free(collector.records);
    if (status == UMI_STATUS_OK && out_list->total_available > out_list->count) {
        out_list->truncated = true;
    }
    return status;
}

UmiStatus umi_workbench_layout_document_store_verify(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_chunk_store_verify(
        &store->chunks, layout_id, out_manifest);
}
