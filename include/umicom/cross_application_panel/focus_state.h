/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/focus_state.h
 *
 * PURPOSE:
 *   Define cross-application panel focus state state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FOCUS_STATE_H
#define UMICOM_CROSS_APPLICATION_PANEL_FOCUS_STATE_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel focus state data shared with callers of this public contract.
 */
typedef struct UmiPanelFocusState {
    uint32_t structure_size;
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char previous_instance_id[UMI_PANEL_TEXT_CAPACITY];
    uint32_t reason;
    bool focused;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiPanelFocusState;
/**
 * Represent the panel focus state store data shared with callers of this public contract.
 */
typedef struct UmiPanelFocusStateStore { UmiPanelFocusState items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelFocusStateStore;
/**
 * Initialise panel focus state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_focus_state_init(UmiPanelFocusState *record);
/**
 * Check that panel focus state satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_focus_state_validate(const UmiPanelFocusState *record);
/**
 * Initialise panel focus state store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_focus_state_store_init(UmiPanelFocusStateStore *store);
/**
 * Provide the panel focus state store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_focus_state_store_put(UmiPanelFocusStateStore *store,const UmiPanelFocusState *record);
/**
 * Remove panel focus state store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_focus_state_store_remove(UmiPanelFocusStateStore *store,const char *identity);
/**
 * Find panel focus state store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelFocusState *umi_panel_focus_state_store_find(UmiPanelFocusStateStore *store,const char *identity);
/**
 * Provide the panel focus state store find const operation used by this module and its
 * client applications.
 */
const UmiPanelFocusState *umi_panel_focus_state_store_find_const(const UmiPanelFocusStateStore *store,const char *identity);
/**
 * Provide the panel focus state store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_focus_state_store_snapshot(const UmiPanelFocusStateStore *store,UmiPanelFocusState *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
