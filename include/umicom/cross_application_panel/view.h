/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/view.h
 *
 * PURPOSE:
 *   Define cross-application panel view state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_VIEW_H
#define UMICOM_CROSS_APPLICATION_PANEL_VIEW_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelView {
    uint32_t structure_size;
    char view_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char title[UMI_PANEL_TEXT_CAPACITY];
    char subtitle[UMI_PANEL_DESCRIPTION_CAPACITY];
    char empty_message[UMI_PANEL_DESCRIPTION_CAPACITY];
    bool visible;
    bool active;
    uint64_t revision;
} UmiPanelView;
typedef struct UmiPanelViewStore { UmiPanelView items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelViewStore;
void umi_panel_view_init(UmiPanelView *record);
UmiStatus umi_panel_view_validate(const UmiPanelView *record);
void umi_panel_view_store_init(UmiPanelViewStore *store);
UmiStatus umi_panel_view_store_put(UmiPanelViewStore *store,const UmiPanelView *record);
UmiStatus umi_panel_view_store_remove(UmiPanelViewStore *store,const char *identity);
UmiPanelView *umi_panel_view_store_find(UmiPanelViewStore *store,const char *identity);
const UmiPanelView *umi_panel_view_store_find_const(const UmiPanelViewStore *store,const char *identity);
UmiStatus umi_panel_view_store_snapshot(const UmiPanelViewStore *store,UmiPanelView *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
