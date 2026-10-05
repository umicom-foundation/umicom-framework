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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_permission_archive_encode(const UmiPanelPermission *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_permission_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelPermission *value);

#ifdef __cplusplus
}
#endif
#endif
