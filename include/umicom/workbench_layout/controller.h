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

typedef struct UmiWorkbenchLayoutControllerConfig {
    uint32_t structure_size;
    char controller_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    bool reject_commands_while_quiesced;
    bool stop_on_persistence_failure;
    bool emit_command_events;
} UmiWorkbenchLayoutControllerConfig;

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

typedef struct UmiWorkbenchLayoutSlaveController
    UmiWorkbenchLayoutSlaveController;

UmiWorkbenchLayoutControllerConfig
umi_workbench_layout_controller_config_default(void);

UmiStatus umi_workbench_layout_slave_controller_create(
    const UmiWorkbenchLayoutControllerConfig *config,
    UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutSlaveController **out_controller);

void umi_workbench_layout_slave_controller_destroy(
    UmiWorkbenchLayoutSlaveController *controller);

UmiStatus umi_workbench_layout_slave_controller_initialise(
    UmiWorkbenchLayoutSlaveController *controller);

UmiStatus umi_workbench_layout_slave_controller_start(
    UmiWorkbenchLayoutSlaveController *controller);

UmiStatus umi_workbench_layout_slave_controller_quiesce(
    UmiWorkbenchLayoutSlaveController *controller);

UmiStatus umi_workbench_layout_slave_controller_stop(
    UmiWorkbenchLayoutSlaveController *controller,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_layout_slave_controller_handle(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *out_result);

UmiStatus umi_workbench_layout_slave_controller_snapshot(
    const UmiWorkbenchLayoutSlaveController *controller,
    UmiWorkbenchLayoutControllerSnapshot *out_snapshot);

UmiWorkbenchLayoutControllerState
umi_workbench_layout_slave_controller_state(
    const UmiWorkbenchLayoutSlaveController *controller);

#ifdef __cplusplus
}
#endif

#endif
