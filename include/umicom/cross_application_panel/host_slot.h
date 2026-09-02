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
/**
 * Represent the panel host slot data shared with callers of this public contract.
 */
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
/**
 * Represent the panel host slot store data shared with callers of this public contract.
 */
typedef struct UmiPanelHostSlotStore { UmiPanelHostSlot items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelHostSlotStore;
/**
 * Initialise panel host slot from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_host_slot_init(UmiPanelHostSlot *record);
/**
 * Check that panel host slot satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_host_slot_validate(const UmiPanelHostSlot *record);
/**
 * Initialise panel host slot store from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_host_slot_store_init(UmiPanelHostSlotStore *store);
/**
 * Provide the panel host slot store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_host_slot_store_put(UmiPanelHostSlotStore *store,const UmiPanelHostSlot *record);
/**
 * Remove panel host slot store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_host_slot_store_remove(UmiPanelHostSlotStore *store,const char *identity);
/**
 * Find panel host slot store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelHostSlot *umi_panel_host_slot_store_find(UmiPanelHostSlotStore *store,const char *identity);
/**
 * Provide the panel host slot store find const operation used by this module and its
 * client applications.
 */
const UmiPanelHostSlot *umi_panel_host_slot_store_find_const(const UmiPanelHostSlotStore *store,const char *identity);
/**
 * Provide the panel host slot store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_host_slot_store_snapshot(const UmiPanelHostSlotStore *store,UmiPanelHostSlot *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
