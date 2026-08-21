/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/command_router.h
 *
 * PURPOSE:
 *   Expose the reusable panel command router service state and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_COMMAND_ROUTER_H
#define UMICOM_CROSS_APPLICATION_PANEL_COMMAND_ROUTER_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelCommandRouterState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelCommandRouterState;
void umi_panel_command_router_state_init(UmiPanelCommandRouterState *state);
UmiStatus umi_panel_command_router_record(UmiPanelCommandRouterState *state,UmiStatus status,const char *message);
bool umi_panel_command_router_healthy(const UmiPanelCommandRouterState *state);
uint64_t umi_panel_command_router_operation_count(const UmiPanelCommandRouterState *state);
#ifdef __cplusplus
}
#endif
#endif
