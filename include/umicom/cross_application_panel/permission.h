/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/permission.h
 *
 * PURPOSE:
 *   Define cross-application panel permission state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_H
#define UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel permission data shared with callers of this public contract.
 */
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
/**
 * Represent the panel permission store data shared with callers of this public contract.
 */
typedef struct UmiPanelPermissionStore { UmiPanelPermission items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelPermissionStore;
/**
 * Initialise panel permission from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_permission_init(UmiPanelPermission *record);
/**
 * Check that panel permission satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_permission_validate(const UmiPanelPermission *record);
/**
 * Initialise panel permission store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_permission_store_init(UmiPanelPermissionStore *store);
/**
 * Provide the panel permission store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_permission_store_put(UmiPanelPermissionStore *store,const UmiPanelPermission *record);
/**
 * Remove panel permission store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_permission_store_remove(UmiPanelPermissionStore *store,const char *identity);
/**
 * Find panel permission store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelPermission *umi_panel_permission_store_find(UmiPanelPermissionStore *store,const char *identity);
/**
 * Provide the panel permission store find const operation used by this module and its
 * client applications.
 */
const UmiPanelPermission *umi_panel_permission_store_find_const(const UmiPanelPermissionStore *store,const char *identity);
/**
 * Provide the panel permission store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_permission_store_snapshot(const UmiPanelPermissionStore *store,UmiPanelPermission *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
