/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/bookmarks.h
 *
 * PURPOSE:
 *   Retain named source bookmarks independently from editor widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_BOOKMARKS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_BOOKMARKS_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_BOOKMARK_CAPACITY 128U

/**
 * Represent the developer bookmark data shared with callers of this public contract.
 */
typedef struct UmiDeveloperBookmark {
    char bookmark_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char label[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    UmiDeveloperProductivityLocation location;
    uint64_t revision;
} UmiDeveloperBookmark;

/**
 * Represent the developer bookmark store data shared with callers of this public contract.
 */
typedef struct UmiDeveloperBookmarkStore {
    UmiDeveloperBookmark bookmarks[UMI_DEVELOPER_BOOKMARK_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperBookmarkStore;

/**
 * Initialise developer bookmark store from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_bookmark_store_init(
    UmiDeveloperBookmarkStore *store);

/**
 * Provide the developer bookmark store upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_bookmark_store_upsert(
    UmiDeveloperBookmarkStore *store,
    const UmiDeveloperBookmark *bookmark);

/**
 * Remove developer bookmark store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_developer_bookmark_store_remove(
    UmiDeveloperBookmarkStore *store,
    const char *bookmark_id);

/**
 * Find developer bookmark store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_bookmark_store_at(
    const UmiDeveloperBookmarkStore *store,
    size_t index,
    UmiDeveloperBookmark *out_bookmark);

#ifdef __cplusplus
}
#endif

#endif
