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
typedef struct UmiPanelDiagnosticsState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelDiagnosticsState;
void umi_panel_diagnostics_state_init(UmiPanelDiagnosticsState *state);
UmiStatus umi_panel_diagnostics_record(UmiPanelDiagnosticsState *state,UmiStatus status,const char *message);
bool umi_panel_diagnostics_healthy(const UmiPanelDiagnosticsState *state);
uint64_t umi_panel_diagnostics_operation_count(const UmiPanelDiagnosticsState *state);
#ifdef __cplusplus
}
#endif
#endif
