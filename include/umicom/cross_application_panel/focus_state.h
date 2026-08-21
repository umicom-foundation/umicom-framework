/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/focus_state.h
 *
 * PURPOSE:
 *   Define cross-application panel focus state state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FOCUS_STATE_H
#define UMICOM_CROSS_APPLICATION_PANEL_FOCUS_STATE_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelFocusState {
    uint32_t structure_size;
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char previous_instance_id[UMI_PANEL_TEXT_CAPACITY];
    uint32_t reason;
    bool focused;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiPanelFocusState;
typedef struct UmiPanelFocusStateStore { UmiPanelFocusState items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelFocusStateStore;
void umi_panel_focus_state_init(UmiPanelFocusState *record);
UmiStatus umi_panel_focus_state_validate(const UmiPanelFocusState *record);
void umi_panel_focus_state_store_init(UmiPanelFocusStateStore *store);
UmiStatus umi_panel_focus_state_store_put(UmiPanelFocusStateStore *store,const UmiPanelFocusState *record);
UmiStatus umi_panel_focus_state_store_remove(UmiPanelFocusStateStore *store,const char *identity);
UmiPanelFocusState *umi_panel_focus_state_store_find(UmiPanelFocusStateStore *store,const char *identity);
const UmiPanelFocusState *umi_panel_focus_state_store_find_const(const UmiPanelFocusStateStore *store,const char *identity);
UmiStatus umi_panel_focus_state_store_snapshot(const UmiPanelFocusStateStore *store,UmiPanelFocusState *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
