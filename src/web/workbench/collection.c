/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/collection.c
 *
 * PURPOSE:
 *   Implement revision-safe API request collections and deterministic queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/collection.h"

#include <ctype.h>
#include <string.h>

/* Provide the request index operation used by this module and its client applications. */
static size_t request_index(
    const UmiWebWorkbenchCollection *collection,
    const char *request_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < collection->request_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(collection->requests[index].request_id, request_id) == 0) return index;
    }
    return collection->request_count;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}

/*
 * Initialise web workbench collection from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_collection_init(
    UmiWebWorkbenchCollection *collection,
    const char *collection_id,
    const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL) return;
    memset(collection, 0, sizeof(*collection));
    (void)umi_web_workbench_copy_text(collection->collection_id,
        sizeof(collection->collection_id), collection_id != NULL ? collection_id : "collection");
    (void)umi_web_workbench_copy_text(collection->name,
        sizeof(collection->name), name != NULL ? name : "API Collection");
    collection->revision = 1U;
}

/*
 * Provide the web workbench collection upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_collection_upsert(
    UmiWebWorkbenchCollection *collection,
    const UmiWebWorkbenchRequest *request,
    uint64_t expected_revision)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected_revision != 0U && expected_revision != collection->revision) {
        return UMI_STATUS_BUSY;
    }
    status = umi_web_workbench_request_validate(request, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    index = request_index(collection, request->request_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == collection->request_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_WORKBENCH_MAX_REQUESTS) return UMI_STATUS_CAPACITY_EXCEEDED;
        collection->request_count++;
    }
    collection->requests[index] = *request;
    collection->revision++;
    return UMI_STATUS_OK;
}

/*
 * Remove web workbench collection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_collection_remove(
    UmiWebWorkbenchCollection *collection,
    const char *request_id,
    uint64_t expected_revision)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || request_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected_revision != 0U && expected_revision != collection->revision) {
        return UMI_STATUS_BUSY;
    }
    index = request_index(collection, request_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == collection->request_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < collection->request_count) {
        memmove(&collection->requests[index], &collection->requests[index + 1U],
            (collection->request_count - index - 1U) * sizeof(collection->requests[0]));
    }
    collection->request_count--;
    memset(&collection->requests[collection->request_count], 0,
        sizeof(collection->requests[0]));
    collection->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find web workbench collection while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWebWorkbenchRequest *umi_web_workbench_collection_find(
    const UmiWebWorkbenchCollection *collection,
    const char *request_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || request_id == NULL) return NULL;
    index = request_index(collection, request_id);
    return index < collection->request_count ? &collection->requests[index] : NULL;
}

/*
 * Provide the web workbench collection query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_collection_query(
    const UmiWebWorkbenchCollection *collection,
    const char *text,
    const char *folder,
    const UmiWebWorkbenchRequest **out_requests,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || (out_requests == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < collection->request_count && count < capacity; ++index) {
        const UmiWebWorkbenchRequest *request = &collection->requests[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (folder != NULL && folder[0] != '\0' && strcmp(request->folder, folder) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_case_insensitive(request->name, text) &&
            !contains_case_insensitive(request->url, text)) continue;
        out_requests[count++] = request;
    }
    return count;
}
