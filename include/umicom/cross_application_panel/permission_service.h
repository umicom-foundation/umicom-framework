/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/permission_service.h
 *
 * PURPOSE:
 *   Expose the reusable panel permission service service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_SERVICE_H
#define UMICOM_CROSS_APPLICATION_PANEL_PERMISSION_SERVICE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel permission service state data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelPermissionServiceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelPermissionServiceState;
/**
 * Initialise panel permission service state from caller-provided values so later
 * operations receive a known state.
 */
void umi_panel_permission_service_state_init(UmiPanelPermissionServiceState *state);
/**
 * Provide the panel permission service record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_permission_service_record(UmiPanelPermissionServiceState *state,UmiStatus status,const char *message);
/**
 * Provide the panel permission service healthy operation used by this module and its
 * client applications.
 */
bool umi_panel_permission_service_healthy(const UmiPanelPermissionServiceState *state);
/**
 * Return the number of records represented by panel permission service operation without
 * changing their state.
 */
uint64_t umi_panel_permission_service_operation_count(const UmiPanelPermissionServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
