/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/collection_entry.h
 *
 * PURPOSE:
 *   Define cross-application panel collection entry state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_ENTRY_H
#define UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_ENTRY_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
typedef struct UmiPanelCollectionEntryStore { UmiPanelCollectionEntry items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelCollectionEntryStore;
void umi_panel_collection_entry_init(UmiPanelCollectionEntry *record);
UmiStatus umi_panel_collection_entry_validate(const UmiPanelCollectionEntry *record);
void umi_panel_collection_entry_store_init(UmiPanelCollectionEntryStore *store);
UmiStatus umi_panel_collection_entry_store_put(UmiPanelCollectionEntryStore *store,const UmiPanelCollectionEntry *record);
UmiStatus umi_panel_collection_entry_store_remove(UmiPanelCollectionEntryStore *store,const char *identity);
UmiPanelCollectionEntry *umi_panel_collection_entry_store_find(UmiPanelCollectionEntryStore *store,const char *identity);
const UmiPanelCollectionEntry *umi_panel_collection_entry_store_find_const(const UmiPanelCollectionEntryStore *store,const char *identity);
UmiStatus umi_panel_collection_entry_store_snapshot(const UmiPanelCollectionEntryStore *store,UmiPanelCollectionEntry *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
