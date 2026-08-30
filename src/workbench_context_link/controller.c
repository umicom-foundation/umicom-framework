/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/controller.c
 *
 * PURPOSE:
 *   Implement the Workbench Context Link Slave Controller lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/controller.h"

#include <string.h>

void umi_workbench_context_link_slave_controller_init(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    if (controller == NULL) return;
    memset(controller, 0, sizeof(*controller));
    umi_workbench_context_link_service_init(&controller->service);
    controller->state = UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_INITIALISED;
    controller->lifecycle_revision = 1U;
    controller->last_status = UMI_STATUS_OK;
}

void umi_workbench_context_link_slave_controller_destroy(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    if (controller == NULL) return;
    umi_workbench_context_link_service_destroy(&controller->service);
    memset(controller, 0, sizeof(*controller));
}

UmiStatus umi_workbench_context_link_slave_controller_start(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_INITIALISED &&
        controller->state != UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_QUIESCED) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_context_link_service_set_suspended(&controller->service, false);
    controller->state = UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_RUNNING;
    controller->last_status = UMI_STATUS_OK;
    ++controller->lifecycle_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_slave_controller_quiesce(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_context_link_service_set_suspended(&controller->service, true);
    controller->state = UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_QUIESCED;
    ++controller->lifecycle_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_slave_controller_resume(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    return umi_workbench_context_link_slave_controller_start(controller);
}

UmiStatus umi_workbench_context_link_slave_controller_stop(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state == UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_STOPPED) {
        return UMI_STATUS_OK;
    }
    umi_workbench_context_link_service_set_suspended(&controller->service, true);
    controller->state = UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_STOPPED;
    ++controller->lifecycle_revision;
    return UMI_STATUS_OK;
}

UmiWorkbenchContextLinkService *
umi_workbench_context_link_slave_controller_service(
    UmiWorkbenchContextLinkSlaveController *controller)
{
    return controller == NULL ? NULL : &controller->service;
}
