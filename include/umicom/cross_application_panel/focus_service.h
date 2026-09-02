/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/focus_service.h
 *
 * PURPOSE:
 *   Expose the reusable panel focus service service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FOCUS_SERVICE_H
#define UMICOM_CROSS_APPLICATION_PANEL_FOCUS_SERVICE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel focus service state data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelFocusServiceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelFocusServiceState;
/**
 * Initialise panel focus service state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_focus_service_state_init(UmiPanelFocusServiceState *state);
/**
 * Provide the panel focus service record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_focus_service_record(UmiPanelFocusServiceState *state,UmiStatus status,const char *message);
/**
 * Provide the panel focus service healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_focus_service_healthy(const UmiPanelFocusServiceState *state);
/**
 * Return the number of records represented by panel focus service operation without
 * changing their state.
 */
uint64_t umi_panel_focus_service_operation_count(const UmiPanelFocusServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
