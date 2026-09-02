/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/workspace_binding.h
 *
 * PURPOSE:
 *   Define cross-application panel workspace binding state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_BINDING_H
#define UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_BINDING_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel workspace binding data shared with callers of this public contract.
 */
typedef struct UmiPanelWorkspaceBinding {
    uint32_t structure_size;
    char binding_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char workspace_id[UMI_PANEL_TEXT_CAPACITY];
    char layout_id[UMI_PANEL_TEXT_CAPACITY];
    char perspective_id[UMI_PANEL_TEXT_CAPACITY];
    bool enabled;
    uint64_t revision;
} UmiPanelWorkspaceBinding;
/**
 * Represent the panel workspace binding store data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelWorkspaceBindingStore { UmiPanelWorkspaceBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelWorkspaceBindingStore;
/**
 * Initialise panel workspace binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_workspace_binding_init(UmiPanelWorkspaceBinding *record);
/**
 * Check that panel workspace binding satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_panel_workspace_binding_validate(const UmiPanelWorkspaceBinding *record);
/**
 * Initialise panel workspace binding store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_workspace_binding_store_init(UmiPanelWorkspaceBindingStore *store);
/**
 * Provide the panel workspace binding store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_workspace_binding_store_put(UmiPanelWorkspaceBindingStore *store,const UmiPanelWorkspaceBinding *record);
/**
 * Remove panel workspace binding store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_workspace_binding_store_remove(UmiPanelWorkspaceBindingStore *store,const char *identity);
/**
 * Find panel workspace binding store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find(UmiPanelWorkspaceBindingStore *store,const char *identity);
/**
 * Provide the panel workspace binding store find const operation used by this module and
 * its client applications.
 */
const UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find_const(const UmiPanelWorkspaceBindingStore *store,const char *identity);
/**
 * Provide the panel workspace binding store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_workspace_binding_store_snapshot(const UmiPanelWorkspaceBindingStore *store,UmiPanelWorkspaceBinding *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
