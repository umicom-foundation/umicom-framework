/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/permission.h
 *
 * PURPOSE:
 *   Represent user and workload permissions for context operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PERMISSION_H
#define UMICOM_CONTEXT_CHANNEL_PERMISSION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PERMISSION_MAX_ITEMS 128U
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
typedef struct UmiContextPermissionStore {
    UmiContextPermission items[UMI_CONTEXT_PERMISSION_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextPermissionStore;
void umi_context_permission_init(UmiContextPermission *record);
UmiStatus umi_context_permission_validate(const UmiContextPermission *record);
void umi_context_permission_store_init(UmiContextPermissionStore *store);
UmiStatus umi_context_permission_store_put(UmiContextPermissionStore *store,const UmiContextPermission *record);
UmiStatus umi_context_permission_store_remove(UmiContextPermissionStore *store,const char *identity);
UmiContextPermission *umi_context_permission_store_find(UmiContextPermissionStore *store,const char *identity);
const UmiContextPermission *umi_context_permission_store_find_const(const UmiContextPermissionStore *store,const char *identity);
size_t umi_context_permission_store_count(const UmiContextPermissionStore *store);
UmiStatus umi_context_permission_store_snapshot(const UmiContextPermissionStore *store,UmiContextPermission *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
