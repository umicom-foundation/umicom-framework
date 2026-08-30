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
typedef struct UmiPanelBookmarkStore { UmiPanelBookmark items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelBookmarkStore;
void umi_panel_bookmark_init(UmiPanelBookmark *record);
UmiStatus umi_panel_bookmark_validate(const UmiPanelBookmark *record);
void umi_panel_bookmark_store_init(UmiPanelBookmarkStore *store);
UmiStatus umi_panel_bookmark_store_put(UmiPanelBookmarkStore *store,const UmiPanelBookmark *record);
UmiStatus umi_panel_bookmark_store_remove(UmiPanelBookmarkStore *store,const char *identity);
UmiPanelBookmark *umi_panel_bookmark_store_find(UmiPanelBookmarkStore *store,const char *identity);
const UmiPanelBookmark *umi_panel_bookmark_store_find_const(const UmiPanelBookmarkStore *store,const char *identity);
UmiStatus umi_panel_bookmark_store_snapshot(const UmiPanelBookmarkStore *store,UmiPanelBookmark *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
