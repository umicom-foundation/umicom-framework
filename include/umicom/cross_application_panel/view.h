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
/**
 * Represent the panel view data shared with callers of this public contract.
 */
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
/**
 * Represent the panel view store data shared with callers of this public contract.
 */
typedef struct UmiPanelViewStore { UmiPanelView items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelViewStore;
/**
 * Initialise panel view from caller-provided values so later operations receive a known
 * state.
 */
void umi_panel_view_init(UmiPanelView *record);
/**
 * Check that panel view satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_view_validate(const UmiPanelView *record);
/**
 * Initialise panel view store from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_view_store_init(UmiPanelViewStore *store);
/**
 * Provide the panel view store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_view_store_put(UmiPanelViewStore *store,const UmiPanelView *record);
/**
 * Remove panel view store while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_panel_view_store_remove(UmiPanelViewStore *store,const char *identity);
/**
 * Find panel view store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelView *umi_panel_view_store_find(UmiPanelViewStore *store,const char *identity);
/**
 * Provide the panel view store find const operation used by this module and its client
 * applications.
 */
const UmiPanelView *umi_panel_view_store_find_const(const UmiPanelViewStore *store,const char *identity);
/**
 * Provide the panel view store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_view_store_snapshot(const UmiPanelViewStore *store,UmiPanelView *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
