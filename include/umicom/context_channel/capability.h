/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/capability.h
 *
 * PURPOSE:
 *   Declare context publication and observation capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CAPABILITY_H
#define UMICOM_CONTEXT_CHANNEL_CAPABILITY_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_CAPABILITY_MAX_ITEMS 128U
typedef struct UmiContextCapability {
    uint32_t structure_size;
    char capability_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool can_publish;
    bool can_observe;
    bool can_share_cross_application;
    uint64_t revision;
} UmiContextCapability;
typedef struct UmiContextCapabilityStore {
    UmiContextCapability items[UMI_CONTEXT_CAPABILITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextCapabilityStore;
void umi_context_capability_init(UmiContextCapability *record);
UmiStatus umi_context_capability_validate(const UmiContextCapability *record);
void umi_context_capability_store_init(UmiContextCapabilityStore *store);
UmiStatus umi_context_capability_store_put(UmiContextCapabilityStore *store,const UmiContextCapability *record);
UmiStatus umi_context_capability_store_remove(UmiContextCapabilityStore *store,const char *identity);
UmiContextCapability *umi_context_capability_store_find(UmiContextCapabilityStore *store,const char *identity);
const UmiContextCapability *umi_context_capability_store_find_const(const UmiContextCapabilityStore *store,const char *identity);
size_t umi_context_capability_store_count(const UmiContextCapabilityStore *store);
UmiStatus umi_context_capability_store_snapshot(const UmiContextCapabilityStore *store,UmiContextCapability *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
