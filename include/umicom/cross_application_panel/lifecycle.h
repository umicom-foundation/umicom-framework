/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/lifecycle.h
 *
 * PURPOSE:
 *   Expose the reusable panel lifecycle service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_LIFECYCLE_H
#define UMICOM_CROSS_APPLICATION_PANEL_LIFECYCLE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel lifecycle runtime state data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelLifecycleRuntimeState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelLifecycleRuntimeState;
/**
 * Initialise panel lifecycle state from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_lifecycle_state_init(UmiPanelLifecycleRuntimeState *state);
/**
 * Provide the panel lifecycle record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_lifecycle_record(UmiPanelLifecycleRuntimeState *state,UmiStatus status,const char *message);
/**
 * Provide the panel lifecycle healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_lifecycle_healthy(const UmiPanelLifecycleRuntimeState *state);
/**
 * Return the number of records represented by panel lifecycle operation without changing
 * their state.
 */
uint64_t umi_panel_lifecycle_operation_count(const UmiPanelLifecycleRuntimeState *state);
#ifdef __cplusplus
}
#endif
#endif
