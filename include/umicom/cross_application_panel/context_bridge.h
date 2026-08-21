/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/context_bridge.h
 *
 * PURPOSE:
 *   Expose the reusable panel context bridge service state and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BRIDGE_H
#define UMICOM_CROSS_APPLICATION_PANEL_CONTEXT_BRIDGE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelContextBridgeState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelContextBridgeState;
void umi_panel_context_bridge_state_init(UmiPanelContextBridgeState *state);
UmiStatus umi_panel_context_bridge_record(UmiPanelContextBridgeState *state,UmiStatus status,const char *message);
bool umi_panel_context_bridge_healthy(const UmiPanelContextBridgeState *state);
uint64_t umi_panel_context_bridge_operation_count(const UmiPanelContextBridgeState *state);
#ifdef __cplusplus
}
#endif
#endif
