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

typedef struct UmiDeveloperBookmark {
    char bookmark_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char label[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    UmiDeveloperProductivityLocation location;
    uint64_t revision;
} UmiDeveloperBookmark;

typedef struct UmiDeveloperBookmarkStore {
    UmiDeveloperBookmark bookmarks[UMI_DEVELOPER_BOOKMARK_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperBookmarkStore;

void umi_developer_bookmark_store_init(
    UmiDeveloperBookmarkStore *store);

UmiStatus umi_developer_bookmark_store_upsert(
    UmiDeveloperBookmarkStore *store,
    const UmiDeveloperBookmark *bookmark);

UmiStatus umi_developer_bookmark_store_remove(
    UmiDeveloperBookmarkStore *store,
    const char *bookmark_id);

UmiStatus umi_developer_bookmark_store_at(
    const UmiDeveloperBookmarkStore *store,
    size_t index,
    UmiDeveloperBookmark *out_bookmark);

#ifdef __cplusplus
}
#endif

#endif
