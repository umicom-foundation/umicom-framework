/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/controller.c
 *
 * PURPOSE:
 *   Implement Provider Slave Controller lifecycle without changing established Master/Slave terminology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/controller.h"

#include <string.h>

/*
 * Initialise workbench selection provider slave controller from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_slave_controller_init(
    UmiWorkbenchSelectionProviderSlaveController *controller,
    UmiWorkbenchSelectionProviderService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return;
    memset(controller, 0, sizeof(*controller));
    controller->service = service;
    controller->state =
        UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_INITIALISED;
    controller->last_status =
        service != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    controller->revision = 1U;
}

/*
 * Provide the workbench selection provider slave controller start operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_start(
    UmiWorkbenchSelectionProviderSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
            UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_INITIALISED &&
        controller->state !=
            UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_QUIESCED) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_selection_provider_service_set_suspended(
        controller->service, false);
    controller->state =
        UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_RUNNING;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_quiesce(
    UmiWorkbenchSelectionProviderSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
        UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    umi_workbench_selection_provider_service_set_suspended(
        controller->service, true);
    controller->state =
        UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_QUIESCED;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider slave controller resume operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_resume(
    UmiWorkbenchSelectionProviderSlaveController *controller)
{
    return umi_workbench_selection_provider_slave_controller_start(
        controller);
}

/*
 * Provide the workbench selection provider slave controller stop operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_stop(
    UmiWorkbenchSelectionProviderSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_selection_provider_service_set_suspended(
        controller->service, true);
    controller->state =
        UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_STOPPED;
    controller->last_status = UMI_STATUS_OK;
    ++controller->revision;
    return UMI_STATUS_OK;
}
