/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/capability_service.h
 *
 * PURPOSE:
 *   Expose the reusable panel capability service service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CAPABILITY_SERVICE_H
#define UMICOM_CROSS_APPLICATION_PANEL_CAPABILITY_SERVICE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelCapabilityServiceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelCapabilityServiceState;
void umi_panel_capability_service_state_init(UmiPanelCapabilityServiceState *state);
UmiStatus umi_panel_capability_service_record(UmiPanelCapabilityServiceState *state,UmiStatus status,const char *message);
bool umi_panel_capability_service_healthy(const UmiPanelCapabilityServiceState *state);
uint64_t umi_panel_capability_service_operation_count(const UmiPanelCapabilityServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
