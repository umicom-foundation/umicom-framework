/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/context_binding.h
 *
 * PURPOSE:
 *   Define cross-application panel context binding state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BINDING_H
#define UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BINDING_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelContextBinding {
    uint32_t structure_size;
    char binding_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char channel_id[UMI_PANEL_TEXT_CAPACITY];
    char schema_id[UMI_PANEL_TEXT_CAPACITY];
    bool follow;
    bool publish;
    uint64_t revision;
} UmiPanelContextBinding;
typedef struct UmiPanelContextBindingStore { UmiPanelContextBinding items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelContextBindingStore;
void umi_panel_context_binding_init(UmiPanelContextBinding *record);
UmiStatus umi_panel_context_binding_validate(const UmiPanelContextBinding *record);
void umi_panel_context_binding_store_init(UmiPanelContextBindingStore *store);
UmiStatus umi_panel_context_binding_store_put(UmiPanelContextBindingStore *store,const UmiPanelContextBinding *record);
UmiStatus umi_panel_context_binding_store_remove(UmiPanelContextBindingStore *store,const char *identity);
UmiPanelContextBinding *umi_panel_context_binding_store_find(UmiPanelContextBindingStore *store,const char *identity);
const UmiPanelContextBinding *umi_panel_context_binding_store_find_const(const UmiPanelContextBindingStore *store,const char *identity);
UmiStatus umi_panel_context_binding_store_snapshot(const UmiPanelContextBindingStore *store,UmiPanelContextBinding *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
