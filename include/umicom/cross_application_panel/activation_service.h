/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/activation_service.h
 *
 * PURPOSE:
 *   Expose the reusable panel activation service service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_ACTIVATION_SERVICE_H
#define UMICOM_CROSS_APPLICATION_PANEL_ACTIVATION_SERVICE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel activation service state data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelActivationServiceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelActivationServiceState;
/**
 * Initialise panel activation service state from caller-provided values so later
 * operations receive a known state.
 */
void umi_panel_activation_service_state_init(UmiPanelActivationServiceState *state);
/**
 * Provide the panel activation service record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_activation_service_record(UmiPanelActivationServiceState *state,UmiStatus status,const char *message);
/**
 * Provide the panel activation service healthy operation used by this module and its
 * client applications.
 */
bool umi_panel_activation_service_healthy(const UmiPanelActivationServiceState *state);
/**
 * Return the number of records represented by panel activation service operation without
 * changing their state.
 */
uint64_t umi_panel_activation_service_operation_count(const UmiPanelActivationServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
