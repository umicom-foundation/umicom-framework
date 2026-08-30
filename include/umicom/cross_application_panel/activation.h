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
#ifdef __cplusplus
extern "C" {
#endif
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
typedef struct UmiPanelActivationStore { UmiPanelActivation items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelActivationStore;
void umi_panel_activation_init(UmiPanelActivation *record);
UmiStatus umi_panel_activation_validate(const UmiPanelActivation *record);
void umi_panel_activation_store_init(UmiPanelActivationStore *store);
UmiStatus umi_panel_activation_store_put(UmiPanelActivationStore *store,const UmiPanelActivation *record);
UmiStatus umi_panel_activation_store_remove(UmiPanelActivationStore *store,const char *identity);
UmiPanelActivation *umi_panel_activation_store_find(UmiPanelActivationStore *store,const char *identity);
const UmiPanelActivation *umi_panel_activation_store_find_const(const UmiPanelActivationStore *store,const char *identity);
UmiStatus umi_panel_activation_store_snapshot(const UmiPanelActivationStore *store,UmiPanelActivation *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
