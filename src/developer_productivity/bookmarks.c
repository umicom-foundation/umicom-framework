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

static size_t find_index(
    const UmiDeveloperBookmarkStore *store,
    const char *bookmark_id)
{
    size_t index;

    if (store == NULL || bookmark_id == NULL) return (size_t)-1;

    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->bookmarks[index].bookmark_id, bookmark_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

void umi_developer_bookmark_store_init(
    UmiDeveloperBookmarkStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

UmiStatus umi_developer_bookmark_store_upsert(
    UmiDeveloperBookmarkStore *store,
    const UmiDeveloperBookmark *bookmark)
{
    size_t index;
    UmiStatus status;

    if (store == NULL || bookmark == NULL ||
        bookmark->bookmark_id[0] == '\0' ||
        bookmark->label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &bookmark->location);
    if (status != UMI_STATUS_OK) return status;

    index = find_index(store, bookmark->bookmark_id);
    if (index == (size_t)-1) {
        if (store->count >= UMI_DEVELOPER_BOOKMARK_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = store->count++;
    }

    store->bookmarks[index] = *bookmark;
    store->bookmarks[index].revision = ++store->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_bookmark_store_remove(
    UmiDeveloperBookmarkStore *store,
    const char *bookmark_id)
{
    size_t index;
    size_t tail;

    if (store == NULL || bookmark_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(store, bookmark_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = store->count - index - 1U;
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

UmiStatus umi_developer_bookmark_store_at(
    const UmiDeveloperBookmarkStore *store,
    size_t index,
    UmiDeveloperBookmark *out_bookmark)
{
    if (store == NULL || out_bookmark == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= store->count) return UMI_STATUS_NOT_FOUND;

    *out_bookmark = store->bookmarks[index];
    return UMI_STATUS_OK;
}
