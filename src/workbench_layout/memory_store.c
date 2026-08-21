/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/memory_store.c
 *
 * PURPOSE:
 *   Implement a deterministic transactional in-memory persistence adapter for
 *   tests, previews, headless tools and products without a configured Data
 *   Server.
 *
 * DESIGN:
 *   Layout documents are allocated only for occupied records. This keeps the
 *   public store object small while preserving fixed upper bounds, complete
 *   value-copy semantics and rollback snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/memory_store.h"

#include <stdlib.h>
#include <string.h>

#include "internal.h"

static void release_layout_records(
    UmiWorkbenchMemoryLayoutRecord *records,
    size_t capacity)
{
    size_t index;

    if (records == NULL) {
        return;
    }
    for (index = 0U; index < capacity; ++index) {
        free(records[index].document);
        records[index].document = NULL;
        records[index].occupied = false;
    }
}

static void release_session_records(
    UmiWorkbenchMemorySessionRecord *records,
    size_t capacity)
{
    size_t index;

    if (records == NULL) {
        return;
    }
    for (index = 0U; index < capacity; ++index) {
        free(records[index].session);
        records[index].session = NULL;
        records[index].occupied = false;
    }
}

static UmiStatus clone_layout_records(
    const UmiWorkbenchMemoryLayoutRecord *source,
    UmiWorkbenchMemoryLayoutRecord *destination,
    size_t capacity)
{
    size_t index;

    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    release_layout_records(destination, capacity);

    for (index = 0U; index < capacity; ++index) {
        if (!source[index].occupied ||
            source[index].document == NULL) {
            continue;
        }
        destination[index].document =
            (UmiWorkbenchLayoutDocument *)malloc(
                sizeof(*destination[index].document));
        if (destination[index].document == NULL) {
            release_layout_records(destination, capacity);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        *destination[index].document = *source[index].document;
        destination[index].occupied = true;
    }
    return UMI_STATUS_OK;
}

static UmiStatus clone_session_records(
    const UmiWorkbenchMemorySessionRecord *source,
    UmiWorkbenchMemorySessionRecord *destination,
    size_t capacity)
{
    size_t index;

    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    release_session_records(destination, capacity);

    for (index = 0U; index < capacity; ++index) {
        if (!source[index].occupied ||
            source[index].session == NULL) {
            continue;
        }
        destination[index].session =
            (UmiWorkbenchLayoutSession *)malloc(
                sizeof(*destination[index].session));
        if (destination[index].session == NULL) {
            release_session_records(destination, capacity);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        *destination[index].session = *source[index].session;
        destination[index].occupied = true;
    }
    return UMI_STATUS_OK;
}

static void move_layout_records(
    UmiWorkbenchMemoryLayoutRecord *source,
    UmiWorkbenchMemoryLayoutRecord *destination,
    size_t capacity)
{
    size_t index;

    release_layout_records(destination, capacity);
    for (index = 0U; index < capacity; ++index) {
        destination[index] = source[index];
        source[index].document = NULL;
        source[index].occupied = false;
    }
}

static void move_session_records(
    UmiWorkbenchMemorySessionRecord *source,
    UmiWorkbenchMemorySessionRecord *destination,
    size_t capacity)
{
    size_t index;

    release_session_records(destination, capacity);
    for (index = 0U; index < capacity; ++index) {
        destination[index] = source[index];
        source[index].session = NULL;
        source[index].occupied = false;
    }
}

static size_t find_layout_slot(
    const UmiWorkbenchMemoryStore *store,
    const char *layout_id)
{
    size_t index;

    if (store == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        if (store->layouts[index].occupied &&
            store->layouts[index].document != NULL &&
            strcmp(
                store->layouts[index].document->identity.layout_id,
                layout_id) == 0) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

static size_t find_free_layout_slot(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;

    for (index = 0U;
         store != NULL &&
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        if (!store->layouts[index].occupied) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

static size_t find_session_slot(
    const UmiWorkbenchMemoryStore *store,
    const char *session_id)
{
    size_t index;

    if (store == NULL ||
        !umi_workbench_layout_text_present(session_id)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    for (index = 0U;
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        if (store->sessions[index].occupied &&
            store->sessions[index].session != NULL &&
            strcmp(
                store->sessions[index].session->session_id,
                session_id) == 0) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

static size_t find_free_session_slot(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;

    for (index = 0U;
         store != NULL &&
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        if (!store->sessions[index].occupied) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

static bool query_text_matches(
    const UmiWorkbenchLayoutStoreQuery *query,
    const UmiWorkbenchLayoutDocument *document)
{
    size_t index;

    if (query->text[0] == '\0') {
        return true;
    }
    if (umi_workbench_layout_text_contains_case_insensitive(
            document->identity.layout_id, query->text) ||
        umi_workbench_layout_text_contains_case_insensitive(
            document->name, query->text) ||
        umi_workbench_layout_text_contains_case_insensitive(
            document->description, query->text)) {
        return true;
    }
    for (index = 0U; index < document->tag_count; ++index) {
        if (umi_workbench_layout_text_contains_case_insensitive(
                document->tags[index].value, query->text)) {
            return true;
        }
    }
    return false;
}

static bool query_matches(
    const UmiWorkbenchLayoutStoreQuery *query,
    const UmiWorkbenchLayoutDocument *document)
{
    if (query->owner_user_id[0] != '\0' &&
        strcmp(
            query->owner_user_id,
            document->identity.owner_user_id) != 0) {
        return false;
    }
    if (query->owner_application_id[0] != '\0' &&
        strcmp(
            query->owner_application_id,
            document->identity.owner_application_id) != 0) {
        return false;
    }
    if (query->workspace_id[0] != '\0' &&
        strcmp(
            query->workspace_id,
            document->identity.workspace_id) != 0) {
        return false;
    }
    if (query->category[0] != '\0' &&
        strcmp(query->category, document->category) != 0) {
        return false;
    }
    if (!query->include_built_in &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN)) {
        return false;
    }
    if (!query->include_shared &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED)) {
        return false;
    }
    if (!query->include_read_only &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY)) {
        return false;
    }
    return query_text_matches(query, document);
}

static void fill_summary(
    UmiWorkbenchLayoutRecordSummary *summary,
    const UmiWorkbenchLayoutDocument *document)
{
    (void)memset(summary, 0, sizeof(*summary));
    summary->structure_size = sizeof(*summary);
    (void)umi_workbench_layout_copy_text(
        summary->layout_id,
        sizeof(summary->layout_id),
        document->identity.layout_id,
        false);
    (void)umi_workbench_layout_copy_text(
        summary->name,
        sizeof(summary->name),
        document->name,
        false);
    (void)umi_workbench_layout_copy_text(
        summary->category,
        sizeof(summary->category),
        document->category,
        true);
    (void)umi_workbench_layout_copy_text(
        summary->owner_user_id,
        sizeof(summary->owner_user_id),
        document->identity.owner_user_id,
        true);
    (void)umi_workbench_layout_copy_text(
        summary->owner_application_id,
        sizeof(summary->owner_application_id),
        document->identity.owner_application_id,
        true);
    (void)umi_workbench_layout_copy_text(
        summary->workspace_id,
        sizeof(summary->workspace_id),
        document->identity.workspace_id,
        true);
    summary->revision = document->version.revision;
    summary->modified_at_ms = document->audit.modified_at_ms;
    summary->content_hash = document->content_hash;
    summary->flags = document->flags;
}

static UmiStatus ensure_layout_record(
    UmiWorkbenchMemoryLayoutRecord *record)
{
    if (record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->document == NULL) {
        record->document =
            (UmiWorkbenchLayoutDocument *)malloc(
                sizeof(*record->document));
        if (record->document == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus ensure_session_record(
    UmiWorkbenchMemorySessionRecord *record)
{
    if (record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->session == NULL) {
        record->session =
            (UmiWorkbenchLayoutSession *)malloc(
                sizeof(*record->session));
        if (record->session == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus memory_save_layout(
    void *context,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;
    uint64_t revision;
    UmiStatus status;

    if (store == NULL || document == NULL ||
        out_revision == NULL ||
        !umi_workbench_layout_text_present(
            document->identity.layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_layout_slot(
        store, document->identity.layout_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        if (expected_revision != 0U) {
            return UMI_STATUS_NOT_FOUND;
        }
        index = find_free_layout_slot(store);
        if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        revision = document->version.revision > 0U
            ? document->version.revision
            : 1U;
    } else {
        uint64_t current =
            store->layouts[index].document->version.revision;
        if (expected_revision != 0U &&
            expected_revision != current) {
            return UMI_STATUS_BUSY;
        }
        revision = current + 1U;
        if (document->version.revision > revision) {
            revision = document->version.revision;
        }
    }

    status = ensure_layout_record(&store->layouts[index]);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    *store->layouts[index].document = *document;
    store->layouts[index].document->structure_size =
        sizeof(*store->layouts[index].document);
    store->layouts[index].document->version.revision = revision;
    store->layouts[index].occupied = true;
    store->revision += 1U;
    *out_revision = revision;
    return UMI_STATUS_OK;
}

static UmiStatus memory_load_layout(
    void *context,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    if (store == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_layout_slot(store, layout_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_document = *store->layouts[index].document;
    return UMI_STATUS_OK;
}

static UmiStatus memory_delete_layout(
    void *context,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_layout_slot(store, layout_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (expected_revision != 0U &&
        expected_revision !=
            store->layouts[index].document->version.revision) {
        return UMI_STATUS_BUSY;
    }

    free(store->layouts[index].document);
    (void)memset(
        &store->layouts[index],
        0,
        sizeof(store->layouts[index]));
    store->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus memory_list_layouts(
    void *context,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;
    size_t matched = 0U;
    size_t included = 0U;
    size_t limit;

    if (store == NULL || query == NULL || out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_list, 0, sizeof(*out_list));
    out_list->structure_size = sizeof(*out_list);
    limit = query->limit > 0U
        ? query->limit
        : UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    if (limit > UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS) {
        limit = UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    }

    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        const UmiWorkbenchMemoryLayoutRecord *record =
            &store->layouts[index];

        if (!record->occupied ||
            record->document == NULL ||
            !query_matches(query, record->document)) {
            continue;
        }
        if (matched >= query->offset && included < limit) {
            fill_summary(
                &out_list->records[included],
                record->document);
            included += 1U;
        }
        matched += 1U;
    }

    out_list->count = included;
    out_list->total_available = matched;
    out_list->truncated =
        query->offset + included < matched;
    return UMI_STATUS_OK;
}

static UmiStatus memory_save_session(
    void *context,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;
    uint64_t revision;
    UmiStatus status;

    if (store == NULL || session == NULL ||
        out_revision == NULL ||
        !umi_workbench_layout_text_present(session->session_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_session_slot(store, session->session_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        if (expected_revision != 0U) {
            return UMI_STATUS_NOT_FOUND;
        }
        index = find_free_session_slot(store);
        if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        revision = session->revision > 0U
            ? session->revision
            : 1U;
    } else {
        uint64_t current =
            store->sessions[index].session->revision;
        if (expected_revision != 0U &&
            expected_revision != current) {
            return UMI_STATUS_BUSY;
        }
        revision = current + 1U;
        if (session->revision > revision) {
            revision = session->revision;
        }
    }

    status = ensure_session_record(&store->sessions[index]);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    *store->sessions[index].session = *session;
    store->sessions[index].session->structure_size =
        sizeof(*store->sessions[index].session);
    store->sessions[index].session->revision = revision;
    store->sessions[index].occupied = true;
    store->revision += 1U;
    *out_revision = revision;
    return UMI_STATUS_OK;
}

static UmiStatus memory_load_session(
    void *context,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    if (store == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_session_slot(store, session_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_session = *store->sessions[index].session;
    return UMI_STATUS_OK;
}

static UmiStatus memory_delete_session(
    void *context,
    const char *session_id,
    uint64_t expected_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_session_slot(store, session_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (expected_revision != 0U &&
        expected_revision !=
            store->sessions[index].session->revision) {
        return UMI_STATUS_BUSY;
    }

    free(store->sessions[index].session);
    (void)memset(
        &store->sessions[index],
        0,
        sizeof(store->sessions[index]));
    store->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus memory_begin_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    UmiStatus status;

    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->transaction_open) {
        return UMI_STATUS_BUSY;
    }

    status = clone_layout_records(
        store->layouts,
        store->transaction_layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    if (status == UMI_STATUS_OK) {
        status = clone_session_records(
            store->sessions,
            store->transaction_sessions,
            UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    }
    if (status != UMI_STATUS_OK) {
        release_layout_records(
            store->transaction_layouts,
            UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
        release_session_records(
            store->transaction_sessions,
            UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
        return status;
    }

    store->transaction_open = true;
    return UMI_STATUS_OK;
}

static UmiStatus memory_commit_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;

    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!store->transaction_open) {
        return UMI_STATUS_INVALID_STATE;
    }

    release_layout_records(
        store->transaction_layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    release_session_records(
        store->transaction_sessions,
        UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    store->transaction_open = false;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus memory_rollback_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;

    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!store->transaction_open) {
        return UMI_STATUS_INVALID_STATE;
    }

    move_layout_records(
        store->transaction_layouts,
        store->layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    move_session_records(
        store->transaction_sessions,
        store->sessions,
        UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    store->transaction_open = false;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_workbench_memory_store_init(
    UmiWorkbenchMemoryStore *store)
{
    if (store == NULL) {
        return;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->revision = 1U;
}

UmiWorkbenchLayoutStoreAdapter umi_workbench_memory_store_adapter(
    UmiWorkbenchMemoryStore *store)
{
    UmiWorkbenchLayoutStoreAdapter adapter;

    (void)memset(&adapter, 0, sizeof(adapter));
    adapter.structure_size = sizeof(adapter);
    adapter.kind = UMI_WORKBENCH_LAYOUT_STORE_MEMORY;
    adapter.context = store;
    adapter.save_layout = memory_save_layout;
    adapter.load_layout = memory_load_layout;
    adapter.delete_layout = memory_delete_layout;
    adapter.list_layouts = memory_list_layouts;
    adapter.save_session = memory_save_session;
    adapter.load_session = memory_load_session;
    adapter.delete_session = memory_delete_session;
    adapter.begin_transaction = memory_begin_transaction;
    adapter.commit_transaction = memory_commit_transaction;
    adapter.rollback_transaction = memory_rollback_transaction;
    return adapter;
}

size_t umi_workbench_memory_store_layout_count(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;
    size_t count = 0U;

    if (store == NULL) {
        return 0U;
    }
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        if (store->layouts[index].occupied &&
            store->layouts[index].document != NULL) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_workbench_memory_store_session_count(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;
    size_t count = 0U;

    if (store == NULL) {
        return 0U;
    }
    for (index = 0U;
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        if (store->sessions[index].occupied &&
            store->sessions[index].session != NULL) {
            count += 1U;
        }
    }
    return count;
}

void umi_workbench_memory_store_clear(
    UmiWorkbenchMemoryStore *store)
{
    if (store == NULL) {
        return;
    }

    release_layout_records(
        store->layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    release_session_records(
        store->sessions,
        UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    release_layout_records(
        store->transaction_layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    release_session_records(
        store->transaction_sessions,
        UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->revision = 1U;
}
