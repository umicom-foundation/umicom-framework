/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/activation.h
 *
 * PURPOSE:
 *   Define cross-application panel activation state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_ACTIVATION_H
#define UMICOM_CROSS_APPLICATION_PANEL_ACTIVATION_H
#include "umicom/cross_application_panel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel activation data shared with callers of this public contract.
 */
typedef struct UmiPanelActivation {
    uint32_t structure_size;
    char activation_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    uint32_t reason;
    char source_id[UMI_PANEL_TEXT_CAPACITY];
    uint64_t timestamp_ms;
    bool successful;
    uint64_t revision;
} UmiPanelActivation;
/**
 * Represent the panel activation store data shared with callers of this public contract.
 */
typedef struct UmiPanelActivationStore { UmiPanelActivation items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelActivationStore;
/**
 * Initialise panel activation from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_activation_init(UmiPanelActivation *record);
/**
 * Check that panel activation satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_activation_validate(const UmiPanelActivation *record);
/**
 * Initialise panel activation store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_activation_store_init(UmiPanelActivationStore *store);
/**
 * Provide the panel activation store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_activation_store_put(UmiPanelActivationStore *store,const UmiPanelActivation *record);
/**
 * Remove panel activation store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_activation_store_remove(UmiPanelActivationStore *store,const char *identity);
/**
 * Find panel activation store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelActivation *umi_panel_activation_store_find(UmiPanelActivationStore *store,const char *identity);
/**
 * Provide the panel activation store find const operation used by this module and its
 * client applications.
 */
const UmiPanelActivation *umi_panel_activation_store_find_const(const UmiPanelActivationStore *store,const char *identity);
/**
 * Provide the panel activation store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_activation_store_snapshot(const UmiPanelActivationStore *store,UmiPanelActivation *records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_activation_archive_encode(const UmiPanelActivation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_activation_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelActivation *value);

#ifdef __cplusplus
}
#endif
#endif
