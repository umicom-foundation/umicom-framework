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
/**
 * Represent the context bookmark data shared with callers of this public contract.
 */
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
/**
 * Represent the context bookmark store data shared with callers of this public contract.
 */
typedef struct UmiContextBookmarkStore {
    UmiContextBookmark items[UMI_CONTEXT_BOOKMARK_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextBookmarkStore;
/**
 * Initialise context bookmark from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_bookmark_init(UmiContextBookmark *record);
/**
 * Check that context bookmark satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_bookmark_validate(const UmiContextBookmark *record);
/**
 * Initialise context bookmark store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_bookmark_store_init(UmiContextBookmarkStore *store);
/**
 * Provide the context bookmark store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_bookmark_store_put(UmiContextBookmarkStore *store,const UmiContextBookmark *record);
/**
 * Remove context bookmark store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_bookmark_store_remove(UmiContextBookmarkStore *store,const char *identity);
/**
 * Find context bookmark store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextBookmark *umi_context_bookmark_store_find(UmiContextBookmarkStore *store,const char *identity);
/**
 * Provide the context bookmark store find const operation used by this module and its
 * client applications.
 */
const UmiContextBookmark *umi_context_bookmark_store_find_const(const UmiContextBookmarkStore *store,const char *identity);
/**
 * Return the number of records represented by context bookmark store without changing
 * their state.
 */
size_t umi_context_bookmark_store_count(const UmiContextBookmarkStore *store);
/**
 * Provide the context bookmark store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_bookmark_store_snapshot(const UmiContextBookmarkStore *store,UmiContextBookmark *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
