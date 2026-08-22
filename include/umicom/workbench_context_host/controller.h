/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/controller.h
 *
 * PURPOSE:
 *   Define a Framework Slave Controller that governs one workbench context host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_CONTROLLER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_CONTROLLER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiWorkbenchContextHostControllerState {
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_CREATED=1,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED=2,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING=3,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED=4,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_STOPPED=5,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_FAILED=6
} UmiWorkbenchContextHostControllerState;
typedef struct UmiWorkbenchContextHostSlaveController {
    UmiWorkbenchContextHost *host;
    UmiWorkbenchContextHostControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchContextHostSlaveController;
void umi_workbench_context_host_slave_controller_init(
    UmiWorkbenchContextHostSlaveController *controller,UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_slave_controller_start(
    UmiWorkbenchContextHostSlaveController *controller);
UmiStatus umi_workbench_context_host_slave_controller_quiesce(
    UmiWorkbenchContextHostSlaveController *controller);
UmiStatus umi_workbench_context_host_slave_controller_resume(
    UmiWorkbenchContextHostSlaveController *controller);
UmiStatus umi_workbench_context_host_slave_controller_stop(
    UmiWorkbenchContextHostSlaveController *controller);
#ifdef __cplusplus
}
#endif
#endif
