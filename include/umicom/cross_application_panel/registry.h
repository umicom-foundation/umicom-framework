/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/registry.h
 *
 * PURPOSE:
 *   Expose the reusable panel registry service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_REGISTRY_H
#define UMICOM_CROSS_APPLICATION_PANEL_REGISTRY_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel registry state data shared with callers of this public contract.
 */
typedef struct UmiPanelRegistryState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelRegistryState;
/**
 * Initialise panel registry state from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_registry_state_init(UmiPanelRegistryState *state);
/**
 * Provide the panel registry record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_registry_record(UmiPanelRegistryState *state,UmiStatus status,const char *message);
/**
 * Provide the panel registry healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_registry_healthy(const UmiPanelRegistryState *state);
/**
 * Return the number of records represented by panel registry operation without changing
 * their state.
 */
uint64_t umi_panel_registry_operation_count(const UmiPanelRegistryState *state);
#ifdef __cplusplus
}
#endif
#endif
