/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/controller.h
 *
 * PURPOSE:
 *   Define the Layout Slave Controller that receives typed commands from the Master Controller and delegates durable work to the layout service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_CONTROLLER_H
#define UMICOM_WORKBENCH_LAYOUT_CONTROLLER_H

#include "umicom/workbench_layout/commands.h"
#include "umicom/workbench_layout/service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout controller config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutControllerConfig {
    uint32_t structure_size;
    char controller_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    bool reject_commands_while_quiesced;
    bool stop_on_persistence_failure;
    bool emit_command_events;
} UmiWorkbenchLayoutControllerConfig;

/**
 * Represent the workbench layout controller snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutControllerSnapshot {
    uint32_t structure_size;
    char controller_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutControllerState state;
    uint64_t accepted_command_count;
    uint64_t rejected_command_count;
    uint64_t failed_command_count;
    uint64_t revision;
    UmiWorkbenchLayoutServiceSnapshot service;
} UmiWorkbenchLayoutControllerSnapshot;

/**
 * Represent the workbench layout slave controller data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutSlaveController
    UmiWorkbenchLayoutSlaveController;

/**
 * Provide the workbench layout controller config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutControllerConfig
umi_workbench_layout_controller_config_default(void);

/**
 * Initialise workbench layout slave controller from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_slave_controller_create(
    const UmiWorkbenchLayoutControllerConfig *config,
    UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutSlaveController **out_controller);

/**
 * Release or reset state held by workbench layout slave controller so the same storage can
 * be reused safely.
 */
void umi_workbench_layout_slave_controller_destroy(
    UmiWorkbenchLayoutSlaveController *controller);

/**
 * Provide the workbench layout slave controller initialise operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_initialise(
    UmiWorkbenchLayoutSlaveController *controller);

/**
 * Provide the workbench layout slave controller start operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_start(
    UmiWorkbenchLayoutSlaveController *controller);

/**
 * Provide the workbench layout slave controller quiesce operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_quiesce(
    UmiWorkbenchLayoutSlaveController *controller);

/**
 * Provide the workbench layout slave controller stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_stop(
    UmiWorkbenchLayoutSlaveController *controller,
    uint64_t timestamp_ms);

/**
 * Perform workbench layout slave controller through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_slave_controller_handle(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *out_result);

/**
 * Provide the workbench layout slave controller snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_snapshot(
    const UmiWorkbenchLayoutSlaveController *controller,
    UmiWorkbenchLayoutControllerSnapshot *out_snapshot);

/**
 * Provide the workbench layout slave controller state operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutControllerState
umi_workbench_layout_slave_controller_state(
    const UmiWorkbenchLayoutSlaveController *controller);

#ifdef __cplusplus
}
#endif

#endif
