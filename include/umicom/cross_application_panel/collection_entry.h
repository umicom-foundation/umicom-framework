/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/collection_entry.h
 *
 * PURPOSE:
 *   Define cross-application panel collection entry state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_ENTRY_H
#define UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_ENTRY_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel collection entry data shared with callers of this public contract.
 */
typedef struct UmiPanelCollectionEntry {
    uint32_t structure_size;
    char entry_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char group_id[UMI_PANEL_TEXT_CAPACITY];
    uint32_t order;
    bool selected;
    uint64_t revision;
} UmiPanelCollectionEntry;
/**
 * Represent the panel collection entry store data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelCollectionEntryStore { UmiPanelCollectionEntry items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelCollectionEntryStore;
/**
 * Initialise panel collection entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_collection_entry_init(UmiPanelCollectionEntry *record);
/**
 * Check that panel collection entry satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_panel_collection_entry_validate(const UmiPanelCollectionEntry *record);
/**
 * Initialise panel collection entry store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_collection_entry_store_init(UmiPanelCollectionEntryStore *store);
/**
 * Provide the panel collection entry store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_collection_entry_store_put(UmiPanelCollectionEntryStore *store,const UmiPanelCollectionEntry *record);
/**
 * Remove panel collection entry store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_collection_entry_store_remove(UmiPanelCollectionEntryStore *store,const char *identity);
/**
 * Find panel collection entry store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiPanelCollectionEntry *umi_panel_collection_entry_store_find(UmiPanelCollectionEntryStore *store,const char *identity);
/**
 * Provide the panel collection entry store find const operation used by this module and
 * its client applications.
 */
const UmiPanelCollectionEntry *umi_panel_collection_entry_store_find_const(const UmiPanelCollectionEntryStore *store,const char *identity);
/**
 * Provide the panel collection entry store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_collection_entry_store_snapshot(const UmiPanelCollectionEntryStore *store,UmiPanelCollectionEntry *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
