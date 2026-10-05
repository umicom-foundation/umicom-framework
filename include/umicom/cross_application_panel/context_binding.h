/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/context_binding.h
 *
 * PURPOSE:
 *   Define cross-application panel context binding state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BINDING_H
#define UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BINDING_H
#include "umicom/cross_application_panel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel context binding data shared with callers of this public contract.
 */
typedef struct UmiPanelContextBinding {
    uint32_t structure_size;
    char binding_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char channel_id[UMI_PANEL_TEXT_CAPACITY];
    char schema_id[UMI_PANEL_TEXT_CAPACITY];
    bool follow;
    bool publish;
    uint64_t revision;
} UmiPanelContextBinding;
/**
 * Represent the panel context binding store data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelContextBindingStore { UmiPanelContextBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelContextBindingStore;
/**
 * Initialise panel context binding from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_context_binding_init(UmiPanelContextBinding *record);
/**
 * Check that panel context binding satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_panel_context_binding_validate(const UmiPanelContextBinding *record);
/**
 * Initialise panel context binding store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_context_binding_store_init(UmiPanelContextBindingStore *store);
/**
 * Provide the panel context binding store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_context_binding_store_put(UmiPanelContextBindingStore *store,const UmiPanelContextBinding *record);
/**
 * Remove panel context binding store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_context_binding_store_remove(UmiPanelContextBindingStore *store,const char *identity);
/**
 * Find panel context binding store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiPanelContextBinding *umi_panel_context_binding_store_find(UmiPanelContextBindingStore *store,const char *identity);
/**
 * Provide the panel context binding store find const operation used by this module and its
 * client applications.
 */
const UmiPanelContextBinding *umi_panel_context_binding_store_find_const(const UmiPanelContextBindingStore *store,const char *identity);
/**
 * Provide the panel context binding store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_context_binding_store_snapshot(const UmiPanelContextBindingStore *store,UmiPanelContextBinding *records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_context_binding_archive_encode(const UmiPanelContextBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_context_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelContextBinding *value);

#ifdef __cplusplus
}
#endif
#endif
