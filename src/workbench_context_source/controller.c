/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/controller.c
 *
 * PURPOSE:
 *   Implement the live interaction source Slave Controller lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/controller.h"

#include <string.h>

void umi_workbench_context_source_slave_controller_init(
    UmiWorkbenchContextSourceSlaveController *controller,
    UmiWorkbenchContextSourceService *service)
{
    if (controller == NULL) return;
    memset(controller, 0, sizeof(*controller));
    controller->service = service;
    controller->state =
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_INITIALISED;
    controller->last_status =
        service != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    controller->revision = 1U;
}

UmiStatus umi_workbench_context_source_slave_controller_start(
    UmiWorkbenchContextSourceSlaveController *controller)
{
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->state !=
            UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_INITIALISED &&
        controller->state !=
            UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_QUIESCED) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_context_source_service_set_suspended(
        controller->service, false);
    controller->state =
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_RUNNING;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_source_slave_controller_quiesce(
    UmiWorkbenchContextSourceSlaveController *controller)
{
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->state !=
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_context_source_service_set_suspended(
        controller->service, true);
    controller->state =
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_QUIESCED;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_source_slave_controller_resume(
    UmiWorkbenchContextSourceSlaveController *controller)
{
    return umi_workbench_context_source_slave_controller_start(
        controller);
}

UmiStatus umi_workbench_context_source_slave_controller_stop(
    UmiWorkbenchContextSourceSlaveController *controller)
{
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_source_service_set_suspended(
        controller->service, true);
    controller->state =
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_STOPPED;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}
