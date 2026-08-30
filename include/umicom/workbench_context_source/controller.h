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

typedef enum UmiWorkbenchContextSourceControllerState {
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_FAILED = 6
} UmiWorkbenchContextSourceControllerState;

typedef struct UmiWorkbenchContextSourceSlaveController {
    UmiWorkbenchContextSourceService *service;
    UmiWorkbenchContextSourceControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchContextSourceSlaveController;

void umi_workbench_context_source_slave_controller_init(
    UmiWorkbenchContextSourceSlaveController *controller,
    UmiWorkbenchContextSourceService *service);
UmiStatus umi_workbench_context_source_slave_controller_start(
    UmiWorkbenchContextSourceSlaveController *controller);
UmiStatus umi_workbench_context_source_slave_controller_quiesce(
    UmiWorkbenchContextSourceSlaveController *controller);
UmiStatus umi_workbench_context_source_slave_controller_resume(
    UmiWorkbenchContextSourceSlaveController *controller);
UmiStatus umi_workbench_context_source_slave_controller_stop(
    UmiWorkbenchContextSourceSlaveController *controller);

#ifdef __cplusplus
}
#endif
#endif
