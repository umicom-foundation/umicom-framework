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
typedef struct UmiContextProviderStateStore {
    UmiContextProviderState items[UMI_CONTEXT_PROVIDER_STATE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextProviderStateStore;
void umi_context_provider_state_init(UmiContextProviderState *record);
UmiStatus umi_context_provider_state_validate(const UmiContextProviderState *record);
void umi_context_provider_state_store_init(UmiContextProviderStateStore *store);
UmiStatus umi_context_provider_state_store_put(UmiContextProviderStateStore *store,const UmiContextProviderState *record);
UmiStatus umi_context_provider_state_store_remove(UmiContextProviderStateStore *store,const char *identity);
UmiContextProviderState *umi_context_provider_state_store_find(UmiContextProviderStateStore *store,const char *identity);
const UmiContextProviderState *umi_context_provider_state_store_find_const(const UmiContextProviderStateStore *store,const char *identity);
size_t umi_context_provider_state_store_count(const UmiContextProviderStateStore *store);
UmiStatus umi_context_provider_state_store_snapshot(const UmiContextProviderStateStore *store,UmiContextProviderState *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
