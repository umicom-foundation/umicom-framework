/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/workbench_bridge.c
 *
 * PURPOSE:
 *   Turn the existing Developer Workbench's core Debug menu commands into real
 *   Framework DAP operations when configuration/session context is available.
 *
 * LIFECYCLE POLICY:
 *   Command enablement must not depend on producing the full diagnostic/debug
 *   snapshot. A newly created runtime has valid start/attach capability before
 *   an adapter exists, while an adapter pointer is the platform-owned lifecycle
 *   boundary for a bound/in-flight debug session. Deep snapshots remain useful
 *   for paused/thread state but are not a prerequisite for starting debugging.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the context can start operation used by this module and its client applications. */
static int context_can_start(
    const UmiDebugRuntimeWorkbenchBridge *bridge)
{
    return bridge != NULL &&
        bridge->runtime != NULL &&
        bridge->context.profile_id[0] != '\0' &&
        bridge->context.session_id[0] != '\0' &&
        bridge->context.configuration_id[0] != '\0';
}

/*
 * Provide the runtime session bound operation used by this module and its client
 * applications.
 */
static int runtime_session_bound(
    const UmiDebugRuntimeWorkbenchBridge *bridge)
{
    return bridge != NULL &&
        bridge->runtime != NULL &&
        umi_debug_runtime_platform_adapter(bridge->runtime) != NULL;
}

/* Provide the start enabled operation used by this module and its client applications. */
static int start_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    (void)argument;

    /*
     * A configured, adapter-free runtime is ready to launch/attach. This avoids
     * over-coupling command availability to a deep snapshot whose nested Debug
     * Service views may legitimately be unbound before the first session.
     */
    return context_can_start(bridge) && !runtime_session_bound(bridge);
}

/* Provide the active enabled operation used by this module and its client applications. */
static int active_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    (void)argument;

    return runtime_session_bound(bridge);
}

/* Provide the continue enabled operation used by this module and its client applications. */
static int continue_enabled(void *user_data, const char *argument)
{
    UmiDebugRuntimeWorkbenchBridge *bridge =
        (UmiDebugRuntimeWorkbenchBridge *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    (void)argument;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!runtime_session_bound(bridge) || bridge->context.thread_id == 0U) {
        return 0;
    }

    /*
     * Continue additionally requires paused state. Once a session is bound the
     * full snapshot is appropriate because the underlying debugger models are
     * expected to have operational session state.
     */
    return umi_debug_runtime_platform_snapshot(
        bridge->runtime,
        &snapshot) == UMI_STATUS_OK &&
        snapshot.active &&
        snapshot.paused;
}

/* Provide the start action operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the attach action operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the continue action operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the stop action operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Initialise debug runtime workbench bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_runtime_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDebugRuntimePlatform *runtime,
    UmiDebugRuntimeWorkbenchBridge **out_bridge)
{
    UmiDebugRuntimeWorkbenchBridge *bridge;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || runtime == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;
    bridge = (UmiDebugRuntimeWorkbenchBridge *)calloc(
        1U, sizeof(*bridge));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->workbench = workbench;
    bridge->runtime = runtime;
    bridge->context.timeout_ms = 1000U;

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug runtime workbench bridge so the same storage can be
 * reused safely.
 */
void umi_debug_runtime_workbench_bridge_destroy(
    UmiDebugRuntimeWorkbenchBridge *bridge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL) return;

    /* The workbench stores callback context as a borrowed pointer. Remove all
     * bridge-owned bindings before releasing that context to prevent a later
     * enablement query from dereferencing freed memory. */
    if (bridge->workbench != NULL) {
        (void)umi_developer_workbench_unbind_action(
            bridge->workbench, "debug.start");
        (void)umi_developer_workbench_unbind_action(
            bridge->workbench, "debug.attach");
        (void)umi_developer_workbench_unbind_action(
            bridge->workbench, "debug.continue");
        (void)umi_developer_workbench_unbind_action(
            bridge->workbench, "debug.stop");
    }
    free(bridge);
}

/*
 * Provide the debug runtime workbench bridge set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_workbench_bridge_set_context(
    UmiDebugRuntimeWorkbenchBridge *bridge,
    const UmiDebugRuntimeWorkbenchContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->context = *context;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bridge->context.timeout_ms == 0U) {
        bridge->context.timeout_ms = 1000U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime workbench bridge bind operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_workbench_bridge_bind(
    UmiDebugRuntimeWorkbenchBridge *bridge)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.start",
        start_action,
        start_enabled,
        bridge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.attach",
        attach_action,
        start_enabled,
        bridge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.continue",
        continue_action,
        continue_enabled,
        bridge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_workbench_bind_action(
        bridge->workbench,
        "debug.stop",
        stop_action,
        active_enabled,
        bridge);
}
