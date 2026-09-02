/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/permission.h
 *
 * PURPOSE:
 *   Represent user and workload permissions for context operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PERMISSION_H
#define UMICOM_CONTEXT_CHANNEL_PERMISSION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PERMISSION_MAX_ITEMS 128U
/**
 * Represent the context permission data shared with callers of this public contract.
 */
typedef struct UmiContextPermission {
    uint32_t structure_size;
    char permission_id[UMI_CONTEXT_TEXT_CAPACITY];
    char subject_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool allow_publish;
    bool allow_observe;
    bool allow_rebind;
    bool allow_history;
    uint64_t revision;
} UmiContextPermission;
/**
 * Represent the context permission store data shared with callers of this public contract.
 */
typedef struct UmiContextPermissionStore {
    UmiContextPermission items[UMI_CONTEXT_PERMISSION_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextPermissionStore;
/**
 * Initialise context permission from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_permission_init(UmiContextPermission *record);
/**
 * Check that context permission satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_permission_validate(const UmiContextPermission *record);
/**
 * Initialise context permission store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_permission_store_init(UmiContextPermissionStore *store);
/**
 * Provide the context permission store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_permission_store_put(UmiContextPermissionStore *store,const UmiContextPermission *record);
/**
 * Remove context permission store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_permission_store_remove(UmiContextPermissionStore *store,const char *identity);
/**
 * Find context permission store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextPermission *umi_context_permission_store_find(UmiContextPermissionStore *store,const char *identity);
/**
 * Provide the context permission store find const operation used by this module and its
 * client applications.
 */
const UmiContextPermission *umi_context_permission_store_find_const(const UmiContextPermissionStore *store,const char *identity);
/**
 * Return the number of records represented by context permission store without changing
 * their state.
 */
size_t umi_context_permission_store_count(const UmiContextPermissionStore *store);
/**
 * Provide the context permission store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_permission_store_snapshot(const UmiContextPermissionStore *store,UmiContextPermission *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
