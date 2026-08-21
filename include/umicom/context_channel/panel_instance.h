/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/panel_instance.h
 *
 * PURPOSE:
 *   Track runtime panel instances placed into shared layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PANEL_INSTANCE_H
#define UMICOM_CONTEXT_CHANNEL_PANEL_INSTANCE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PANEL_INSTANCE_MAX_ITEMS 256U
typedef struct UmiCrossApplicationPanelInstance {
    uint32_t structure_size;
    char instance_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char layout_node_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool visible;
    bool active;
    uint64_t last_context_sequence;
    uint64_t revision;
} UmiCrossApplicationPanelInstance;
typedef struct UmiCrossApplicationPanelInstanceStore {
    UmiCrossApplicationPanelInstance items[UMI_CONTEXT_PANEL_INSTANCE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCrossApplicationPanelInstanceStore;
void umi_context_panel_instance_init(UmiCrossApplicationPanelInstance *record);
UmiStatus umi_context_panel_instance_validate(const UmiCrossApplicationPanelInstance *record);
void umi_context_panel_instance_store_init(UmiCrossApplicationPanelInstanceStore *store);
UmiStatus umi_context_panel_instance_store_put(UmiCrossApplicationPanelInstanceStore *store,const UmiCrossApplicationPanelInstance *record);
UmiStatus umi_context_panel_instance_store_remove(UmiCrossApplicationPanelInstanceStore *store,const char *identity);
UmiCrossApplicationPanelInstance *umi_context_panel_instance_store_find(UmiCrossApplicationPanelInstanceStore *store,const char *identity);
const UmiCrossApplicationPanelInstance *umi_context_panel_instance_store_find_const(const UmiCrossApplicationPanelInstanceStore *store,const char *identity);
size_t umi_context_panel_instance_store_count(const UmiCrossApplicationPanelInstanceStore *store);
UmiStatus umi_context_panel_instance_store_snapshot(const UmiCrossApplicationPanelInstanceStore *store,UmiCrossApplicationPanelInstance *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
