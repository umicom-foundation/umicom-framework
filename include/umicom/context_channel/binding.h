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
/**
 * Represent the context binding data shared with callers of this public contract.
 */
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
/**
 * Represent the context binding store data shared with callers of this public contract.
 */
typedef struct UmiContextBindingStore {
    UmiContextBinding items[UMI_CONTEXT_BINDING_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextBindingStore;
/**
 * Initialise context binding from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_binding_init(UmiContextBinding *record);
/**
 * Check that context binding satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_binding_validate(const UmiContextBinding *record);
/**
 * Initialise context binding store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_binding_store_init(UmiContextBindingStore *store);
/**
 * Provide the context binding store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_binding_store_put(UmiContextBindingStore *store,const UmiContextBinding *record);
/**
 * Remove context binding store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_binding_store_remove(UmiContextBindingStore *store,const char *identity);
/**
 * Find context binding store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextBinding *umi_context_binding_store_find(UmiContextBindingStore *store,const char *identity);
/**
 * Provide the context binding store find const operation used by this module and its
 * client applications.
 */
const UmiContextBinding *umi_context_binding_store_find_const(const UmiContextBindingStore *store,const char *identity);
/**
 * Return the number of records represented by context binding store without changing their
 * state.
 */
size_t umi_context_binding_store_count(const UmiContextBindingStore *store);
/**
 * Provide the context binding store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_binding_store_snapshot(const UmiContextBindingStore *store,UmiContextBinding *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
