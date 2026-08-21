/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/workspace_binding.h
 *
 * PURPOSE:
 *   Define cross-application panel workspace binding state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_BINDING_H
#define UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_BINDING_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
typedef struct UmiPanelWorkspaceBindingStore { UmiPanelWorkspaceBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelWorkspaceBindingStore;
void umi_panel_workspace_binding_init(UmiPanelWorkspaceBinding *record);
UmiStatus umi_panel_workspace_binding_validate(const UmiPanelWorkspaceBinding *record);
void umi_panel_workspace_binding_store_init(UmiPanelWorkspaceBindingStore *store);
UmiStatus umi_panel_workspace_binding_store_put(UmiPanelWorkspaceBindingStore *store,const UmiPanelWorkspaceBinding *record);
UmiStatus umi_panel_workspace_binding_store_remove(UmiPanelWorkspaceBindingStore *store,const char *identity);
UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find(UmiPanelWorkspaceBindingStore *store,const char *identity);
const UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find_const(const UmiPanelWorkspaceBindingStore *store,const char *identity);
UmiStatus umi_panel_workspace_binding_store_snapshot(const UmiPanelWorkspaceBindingStore *store,UmiPanelWorkspaceBinding *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
