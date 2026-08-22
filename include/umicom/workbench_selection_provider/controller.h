/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/controller.h
 *
 * PURPOSE:
 *   Define a structured-selection Provider Slave Controller for Master Controller lifecycle orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_H
#include "umicom/workbench_selection_provider/service.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchSelectionProviderControllerState {
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_INITIALISED = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_RUNNING = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_QUIESCED = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_STOPPED = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_CONTROLLER_FAILED = 5
} UmiWorkbenchSelectionProviderControllerState;

typedef struct UmiWorkbenchSelectionProviderSlaveController {
    UmiWorkbenchSelectionProviderService *service;
    UmiWorkbenchSelectionProviderControllerState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiWorkbenchSelectionProviderSlaveController;

void umi_workbench_selection_provider_slave_controller_init(
    UmiWorkbenchSelectionProviderSlaveController *controller,
    UmiWorkbenchSelectionProviderService *service);
UmiStatus umi_workbench_selection_provider_slave_controller_start(
    UmiWorkbenchSelectionProviderSlaveController *controller);
UmiStatus umi_workbench_selection_provider_slave_controller_quiesce(
    UmiWorkbenchSelectionProviderSlaveController *controller);
UmiStatus umi_workbench_selection_provider_slave_controller_resume(
    UmiWorkbenchSelectionProviderSlaveController *controller);
UmiStatus umi_workbench_selection_provider_slave_controller_stop(
    UmiWorkbenchSelectionProviderSlaveController *controller);

#ifdef __cplusplus
}
#endif
#endif
