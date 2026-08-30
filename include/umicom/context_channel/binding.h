/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/binding.h
 *
 * PURPOSE:
 *   Bind reusable panels to channels through explicit contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_BINDING_H
#define UMICOM_CONTEXT_CHANNEL_BINDING_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_BINDING_MAX_ITEMS 256U
typedef struct UmiContextBinding {
    uint32_t structure_size;
    char binding_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char accepted_schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool publish_enabled;
    bool observe_enabled;
    uint64_t revision;
} UmiContextBinding;
typedef struct UmiContextBindingStore {
    UmiContextBinding items[UMI_CONTEXT_BINDING_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextBindingStore;
void umi_context_binding_init(UmiContextBinding *record);
UmiStatus umi_context_binding_validate(const UmiContextBinding *record);
void umi_context_binding_store_init(UmiContextBindingStore *store);
UmiStatus umi_context_binding_store_put(UmiContextBindingStore *store,const UmiContextBinding *record);
UmiStatus umi_context_binding_store_remove(UmiContextBindingStore *store,const char *identity);
UmiContextBinding *umi_context_binding_store_find(UmiContextBindingStore *store,const char *identity);
const UmiContextBinding *umi_context_binding_store_find_const(const UmiContextBindingStore *store,const char *identity);
size_t umi_context_binding_store_count(const UmiContextBindingStore *store);
UmiStatus umi_context_binding_store_snapshot(const UmiContextBindingStore *store,UmiContextBinding *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
