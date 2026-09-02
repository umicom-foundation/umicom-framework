/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/provider_state.h
 *
 * PURPOSE:
 *   Track health evidence for context providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PROVIDER_STATE_H
#define UMICOM_CONTEXT_CHANNEL_PROVIDER_STATE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PROVIDER_STATE_MAX_ITEMS 64U
/**
 * Represent the context provider state data shared with callers of this public contract.
 */
typedef struct UmiContextProviderState {
    uint32_t structure_size;
    char provider_id[UMI_CONTEXT_TEXT_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    UmiStatus status;
    uint64_t last_success_ms;
    uint64_t last_failure_ms;
    uint64_t publish_count;
    uint64_t failure_count;
    uint64_t revision;
} UmiContextProviderState;
/**
 * Represent the context provider state store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextProviderStateStore {
    UmiContextProviderState items[UMI_CONTEXT_PROVIDER_STATE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextProviderStateStore;
/**
 * Initialise context provider state from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_provider_state_init(UmiContextProviderState *record);
/**
 * Check that context provider state satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_provider_state_validate(const UmiContextProviderState *record);
/**
 * Initialise context provider state store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_provider_state_store_init(UmiContextProviderStateStore *store);
/**
 * Provide the context provider state store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_provider_state_store_put(UmiContextProviderStateStore *store,const UmiContextProviderState *record);
/**
 * Remove context provider state store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_provider_state_store_remove(UmiContextProviderStateStore *store,const char *identity);
/**
 * Find context provider state store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiContextProviderState *umi_context_provider_state_store_find(UmiContextProviderStateStore *store,const char *identity);
/**
 * Provide the context provider state store find const operation used by this module and
 * its client applications.
 */
const UmiContextProviderState *umi_context_provider_state_store_find_const(const UmiContextProviderStateStore *store,const char *identity);
/**
 * Return the number of records represented by context provider state store without
 * changing their state.
 */
size_t umi_context_provider_state_store_count(const UmiContextProviderStateStore *store);
/**
 * Provide the context provider state store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_provider_state_store_snapshot(const UmiContextProviderStateStore *store,UmiContextProviderState *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
