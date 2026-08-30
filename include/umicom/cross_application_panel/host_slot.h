/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/host_slot.h
 *
 * PURPOSE:
 *   Define cross-application panel host slot state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_HOST_SLOT_H
#define UMICOM_CROSS_APPLICATION_PANEL_HOST_SLOT_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelHostSlot {
    uint32_t structure_size;
    char slot_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char container_id[UMI_PANEL_TEXT_CAPACITY];
    UmiPanelPlacement placement;
    uint32_t order;
    bool visible;
    uint64_t revision;
} UmiPanelHostSlot;
typedef struct UmiPanelHostSlotStore { UmiPanelHostSlot items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelHostSlotStore;
void umi_panel_host_slot_init(UmiPanelHostSlot *record);
UmiStatus umi_panel_host_slot_validate(const UmiPanelHostSlot *record);
void umi_panel_host_slot_store_init(UmiPanelHostSlotStore *store);
UmiStatus umi_panel_host_slot_store_put(UmiPanelHostSlotStore *store,const UmiPanelHostSlot *record);
UmiStatus umi_panel_host_slot_store_remove(UmiPanelHostSlotStore *store,const char *identity);
UmiPanelHostSlot *umi_panel_host_slot_store_find(UmiPanelHostSlotStore *store,const char *identity);
const UmiPanelHostSlot *umi_panel_host_slot_store_find_const(const UmiPanelHostSlotStore *store,const char *identity);
UmiStatus umi_panel_host_slot_store_snapshot(const UmiPanelHostSlotStore *store,UmiPanelHostSlot *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
