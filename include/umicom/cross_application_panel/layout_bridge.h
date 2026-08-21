/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/layout_bridge.h
 *
 * PURPOSE:
 *   Expose the reusable panel layout bridge service state and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_LAYOUT_BRIDGE_H
#define UMICOM_CROSS_APPLICATION_PANEL_LAYOUT_BRIDGE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelLayoutBridgeState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelLayoutBridgeState;
void umi_panel_layout_bridge_state_init(UmiPanelLayoutBridgeState *state);
UmiStatus umi_panel_layout_bridge_record(UmiPanelLayoutBridgeState *state,UmiStatus status,const char *message);
bool umi_panel_layout_bridge_healthy(const UmiPanelLayoutBridgeState *state);
uint64_t umi_panel_layout_bridge_operation_count(const UmiPanelLayoutBridgeState *state);
#ifdef __cplusplus
}
#endif
#endif
