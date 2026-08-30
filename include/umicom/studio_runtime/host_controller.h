/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/host_controller.h
 *
 * PURPOSE:
 *   Provide the final thin-application controller: execute Framework commands or
 *   shell contributions, refresh state and synchronize the toolkit host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_HOST_CONTROLLER_H
#define UMICOM_STUDIO_RUNTIME_HOST_CONTROLLER_H

#include "umicom/studio_runtime/bootstrap.h"
#include "umicom/studio_runtime/host_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeHostController {
    UmiStudioRuntimeBootstrap *bootstrap;
    UmiStudioRuntimeHostAdapter adapter;
    uint64_t revision;
} UmiStudioRuntimeHostController;

UmiStatus umi_studio_host_controller_init(
    UmiStudioRuntimeHostController *controller,
    UmiStudioRuntimeBootstrap *bootstrap,
    const UmiStudioRuntimeHostAdapter *adapter);

UmiStatus umi_studio_host_controller_refresh(
    UmiStudioRuntimeHostController *controller);

UmiStatus umi_studio_host_controller_execute_command(
    UmiStudioRuntimeHostController *controller,
    const char *command_id,
    const char *argument);

UmiStatus umi_studio_host_controller_execute_contribution(
    UmiStudioRuntimeHostController *controller,
    const char *contribution_id,
    const char *argument);

#ifdef __cplusplus
}
#endif
#endif
