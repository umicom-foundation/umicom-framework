/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/layout_binding.h
 *
 * PURPOSE:
 *   Define cross-application panel layout binding state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_LAYOUT_BINDING_H
#define UMICOM_CROSS_APPLICATION_PANEL_LAYOUT_BINDING_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelLayoutBinding {
    uint32_t structure_size;
    char binding_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char layout_id[UMI_PANEL_TEXT_CAPACITY];
    char node_id[UMI_PANEL_TEXT_CAPACITY];
    UmiPanelPlacement placement;
    uint32_t order;
    uint64_t revision;
} UmiPanelLayoutBinding;
typedef struct UmiPanelLayoutBindingStore { UmiPanelLayoutBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelLayoutBindingStore;
void umi_panel_layout_binding_init(UmiPanelLayoutBinding *record);
UmiStatus umi_panel_layout_binding_validate(const UmiPanelLayoutBinding *record);
void umi_panel_layout_binding_store_init(UmiPanelLayoutBindingStore *store);
UmiStatus umi_panel_layout_binding_store_put(UmiPanelLayoutBindingStore *store,const UmiPanelLayoutBinding *record);
UmiStatus umi_panel_layout_binding_store_remove(UmiPanelLayoutBindingStore *store,const char *identity);
UmiPanelLayoutBinding *umi_panel_layout_binding_store_find(UmiPanelLayoutBindingStore *store,const char *identity);
const UmiPanelLayoutBinding *umi_panel_layout_binding_store_find_const(const UmiPanelLayoutBindingStore *store,const char *identity);
UmiStatus umi_panel_layout_binding_store_snapshot(const UmiPanelLayoutBindingStore *store,UmiPanelLayoutBinding *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
