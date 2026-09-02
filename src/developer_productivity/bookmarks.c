/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/bookmarks.c
 *
 * PURPOSE:
 *   Implement bounded named bookmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/bookmarks.h"

#include <string.h>

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiDeveloperBookmarkStore *store,
    const char *bookmark_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || bookmark_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->bookmarks[index].bookmark_id, bookmark_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise developer bookmark store from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_bookmark_store_init(
    UmiDeveloperBookmarkStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

/*
 * Provide the developer bookmark store upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_bookmark_store_upsert(
    UmiDeveloperBookmarkStore *store,
    const UmiDeveloperBookmark *bookmark)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || bookmark == NULL ||
        bookmark->bookmark_id[0] == '\0' ||
        bookmark->label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &bookmark->location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(store, bookmark->bookmark_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->count >= UMI_DEVELOPER_BOOKMARK_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = store->count++;
    }

    store->bookmarks[index] = *bookmark;
    store->bookmarks[index].revision = ++store->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove developer bookmark store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_developer_bookmark_store_remove(
    UmiDeveloperBookmarkStore *store,
    const char *bookmark_id)
{
    size_t index;
    size_t tail;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || bookmark_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(store, bookmark_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = store->count - index - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (tail > 0U) {
        (void)memmove(
            &store->bookmarks[index],
            &store->bookmarks[index + 1U],
            tail * sizeof(store->bookmarks[0]));
    }

    store->count -= 1U;
    (void)memset(
        &store->bookmarks[store->count],
        0,
        sizeof(store->bookmarks[0]));
    store->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find developer bookmark store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_bookmark_store_at(
    const UmiDeveloperBookmarkStore *store,
    size_t index,
    UmiDeveloperBookmark *out_bookmark)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= store->count) return UMI_STATUS_NOT_FOUND;

    *out_bookmark = store->bookmarks[index];
    return UMI_STATUS_OK;
}
