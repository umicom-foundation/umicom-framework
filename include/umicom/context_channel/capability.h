/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/capability.h
 *
 * PURPOSE:
 *   Declare context publication and observation capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CAPABILITY_H
#define UMICOM_CONTEXT_CHANNEL_CAPABILITY_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_CAPABILITY_MAX_ITEMS 128U
/**
 * Represent the context capability data shared with callers of this public contract.
 */
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
/**
 * Represent the context capability store data shared with callers of this public contract.
 */
typedef struct UmiContextCapabilityStore {
    UmiContextCapability items[UMI_CONTEXT_CAPABILITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextCapabilityStore;
/**
 * Initialise context capability from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_capability_init(UmiContextCapability *record);
/**
 * Check that context capability satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_capability_validate(const UmiContextCapability *record);
/**
 * Initialise context capability store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_capability_store_init(UmiContextCapabilityStore *store);
/**
 * Provide the context capability store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_capability_store_put(UmiContextCapabilityStore *store,const UmiContextCapability *record);
/**
 * Remove context capability store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_capability_store_remove(UmiContextCapabilityStore *store,const char *identity);
/**
 * Find context capability store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextCapability *umi_context_capability_store_find(UmiContextCapabilityStore *store,const char *identity);
/**
 * Provide the context capability store find const operation used by this module and its
 * client applications.
 */
const UmiContextCapability *umi_context_capability_store_find_const(const UmiContextCapabilityStore *store,const char *identity);
/**
 * Return the number of records represented by context capability store without changing
 * their state.
 */
size_t umi_context_capability_store_count(const UmiContextCapabilityStore *store);
/**
 * Provide the context capability store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_capability_store_snapshot(const UmiContextCapabilityStore *store,UmiContextCapability *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
