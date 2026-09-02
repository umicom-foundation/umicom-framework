/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/controller.h
 *
 * PURPOSE:
 *   Define a structured-selection Provider Slave Controller for Master Controller lifecycle orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_H
#include "umicom/workbench_selection_provider/service.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench selection provider controller state values accepted by this
 * public contract.
 */
typedef enum UmiWorkbenchSelectionProviderControllerState {
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_INITIALISED = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_RUNNING = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_QUIESCED = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_STOPPED = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_FAILED = 5
} UmiWorkbenchSelectionProviderControllerState;

/**
 * Represent the workbench selection provider slave controller data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSlaveController {
    UmiWorkbenchSelectionProviderService *service;
    UmiWorkbenchSelectionProviderControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchSelectionProviderSlaveController;

/**
 * Initialise workbench selection provider slave controller from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_slave_controller_init(
    UmiWorkbenchSelectionProviderSlaveController *controller,
    UmiWorkbenchSelectionProviderService *service);
/**
 * Provide the workbench selection provider slave controller start operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_start(
    UmiWorkbenchSelectionProviderSlaveController *controller);
/**
 * Provide the workbench selection provider slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_quiesce(
    UmiWorkbenchSelectionProviderSlaveController *controller);
/**
 * Provide the workbench selection provider slave controller resume operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_resume(
    UmiWorkbenchSelectionProviderSlaveController *controller);
/**
 * Provide the workbench selection provider slave controller stop operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_slave_controller_stop(
    UmiWorkbenchSelectionProviderSlaveController *controller);

#ifdef __cplusplus
}
#endif
#endif
