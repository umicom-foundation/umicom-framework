/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/workbench_bridge.c
 *
 * PURPOSE:
 *   Turn the existing Developer Workbench's core Debug menu commands into real
 *   Framework DAP operations when configuration/session context is available.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/workbench_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugRuntimeWorkbenchBridge {
    UmiDeveloperWorkbench *workbench;
    UmiDebugRuntimePlatform *runtime;
    UmiDebugRuntimeWorkbenchContext context;
};

static int start_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    (void)argument;

    return bridge != NULL &&
        bridge->context.profile_id[0] != '\0' &&
        bridge->context.session_id[0] != '\0' &&
        bridge->context.configuration_id[0] != '\0' &&
        umi_debug_runtime_platform_snapshot(
            bridge->runtime, &snapshot) == UMI_STATUS_OK &&
        !snapshot.active;
}

static int active_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    (void)argument;

    return bridge != NULL &&
        umi_debug_runtime_platform_snapshot(
            bridge->runtime, &snapshot) == UMI_STATUS_OK &&
        snapshot.active;
}

static int continue_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    (void)argument;

    return bridge != NULL &&
        bridge->context.thread_id != 0U &&
        umi_debug_runtime_platform_snapshot(
            bridge->runtime, &snapshot) == UMI_STATUS_OK &&
        snapshot.active &&
        snapshot.paused;
}

static UmiStatus start_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;

    status = umi_debug_runtime_platform_start(
        bridge->runtime,
        bridge->context.profile_id,
        bridge->context.session_id,
        bridge->context.configuration_id,
        bridge->context.launch_arguments_json[0] != '\0'
            ? bridge->context.launch_arguments_json
            : NULL,
        0,
        bridge->context.working_directory[0] != '\0'
            ? bridge->context.working_directory
            : NULL,
        bridge->context.timeout_ms);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "Debug session started."
                : "Debug session failed to start.");
    }

    return status;
}

static UmiStatus attach_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;

    status = umi_debug_runtime_platform_start(
        bridge->runtime,
        bridge->context.profile_id,
        bridge->context.session_id,
        bridge->context.configuration_id,
        bridge->context.attach_arguments_json[0] != '\0'
            ? bridge->context.attach_arguments_json
            : NULL,
        1,
        bridge->context.working_directory[0] != '\0'
            ? bridge->context.working_directory
            : NULL,
        bridge->context.timeout_ms);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "Debugger attached."
                : "Debugger attach failed.");
    }

    return status;
}

static UmiStatus continue_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;

    status = umi_debug_runtime_platform_continue(
        bridge->runtime,
        bridge->context.thread_id,
        bridge->context.timeout_ms);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "Debuggee continued."
                : "Continue failed.");
    }

    return status;
}

static UmiStatus stop_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;

    status = umi_debug_runtime_platform_stop(
        bridge->runtime,
        1,
        bridge->context.timeout_ms);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "Debug session stopped."
                : "Debug stop reported a failure.");
    }

    return status;
}

UmiStatus umi_debug_runtime_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDebugRuntimePlatform *runtime,
    UmiDebugRuntimeWorkbenchBridge **out_bridge)
{
    UmiDebugRuntimeWorkbenchBridge *bridge;

    if (workbench == NULL || runtime == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;
    bridge = (UmiDebugRuntimeWorkbenchBridge *)calloc(
        1U, sizeof(*bridge));
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->workbench = workbench;
    bridge->runtime = runtime;
    bridge->context.timeout_ms = 1000U;

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

void umi_debug_runtime_workbench_bridge_destroy(
    UmiDebugRuntimeWorkbenchBridge *bridge)
{
    free(bridge);
}

UmiStatus umi_debug_runtime_workbench_bridge_set_context(
    UmiDebugRuntimeWorkbenchBridge *bridge,
    const UmiDebugRuntimeWorkbenchContext *context)
{
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->context = *context;
    if (bridge->context.timeout_ms == 0U) {
        bridge->context.timeout_ms = 1000U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_workbench_bridge_bind(
    UmiDebugRuntimeWorkbenchBridge *bridge)
{
    UmiStatus status;

    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.start",
        start_action,
        start_enabled,
        bridge);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.attach",
        attach_action,
        start_enabled,
        bridge);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.continue",
        continue_action,
        continue_enabled,
        bridge);
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.stop",
        stop_action,
        active_enabled,
        bridge);
}
