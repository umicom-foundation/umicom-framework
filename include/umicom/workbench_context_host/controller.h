/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/controller.h
 *
 * PURPOSE:
 *   Define a Framework Slave Controller that governs one workbench context host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_CONTROLLER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_CONTROLLER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named workbench context host controller state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostControllerState {
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_CREATED=1,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_INITIALISED=2,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING=3,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_QUIESCED=4,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_STOPPED=5,
    UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_FAILED=6
} UmiWorkbenchContextHostControllerState;
/**
 * Represent the workbench context host slave controller data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostSlaveController {
    UmiWorkbenchContextHost *host;
    UmiWorkbenchContextHostControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchContextHostSlaveController;
/**
 * Initialise workbench context host slave controller from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_slave_controller_init(
    UmiWorkbenchContextHostSlaveController *controller,UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host slave controller start operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_start(
    UmiWorkbenchContextHostSlaveController *controller);
/**
 * Provide the workbench context host slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_quiesce(
    UmiWorkbenchContextHostSlaveController *controller);
/**
 * Provide the workbench context host slave controller resume operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_resume(
    UmiWorkbenchContextHostSlaveController *controller);
/**
 * Provide the workbench context host slave controller stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_slave_controller_stop(
    UmiWorkbenchContextHostSlaveController *controller);
#ifdef __cplusplus
}
#endif
#endif
