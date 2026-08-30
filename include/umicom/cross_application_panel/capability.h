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
typedef struct UmiPanelCapabilityStore { UmiPanelCapability items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelCapabilityStore;
void umi_panel_capability_init(UmiPanelCapability *record);
UmiStatus umi_panel_capability_validate(const UmiPanelCapability *record);
void umi_panel_capability_store_init(UmiPanelCapabilityStore *store);
UmiStatus umi_panel_capability_store_put(UmiPanelCapabilityStore *store,const UmiPanelCapability *record);
UmiStatus umi_panel_capability_store_remove(UmiPanelCapabilityStore *store,const char *identity);
UmiPanelCapability *umi_panel_capability_store_find(UmiPanelCapabilityStore *store,const char *identity);
const UmiPanelCapability *umi_panel_capability_store_find_const(const UmiPanelCapabilityStore *store,const char *identity);
UmiStatus umi_panel_capability_store_snapshot(const UmiPanelCapabilityStore *store,UmiPanelCapability *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
