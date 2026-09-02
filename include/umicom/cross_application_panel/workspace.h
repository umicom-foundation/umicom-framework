/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/workspace.h
 *
 * PURPOSE:
 *   Expose the reusable panel workspace service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_H
#define UMICOM_CROSS_APPLICATION_PANEL_WORKSPACE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel workspace state data shared with callers of this public contract.
 */
typedef struct UmiPanelWorkspaceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelWorkspaceState;
/**
 * Initialise panel workspace state from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_workspace_state_init(UmiPanelWorkspaceState *state);
/**
 * Provide the panel workspace record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_workspace_record(UmiPanelWorkspaceState *state,UmiStatus status,const char *message);
/**
 * Provide the panel workspace healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_workspace_healthy(const UmiPanelWorkspaceState *state);
/**
 * Return the number of records represented by panel workspace operation without changing
 * their state.
 */
uint64_t umi_panel_workspace_operation_count(const UmiPanelWorkspaceState *state);
#ifdef __cplusplus
}
#endif
#endif
