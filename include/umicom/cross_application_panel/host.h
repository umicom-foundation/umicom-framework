/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/host.h
 *
 * PURPOSE:
 *   Expose the reusable panel host service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_HOST_H
#define UMICOM_CROSS_APPLICATION_PANEL_HOST_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelHostState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelHostState;
void umi_panel_host_state_init(UmiPanelHostState *state);
UmiStatus umi_panel_host_record(UmiPanelHostState *state,UmiStatus status,const char *message);
bool umi_panel_host_healthy(const UmiPanelHostState *state);
uint64_t umi_panel_host_operation_count(const UmiPanelHostState *state);
#ifdef __cplusplus
}
#endif
#endif
