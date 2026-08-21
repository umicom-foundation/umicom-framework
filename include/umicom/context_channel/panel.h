/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/panel.h
 *
 * PURPOSE:
 *   Register reusable cross-application panel contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PANEL_H
#define UMICOM_CONTEXT_CHANNEL_PANEL_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PANEL_MAX_ITEMS 256U
typedef struct UmiCrossApplicationPanel {
    uint32_t structure_size;
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char title[UMI_CONTEXT_TEXT_CAPACITY];
    char component_id[UMI_CONTEXT_TEXT_CAPACITY];
    char default_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool singleton;
    bool context_aware;
    bool enabled;
    uint64_t revision;
} UmiCrossApplicationPanel;
typedef struct UmiCrossApplicationPanelStore {
    UmiCrossApplicationPanel items[UMI_CONTEXT_PANEL_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCrossApplicationPanelStore;
void umi_context_panel_init(UmiCrossApplicationPanel *record);
UmiStatus umi_context_panel_validate(const UmiCrossApplicationPanel *record);
void umi_context_panel_store_init(UmiCrossApplicationPanelStore *store);
UmiStatus umi_context_panel_store_put(UmiCrossApplicationPanelStore *store,const UmiCrossApplicationPanel *record);
UmiStatus umi_context_panel_store_remove(UmiCrossApplicationPanelStore *store,const char *identity);
UmiCrossApplicationPanel *umi_context_panel_store_find(UmiCrossApplicationPanelStore *store,const char *identity);
const UmiCrossApplicationPanel *umi_context_panel_store_find_const(const UmiCrossApplicationPanelStore *store,const char *identity);
size_t umi_context_panel_store_count(const UmiCrossApplicationPanelStore *store);
UmiStatus umi_context_panel_store_snapshot(const UmiCrossApplicationPanelStore *store,UmiCrossApplicationPanel *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
