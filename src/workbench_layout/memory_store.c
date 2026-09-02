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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/memory_store.h"

#include <stdlib.h>
#include <string.h>

#include "internal.h"

/*
 * Provide the release layout records operation used by this module and its client
 * applications.
 */
static void release_layout_records(
    UmiWorkbenchMemoryLayoutRecord *records,
    size_t capacity)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (records == NULL) {
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        free(records[index].document);
        records[index].document = NULL;
        records[index].occupied = false;
    }
}

/*
 * Provide the release session records operation used by this module and its client
 * applications.
 */
static void release_session_records(
    UmiWorkbenchMemorySessionRecord *records,
    size_t capacity)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (records == NULL) {
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        free(records[index].session);
        records[index].session = NULL;
        records[index].occupied = false;
    }
}

/*
 * Provide the clone layout records operation used by this module and its client
 * applications.
 */
static UmiStatus clone_layout_records(
    const UmiWorkbenchMemoryLayoutRecord *source,
    UmiWorkbenchMemoryLayoutRecord *destination,
    size_t capacity)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    release_layout_records(destination, capacity);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!source[index].occupied ||
            source[index].document == NULL) {
            continue;
        }
        destination[index].document =
            (UmiWorkbenchLayoutDocument *)malloc(
                sizeof(*destination[index].document));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (destination[index].document == NULL) {
            release_layout_records(destination, capacity);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        *destination[index].document = *source[index].document;
        destination[index].occupied = true;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the clone session records operation used by this module and its client
 * applications.
 */
static UmiStatus clone_session_records(
    const UmiWorkbenchMemorySessionRecord *source,
    UmiWorkbenchMemorySessionRecord *destination,
    size_t capacity)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    release_session_records(destination, capacity);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!source[index].occupied ||
            source[index].session == NULL) {
            continue;
        }
        destination[index].session =
            (UmiWorkbenchLayoutSession *)malloc(
                sizeof(*destination[index].session));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (destination[index].session == NULL) {
            release_session_records(destination, capacity);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        *destination[index].session = *source[index].session;
        destination[index].occupied = true;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the move layout records operation used by this module and its client
 * applications.
 */
static void move_layout_records(
    UmiWorkbenchMemoryLayoutRecord *source,
    UmiWorkbenchMemoryLayoutRecord *destination,
    size_t capacity)
{
    size_t index;

    release_layout_records(destination, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        destination[index] = source[index];
        source[index].document = NULL;
        source[index].occupied = false;
    }
}

/*
 * Provide the move session records operation used by this module and its client
 * applications.
 */
static void move_session_records(
    UmiWorkbenchMemorySessionRecord *source,
    UmiWorkbenchMemorySessionRecord *destination,
    size_t capacity)
{
    size_t index;

    release_session_records(destination, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < capacity; ++index) {
        destination[index] = source[index];
        source[index].session = NULL;
        source[index].occupied = false;
    }
}

/* Provide the find layout slot operation used by this module and its client applications. */
static size_t find_layout_slot(
    const UmiWorkbenchMemoryStore *store,
    const char *layout_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the find free layout slot operation used by this module and its client
 * applications.
 */
static size_t find_free_layout_slot(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         store != NULL &&
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!store->layouts[index].occupied) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

/* Provide the find session slot operation used by this module and its client applications. */
static size_t find_session_slot(
    const UmiWorkbenchMemoryStore *store,
    const char *session_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL ||
        !umi_workbench_layout_text_present(session_id)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the find free session slot operation used by this module and its client
 * applications.
 */
static size_t find_free_session_slot(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         store != NULL &&
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!store->sessions[index].occupied) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

/*
 * Provide the query text matches operation used by this module and its client
 * applications.
 */
static bool query_text_matches(
    const UmiWorkbenchLayoutStoreQuery *query,
    const UmiWorkbenchLayoutDocument *document)
{
    size_t index;

    /* Apply this branch only when its contract condition is satisfied. */
    if (query->text[0] == '\0') {
        return true;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_text_contains_case_insensitive(
            document->identity.layout_id, query->text) ||
        umi_workbench_layout_text_contains_case_insensitive(
            document->name, query->text) ||
        umi_workbench_layout_text_contains_case_insensitive(
            document->description, query->text)) {
        return true;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->tag_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_layout_text_contains_case_insensitive(
                document->tags[index].value, query->text)) {
            return true;
        }
    }
    return false;
}

/* Provide the query matches operation used by this module and its client applications. */
static bool query_matches(
    const UmiWorkbenchLayoutStoreQuery *query,
    const UmiWorkbenchLayoutDocument *document)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (query->owner_user_id[0] != '\0' &&
        strcmp(
            query->owner_user_id,
            document->identity.owner_user_id) != 0) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (query->owner_application_id[0] != '\0' &&
        strcmp(
            query->owner_application_id,
            document->identity.owner_application_id) != 0) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (query->workspace_id[0] != '\0' &&
        strcmp(
            query->workspace_id,
            document->identity.workspace_id) != 0) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->category[0] != '\0' &&
        strcmp(query->category, document->category) != 0) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_built_in &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN)) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_shared &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED)) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_read_only &&
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY)) {
        return false;
    }
    return query_text_matches(query, document);
}

/* Provide the fill summary operation used by this module and its client applications. */
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

/*
 * Provide the ensure layout record operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_layout_record(
    UmiWorkbenchMemoryLayoutRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record->document == NULL) {
        record->document =
            (UmiWorkbenchLayoutDocument *)malloc(
                sizeof(*record->document));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (record->document == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ensure session record operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_session_record(
    UmiWorkbenchMemorySessionRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record->session == NULL) {
        record->session =
            (UmiWorkbenchLayoutSession *)malloc(
                sizeof(*record->session));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (record->session == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the memory save layout operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || document == NULL ||
        out_revision == NULL ||
        !umi_workbench_layout_text_present(
            document->identity.layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_layout_slot(
        store, document->identity.layout_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (expected_revision != 0U) {
            return UMI_STATUS_NOT_FOUND;
        }
        index = find_free_layout_slot(store);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        revision = document->version.revision > 0U
            ? document->version.revision
            : 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        uint64_t current =
            store->layouts[index].document->version.revision;
        /* Apply this branch only when its contract condition is satisfied. */
        if (expected_revision != 0U &&
            expected_revision != current) {
            return UMI_STATUS_BUSY;
        }
        revision = current + 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (document->version.revision > revision) {
            revision = document->version.revision;
        }
    }

    status = ensure_layout_record(&store->layouts[index]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the memory load layout operation used by this module and its client
 * applications.
 */
static UmiStatus memory_load_layout(
    void *context,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_layout_slot(store, layout_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_document = *store->layouts[index].document;
    return UMI_STATUS_OK;
}

/*
 * Provide the memory delete layout operation used by this module and its client
 * applications.
 */
static UmiStatus memory_delete_layout(
    void *context,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_layout_slot(store, layout_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the memory list layouts operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || query == NULL || out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_list, 0, sizeof(*out_list));
    out_list->structure_size = sizeof(*out_list);
    limit = query->limit > 0U
        ? query->limit
        : UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS) {
        limit = UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        const UmiWorkbenchMemoryLayoutRecord *record =
            &store->layouts[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (!record->occupied ||
            record->document == NULL ||
            !query_matches(query, record->document)) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the memory save session operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || session == NULL ||
        out_revision == NULL ||
        !umi_workbench_layout_text_present(session->session_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_session_slot(store, session->session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (expected_revision != 0U) {
            return UMI_STATUS_NOT_FOUND;
        }
        index = find_free_session_slot(store);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        revision = session->revision > 0U
            ? session->revision
            : 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        uint64_t current =
            store->sessions[index].session->revision;
        /* Apply this branch only when its contract condition is satisfied. */
        if (expected_revision != 0U &&
            expected_revision != current) {
            return UMI_STATUS_BUSY;
        }
        revision = current + 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (session->revision > revision) {
            revision = session->revision;
        }
    }

    status = ensure_session_record(&store->sessions[index]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the memory load session operation used by this module and its client
 * applications.
 */
static UmiStatus memory_load_session(
    void *context,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_session_slot(store, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_session = *store->sessions[index].session;
    return UMI_STATUS_OK;
}

/*
 * Provide the memory delete session operation used by this module and its client
 * applications.
 */
static UmiStatus memory_delete_session(
    void *context,
    const char *session_id,
    uint64_t expected_revision)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_session_slot(store, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the memory begin transaction operation used by this module and its client
 * applications.
 */
static UmiStatus memory_begin_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (store->transaction_open) {
        return UMI_STATUS_BUSY;
    }

    status = clone_layout_records(
        store->layouts,
        store->transaction_layouts,
        UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = clone_session_records(
            store->sessions,
            store->transaction_sessions,
            UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the memory commit transaction operation used by this module and its client
 * applications.
 */
static UmiStatus memory_commit_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the memory rollback transaction operation used by this module and its client
 * applications.
 */
static UmiStatus memory_rollback_transaction(void *context)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Initialise workbench memory store from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_memory_store_init(
    UmiWorkbenchMemoryStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->revision = 1U;
}

/*
 * Provide the workbench memory store adapter operation used by this module and its client
 * applications.
 */
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

/*
 * Return the number of records represented by workbench memory store layout without
 * changing their state.
 */
size_t umi_workbench_memory_store_layout_count(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->layouts[index].occupied &&
            store->layouts[index].document != NULL) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Return the number of records represented by workbench memory store session without
 * changing their state.
 */
size_t umi_workbench_memory_store_session_count(
    const UmiWorkbenchMemoryStore *store)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->sessions[index].occupied &&
            store->sessions[index].session != NULL) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Release or reset state held by workbench memory store so the same storage can be reused
 * safely.
 */
void umi_workbench_memory_store_clear(
    UmiWorkbenchMemoryStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
