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
/**
 * Represent the panel host state data shared with callers of this public contract.
 */
typedef struct UmiPanelHostState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelHostState;
/**
 * Initialise panel host state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_host_state_init(UmiPanelHostState *state);
/**
 * Provide the panel host record operation used by this module and its client applications.
 */
UmiStatus umi_panel_host_record(UmiPanelHostState *state,UmiStatus status,const char *message);
/**
 * Provide the panel host healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_host_healthy(const UmiPanelHostState *state);
/**
 * Return the number of records represented by panel host operation without changing their
 * state.
 */
uint64_t umi_panel_host_operation_count(const UmiPanelHostState *state);
#ifdef __cplusplus
}
#endif
#endif
