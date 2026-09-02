/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/bookmark.h
 *
 * PURPOSE:
 *   Define cross-application panel bookmark state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_BOOKMARK_H
#define UMICOM_CROSS_APPLICATION_PANEL_BOOKMARK_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel bookmark data shared with callers of this public contract.
 */
typedef struct UmiPanelBookmark {
    uint32_t structure_size;
    char bookmark_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char label[UMI_PANEL_TEXT_CAPACITY];
    char workspace_id[UMI_PANEL_TEXT_CAPACITY];
    uint64_t created_at_ms;
    uint64_t revision;
} UmiPanelBookmark;
/**
 * Represent the panel bookmark store data shared with callers of this public contract.
 */
typedef struct UmiPanelBookmarkStore { UmiPanelBookmark items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelBookmarkStore;
/**
 * Initialise panel bookmark from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_bookmark_init(UmiPanelBookmark *record);
/**
 * Check that panel bookmark satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_bookmark_validate(const UmiPanelBookmark *record);
/**
 * Initialise panel bookmark store from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_bookmark_store_init(UmiPanelBookmarkStore *store);
/**
 * Provide the panel bookmark store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_bookmark_store_put(UmiPanelBookmarkStore *store,const UmiPanelBookmark *record);
/**
 * Remove panel bookmark store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_bookmark_store_remove(UmiPanelBookmarkStore *store,const char *identity);
/**
 * Find panel bookmark store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelBookmark *umi_panel_bookmark_store_find(UmiPanelBookmarkStore *store,const char *identity);
/**
 * Provide the panel bookmark store find const operation used by this module and its client
 * applications.
 */
const UmiPanelBookmark *umi_panel_bookmark_store_find_const(const UmiPanelBookmarkStore *store,const char *identity);
/**
 * Provide the panel bookmark store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_bookmark_store_snapshot(const UmiPanelBookmarkStore *store,UmiPanelBookmark *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
