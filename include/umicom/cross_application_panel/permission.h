/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/permission.h
 *
 * PURPOSE:
 *   Define cross-application panel permission state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_H
#define UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelPermission {
    uint32_t structure_size;
    char permission_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char subject_id[UMI_PANEL_TEXT_CAPACITY];
    bool allow_open;
    bool allow_close;
    bool allow_move;
    bool allow_rebind;
    bool allow_cross_application;
    uint64_t revision;
} UmiPanelPermission;
typedef struct UmiPanelPermissionStore { UmiPanelPermission items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelPermissionStore;
void umi_panel_permission_init(UmiPanelPermission *record);
UmiStatus umi_panel_permission_validate(const UmiPanelPermission *record);
void umi_panel_permission_store_init(UmiPanelPermissionStore *store);
UmiStatus umi_panel_permission_store_put(UmiPanelPermissionStore *store,const UmiPanelPermission *record);
UmiStatus umi_panel_permission_store_remove(UmiPanelPermissionStore *store,const char *identity);
UmiPanelPermission *umi_panel_permission_store_find(UmiPanelPermissionStore *store,const char *identity);
const UmiPanelPermission *umi_panel_permission_store_find_const(const UmiPanelPermissionStore *store,const char *identity);
UmiStatus umi_panel_permission_store_snapshot(const UmiPanelPermissionStore *store,UmiPanelPermission *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
