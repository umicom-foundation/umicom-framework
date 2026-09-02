/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/command_router.h
 *
 * PURPOSE:
 *   Expose the reusable panel command router service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the panel command router state data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelCommandRouterState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelCommandRouterState;
/**
 * Initialise panel command router state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_command_router_state_init(UmiPanelCommandRouterState *state);
/**
 * Provide the panel command router record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_command_router_record(UmiPanelCommandRouterState *state,UmiStatus status,const char *message);
/**
 * Provide the panel command router healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_command_router_healthy(const UmiPanelCommandRouterState *state);
/**
 * Return the number of records represented by panel command router operation without
 * changing their state.
 */
uint64_t umi_panel_command_router_operation_count(const UmiPanelCommandRouterState *state);
#ifdef __cplusplus
}
#endif
#endif
