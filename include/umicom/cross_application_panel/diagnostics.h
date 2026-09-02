/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/diagnostics.h
 *
 * PURPOSE:
 *   Expose the reusable panel diagnostics service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTICS_H
#define UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTICS_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel diagnostics state data shared with callers of this public contract.
 */
typedef struct UmiPanelDiagnosticsState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelDiagnosticsState;
/**
 * Initialise panel diagnostics state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_diagnostics_state_init(UmiPanelDiagnosticsState *state);
/**
 * Provide the panel diagnostics record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostics_record(UmiPanelDiagnosticsState *state,UmiStatus status,const char *message);
/**
 * Provide the panel diagnostics healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_diagnostics_healthy(const UmiPanelDiagnosticsState *state);
/**
 * Return the number of records represented by panel diagnostics operation without changing
 * their state.
 */
uint64_t umi_panel_diagnostics_operation_count(const UmiPanelDiagnosticsState *state);
#ifdef __cplusplus
}
#endif
#endif
