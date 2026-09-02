/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/controller.h
 *
 * PURPOSE:
 *   Define a Workbench Context Source Slave Controller for Master Controller lifecycle orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_H
#include "umicom/workbench_context_source/service.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench context source controller state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextSourceControllerState {
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_FAILED = 6
} UmiWorkbenchContextSourceControllerState;

/**
 * Represent the workbench context source slave controller data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceSlaveController {
    UmiWorkbenchContextSourceService *service;
    UmiWorkbenchContextSourceControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchContextSourceSlaveController;

/**
 * Initialise workbench context source slave controller from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_slave_controller_init(
    UmiWorkbenchContextSourceSlaveController *controller,
    UmiWorkbenchContextSourceService *service);
/**
 * Provide the workbench context source slave controller start operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_slave_controller_start(
    UmiWorkbenchContextSourceSlaveController *controller);
/**
 * Provide the workbench context source slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_slave_controller_quiesce(
    UmiWorkbenchContextSourceSlaveController *controller);
/**
 * Provide the workbench context source slave controller resume operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_slave_controller_resume(
    UmiWorkbenchContextSourceSlaveController *controller);
/**
 * Provide the workbench context source slave controller stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_slave_controller_stop(
    UmiWorkbenchContextSourceSlaveController *controller);

#ifdef __cplusplus
}
#endif
#endif
