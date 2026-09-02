/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/capability.h
 *
 * PURPOSE:
 *   Define cross-application panel capability state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CAPABILITY_H
#define UMICOM_CROSS_APPLICATION_PANEL_CAPABILITY_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel capability data shared with callers of this public contract.
 */
typedef struct UmiPanelCapability {
    uint32_t structure_size;
    char capability_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char required_capability[UMI_PANEL_TEXT_CAPACITY];
    char optional_capability[UMI_PANEL_TEXT_CAPACITY];
    bool available;
    bool degraded;
    uint64_t revision;
} UmiPanelCapability;
/**
 * Represent the panel capability store data shared with callers of this public contract.
 */
typedef struct UmiPanelCapabilityStore { UmiPanelCapability items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelCapabilityStore;
/**
 * Initialise panel capability from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_capability_init(UmiPanelCapability *record);
/**
 * Check that panel capability satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_capability_validate(const UmiPanelCapability *record);
/**
 * Initialise panel capability store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_capability_store_init(UmiPanelCapabilityStore *store);
/**
 * Provide the panel capability store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_capability_store_put(UmiPanelCapabilityStore *store,const UmiPanelCapability *record);
/**
 * Remove panel capability store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_capability_store_remove(UmiPanelCapabilityStore *store,const char *identity);
/**
 * Find panel capability store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelCapability *umi_panel_capability_store_find(UmiPanelCapabilityStore *store,const char *identity);
/**
 * Provide the panel capability store find const operation used by this module and its
 * client applications.
 */
const UmiPanelCapability *umi_panel_capability_store_find_const(const UmiPanelCapabilityStore *store,const char *identity);
/**
 * Provide the panel capability store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_capability_store_snapshot(const UmiPanelCapabilityStore *store,UmiPanelCapability *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
