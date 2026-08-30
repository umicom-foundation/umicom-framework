/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/bookmark.h
 *
 * PURPOSE:
 *   Store user bookmarks for reusable context subjects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_BOOKMARK_H
#define UMICOM_CONTEXT_CHANNEL_BOOKMARK_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_BOOKMARK_MAX_ITEMS 128U
typedef struct UmiContextBookmark {
    uint32_t structure_size;
    char bookmark_id[UMI_CONTEXT_TEXT_CAPACITY];
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char label[UMI_CONTEXT_TEXT_CAPACITY];
    char user_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool shared;
    uint64_t created_at_ms;
    uint64_t revision;
} UmiContextBookmark;
typedef struct UmiContextBookmarkStore {
    UmiContextBookmark items[UMI_CONTEXT_BOOKMARK_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextBookmarkStore;
void umi_context_bookmark_init(UmiContextBookmark *record);
UmiStatus umi_context_bookmark_validate(const UmiContextBookmark *record);
void umi_context_bookmark_store_init(UmiContextBookmarkStore *store);
UmiStatus umi_context_bookmark_store_put(UmiContextBookmarkStore *store,const UmiContextBookmark *record);
UmiStatus umi_context_bookmark_store_remove(UmiContextBookmarkStore *store,const char *identity);
UmiContextBookmark *umi_context_bookmark_store_find(UmiContextBookmarkStore *store,const char *identity);
const UmiContextBookmark *umi_context_bookmark_store_find_const(const UmiContextBookmarkStore *store,const char *identity);
size_t umi_context_bookmark_store_count(const UmiContextBookmarkStore *store);
UmiStatus umi_context_bookmark_store_snapshot(const UmiContextBookmarkStore *store,UmiContextBookmark *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
