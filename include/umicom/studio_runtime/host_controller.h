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

/**
 * Represent the studio runtime host controller data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeHostController {
    UmiStudioRuntimeBootstrap *bootstrap;
    UmiStudioRuntimeHostAdapter adapter;
    uint64_t revision;
} UmiStudioRuntimeHostController;

/**
 * Initialise studio host controller from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_host_controller_init(
    UmiStudioRuntimeHostController *controller,
    UmiStudioRuntimeBootstrap *bootstrap,
    const UmiStudioRuntimeHostAdapter *adapter);

/**
 * Provide the studio host controller refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_host_controller_refresh(
    UmiStudioRuntimeHostController *controller);

/**
 * Provide the studio host controller execute command operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_host_controller_execute_command(
    UmiStudioRuntimeHostController *controller,
    const char *command_id,
    const char *argument);

/**
 * Provide the studio host controller execute contribution operation used by this module
 * and its client applications.
 */
UmiStatus umi_studio_host_controller_execute_contribution(
    UmiStudioRuntimeHostController *controller,
    const char *contribution_id,
    const char *argument);

#ifdef __cplusplus
}
#endif
#endif
