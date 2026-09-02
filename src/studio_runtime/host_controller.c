/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/host_controller.c
 *
 * PURPOSE:
 *   Implement the thin Studio host command/refresh/presentation loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/host_controller.h"

#include <string.h>

/*
 * Initialise studio host controller from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_host_controller_init(
    UmiStudioRuntimeHostController *controller,
    UmiStudioRuntimeBootstrap *bootstrap,
    const UmiStudioRuntimeHostAdapter *adapter)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || bootstrap == NULL ||
        bootstrap->platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_host_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(controller, 0, sizeof(*controller));
    controller->bootstrap = bootstrap;
    controller->adapter = *adapter;
    controller->revision = 1U;

    return umi_studio_host_sync(
        bootstrap->platform,
        &controller->adapter);
}

/*
 * Provide the studio host controller refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_host_controller_refresh(
    UmiStudioRuntimeHostController *controller)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->bootstrap == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bootstrap_refresh(
        controller->bootstrap);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_host_sync(
        controller->bootstrap->platform,
        &controller->adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) controller->revision += 1U;
    return status;
}

/*
 * Provide the studio host controller execute command operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_host_controller_execute_command(
    UmiStudioRuntimeHostController *controller,
    const char *command_id,
    const char *argument)
{
    char message[UMI_STUDIO_RUNTIME_TEXT_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->bootstrap == NULL ||
        command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    message[0] = '\0';

    status = umi_studio_runtime_platform_execute_command(
        controller->bootstrap->platform,
        command_id,
        argument,
        message,
        sizeof(message));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_host_sync(
        controller->bootstrap->platform,
        &controller->adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller->adapter.notify != NULL && message[0] != '\0') {
        status = controller->adapter.notify(
            controller->adapter.user_data,
            message);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) controller->revision += 1U;
    return status;
}

/*
 * Provide the studio host controller execute contribution operation used by this module
 * and its client applications.
 */
UmiStatus umi_studio_host_controller_execute_contribution(
    UmiStudioRuntimeHostController *controller,
    const char *contribution_id,
    const char *argument)
{
    char message[UMI_STUDIO_RUNTIME_TEXT_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->bootstrap == NULL ||
        contribution_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    message[0] = '\0';

    status = umi_studio_runtime_platform_execute_contribution(
        controller->bootstrap->platform,
        contribution_id,
        argument,
        message,
        sizeof(message));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_host_sync(
        controller->bootstrap->platform,
        &controller->adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller->adapter.notify != NULL && message[0] != '\0') {
        status = controller->adapter.notify(
            controller->adapter.user_data,
            message);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) controller->revision += 1U;
    return status;
}
