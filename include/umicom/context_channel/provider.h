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
typedef struct UmiContextProviderStore {
    UmiContextProvider items[UMI_CONTEXT_PROVIDER_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextProviderStore;
void umi_context_provider_init(UmiContextProvider *record);
UmiStatus umi_context_provider_validate(const UmiContextProvider *record);
void umi_context_provider_store_init(UmiContextProviderStore *store);
UmiStatus umi_context_provider_store_put(UmiContextProviderStore *store,const UmiContextProvider *record);
UmiStatus umi_context_provider_store_remove(UmiContextProviderStore *store,const char *identity);
UmiContextProvider *umi_context_provider_store_find(UmiContextProviderStore *store,const char *identity);
const UmiContextProvider *umi_context_provider_store_find_const(const UmiContextProviderStore *store,const char *identity);
size_t umi_context_provider_store_count(const UmiContextProviderStore *store);
UmiStatus umi_context_provider_store_snapshot(const UmiContextProviderStore *store,UmiContextProvider *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
