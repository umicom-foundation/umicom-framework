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
/**
 * Represent the panel layout binding data shared with callers of this public contract.
 */
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
/**
 * Represent the panel layout binding store data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelLayoutBindingStore { UmiPanelLayoutBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelLayoutBindingStore;
/**
 * Initialise panel layout binding from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_layout_binding_init(UmiPanelLayoutBinding *record);
/**
 * Check that panel layout binding satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_panel_layout_binding_validate(const UmiPanelLayoutBinding *record);
/**
 * Initialise panel layout binding store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_layout_binding_store_init(UmiPanelLayoutBindingStore *store);
/**
 * Provide the panel layout binding store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_layout_binding_store_put(UmiPanelLayoutBindingStore *store,const UmiPanelLayoutBinding *record);
/**
 * Remove panel layout binding store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_layout_binding_store_remove(UmiPanelLayoutBindingStore *store,const char *identity);
/**
 * Find panel layout binding store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelLayoutBinding *umi_panel_layout_binding_store_find(UmiPanelLayoutBindingStore *store,const char *identity);
/**
 * Provide the panel layout binding store find const operation used by this module and its
 * client applications.
 */
const UmiPanelLayoutBinding *umi_panel_layout_binding_store_find_const(const UmiPanelLayoutBindingStore *store,const char *identity);
/**
 * Provide the panel layout binding store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_layout_binding_store_snapshot(const UmiPanelLayoutBindingStore *store,UmiPanelLayoutBinding *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
