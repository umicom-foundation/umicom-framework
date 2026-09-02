/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/provider.h
 *
 * PURPOSE:
 *   Register context producers and external context providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PROVIDER_H
#define UMICOM_CONTEXT_CHANNEL_PROVIDER_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PROVIDER_MAX_ITEMS 64U
/**
 * Represent the context provider data shared with callers of this public contract.
 */
typedef struct UmiContextProvider {
    uint32_t structure_size;
    char provider_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char display_name[UMI_CONTEXT_TEXT_CAPACITY];
    char capability_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool enabled;
    bool healthy;
    uint64_t revision;
} UmiContextProvider;
/**
 * Represent the context provider store data shared with callers of this public contract.
 */
typedef struct UmiContextProviderStore {
    UmiContextProvider items[UMI_CONTEXT_PROVIDER_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextProviderStore;
/**
 * Initialise context provider from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_provider_init(UmiContextProvider *record);
/**
 * Check that context provider satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_provider_validate(const UmiContextProvider *record);
/**
 * Initialise context provider store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_provider_store_init(UmiContextProviderStore *store);
/**
 * Provide the context provider store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_provider_store_put(UmiContextProviderStore *store,const UmiContextProvider *record);
/**
 * Remove context provider store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_provider_store_remove(UmiContextProviderStore *store,const char *identity);
/**
 * Find context provider store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextProvider *umi_context_provider_store_find(UmiContextProviderStore *store,const char *identity);
/**
 * Provide the context provider store find const operation used by this module and its
 * client applications.
 */
const UmiContextProvider *umi_context_provider_store_find_const(const UmiContextProviderStore *store,const char *identity);
/**
 * Return the number of records represented by context provider store without changing
 * their state.
 */
size_t umi_context_provider_store_count(const UmiContextProviderStore *store);
/**
 * Provide the context provider store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_provider_store_snapshot(const UmiContextProviderStore *store,UmiContextProvider *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
