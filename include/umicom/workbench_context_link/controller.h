/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/controller.h
 *
 * PURPOSE:
 *   Define the Layout/Workbench Context Link Slave Controller lifecycle contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTROLLER_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTROLLER_H

#include "umicom/workbench_context_link/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchContextLinkControllerState {
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_FAILED = 6
} UmiWorkbenchContextLinkControllerState;

typedef struct UmiWorkbenchContextLinkSlaveController {
    UmiWorkbenchContextLinkService service;
    UmiWorkbenchContextLinkControllerState state;
    uint64_t lifecycle_revision;
    UmiStatus last_status;
} UmiWorkbenchContextLinkSlaveController;

void umi_workbench_context_link_slave_controller_init(
    UmiWorkbenchContextLinkSlaveController *controller);
void umi_workbench_context_link_slave_controller_destroy(
    UmiWorkbenchContextLinkSlaveController *controller);
UmiStatus umi_workbench_context_link_slave_controller_start(
    UmiWorkbenchContextLinkSlaveController *controller);
UmiStatus umi_workbench_context_link_slave_controller_quiesce(
    UmiWorkbenchContextLinkSlaveController *controller);
UmiStatus umi_workbench_context_link_slave_controller_resume(
    UmiWorkbenchContextLinkSlaveController *controller);
UmiStatus umi_workbench_context_link_slave_controller_stop(
    UmiWorkbenchContextLinkSlaveController *controller);
UmiWorkbenchContextLinkService *
umi_workbench_context_link_slave_controller_service(
    UmiWorkbenchContextLinkSlaveController *controller);

#ifdef __cplusplus
}
#endif

#endif
