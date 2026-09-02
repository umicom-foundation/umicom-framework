/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/controller.h
 *
 * PURPOSE:
 *   Define the Layout/Workbench Context Link Slave Controller lifecycle contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTROLLER_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTROLLER_H

#include "umicom/workbench_context_link/service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench context link controller state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextLinkControllerState {
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_FAILED = 6
} UmiWorkbenchContextLinkControllerState;

/**
 * Represent the workbench context link slave controller data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkSlaveController {
    UmiWorkbenchContextLinkService service;
    UmiWorkbenchContextLinkControllerState state;
    uint64_t lifecycle_revision;
    UmiStatus last_status;
} UmiWorkbenchContextLinkSlaveController;

/**
 * Initialise workbench context link slave controller from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_slave_controller_init(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Release or reset state held by workbench context link slave controller so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_slave_controller_destroy(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Provide the workbench context link slave controller start operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_slave_controller_start(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Provide the workbench context link slave controller quiesce operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_slave_controller_quiesce(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Provide the workbench context link slave controller resume operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_slave_controller_resume(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Provide the workbench context link slave controller stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_slave_controller_stop(
    UmiWorkbenchContextLinkSlaveController *controller);
/**
 * Provide the workbench context link slave controller service operation used by this
 * module and its client applications.
 */
UmiWorkbenchContextLinkService *
umi_workbench_context_link_slave_controller_service(
    UmiWorkbenchContextLinkSlaveController *controller);

#ifdef __cplusplus
}
#endif

#endif
