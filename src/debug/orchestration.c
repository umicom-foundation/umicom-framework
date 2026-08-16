/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/orchestration.c
 *
 * PURPOSE:
 *   Implement Framework-owned debugger configuration, query and command-state
 *   coordination for every frontend and application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/orchestration.h"

#include <stdlib.h>
#include <string.h>

struct UmiDebugOrchestration {
    UmiDebugService *service;
    UmiDebugController *controller;
    UmiDebugWorkspace *workspace;
    UmiDebugConfigurationResolver *resolver;
    UmiDebugBreakpointQuery *breakpoints;
    UmiDebugWatchQuery *watches;
    UmiDebugConsoleQuery *console;
    char selected_configuration_id[128];
    char selected_session_id[128];
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

UmiStatus umi_debug_orchestration_create(
    UmiDebugService *service, UmiDebugController *controller,
    UmiDebugWorkspace *workspace,
    UmiDebugOrchestration **out_orchestration)
{
    UmiDebugOrchestration *orchestration;
    UmiStatus status;
    if (service == NULL || out_orchestration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_orchestration = NULL;
    orchestration = (UmiDebugOrchestration *)calloc(1U,
                                                    sizeof(*orchestration));
    if (orchestration == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    orchestration->service = service;
    orchestration->controller = controller;
    orchestration->workspace = workspace;
    orchestration->revision = 1U;
    status = umi_debug_configuration_resolver_create(&orchestration->resolver);
    if (status == UMI_STATUS_OK) {
        status = umi_debug_breakpoint_query_create(&orchestration->breakpoints);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_debug_watch_query_create(&orchestration->watches);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_debug_console_query_create(&orchestration->console);
    }
    if (status != UMI_STATUS_OK) {
        umi_debug_orchestration_destroy(orchestration);
        return status;
    }
    *out_orchestration = orchestration;
    return UMI_STATUS_OK;
}

void umi_debug_orchestration_destroy(UmiDebugOrchestration *orchestration)
{
    if (orchestration == NULL) return;
    umi_debug_console_query_destroy(orchestration->console);
    umi_debug_watch_query_destroy(orchestration->watches);
    umi_debug_breakpoint_query_destroy(orchestration->breakpoints);
    umi_debug_configuration_resolver_destroy(orchestration->resolver);
    free(orchestration);
}

UmiStatus umi_debug_orchestration_select_configuration(
    UmiDebugOrchestration *orchestration,
    const UmiDebugConfigurationRequest *request)
{
    UmiStatus status;
    if (orchestration == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_configuration_resolver_resolve(
        orchestration->resolver,
        umi_debug_service_launch_configuration(orchestration->service),
        umi_debug_service_adapter_profiles(orchestration->service), request);
    if (status == UMI_STATUS_OK) {
        copy_text(orchestration->selected_configuration_id,
                  sizeof(orchestration->selected_configuration_id),
                  request->configuration_id);
    }
    orchestration->revision = next_revision(orchestration->revision);
    return status;
}

UmiStatus umi_debug_orchestration_select_session(
    UmiDebugOrchestration *orchestration, const char *session_id)
{
    UmiDebugSessionSnapshot session;
    UmiStatus status;
    if (orchestration == NULL || session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_session_registry_find(
        umi_debug_service_session(orchestration->service), session_id, &session);
    if (status != UMI_STATUS_OK) return status;
    copy_text(orchestration->selected_session_id,
              sizeof(orchestration->selected_session_id), session_id);
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_orchestration_refresh(
    UmiDebugOrchestration *orchestration,
    const UmiDebugBreakpointFilter *breakpoint_filter,
    const UmiDebugWatchFilter *watch_filter,
    const UmiDebugConsoleFilter *console_filter)
{
    UmiDebugBreakpointFilter default_breakpoint_filter;
    UmiDebugWatchFilter default_watch_filter;
    UmiDebugConsoleFilter default_console_filter;
    UmiStatus status;
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (breakpoint_filter == NULL) {
        umi_debug_breakpoint_filter_init(&default_breakpoint_filter);
        breakpoint_filter = &default_breakpoint_filter;
    }
    if (watch_filter == NULL) {
        umi_debug_watch_filter_init(&default_watch_filter);
        watch_filter = &default_watch_filter;
    }
    if (console_filter == NULL) {
        umi_debug_console_filter_init(&default_console_filter);
        console_filter = &default_console_filter;
    }
    status = umi_debug_breakpoint_query_execute(
        orchestration->breakpoints,
        umi_debug_service_breakpoint(orchestration->service),
        breakpoint_filter);
    if (status == UMI_STATUS_OK) {
        status = umi_debug_watch_query_execute(
            orchestration->watches,
            umi_debug_service_watch(orchestration->service), watch_filter);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_debug_console_query_execute(
            orchestration->console,
            umi_debug_service_console_entry(orchestration->service),
            console_filter);
    }
    if (status == UMI_STATUS_OK && orchestration->workspace != NULL) {
        status = umi_debug_workspace_refresh(orchestration->workspace);
    }
    if (status == UMI_STATUS_OK) {
        orchestration->revision = next_revision(orchestration->revision);
    }
    return status;
}

static UmiDebugControllerState controller_state(
    const UmiDebugOrchestration *orchestration)
{
    UmiDebugControllerSnapshot snapshot;
    if (orchestration->controller == NULL ||
        umi_debug_controller_snapshot(orchestration->controller, &snapshot) !=
            UMI_STATUS_OK) {
        return UMI_DEBUG_CONTROLLER_IDLE;
    }
    return snapshot.state;
}

int umi_debug_orchestration_command_enabled(
    const UmiDebugOrchestration *orchestration, UmiDebugCommandKind command)
{
    const UmiDebugCommandDescriptor *descriptor;
    UmiDebugResolvedConfiguration configuration;
    UmiDebugControllerState state;
    int configuration_ready;
    int active_session;
    if (orchestration == NULL) return 0;
    descriptor = umi_debug_command_for_kind(command);
    if (descriptor == NULL) return 0;
    configuration_ready =
        umi_debug_configuration_resolver_result(orchestration->resolver,
                                                 &configuration) ==
            UMI_STATUS_OK && configuration.ready;
    state = controller_state(orchestration);
    active_session = orchestration->selected_session_id[0] != '\0' ||
        (state != UMI_DEBUG_CONTROLLER_IDLE &&
         state != UMI_DEBUG_CONTROLLER_TERMINATED &&
         state != UMI_DEBUG_CONTROLLER_FAILED);
    if (descriptor->requires_configuration && !configuration_ready) return 0;
    if (descriptor->requires_active_session && !active_session) return 0;
    if (descriptor->requires_paused_session &&
        state != UMI_DEBUG_CONTROLLER_PAUSED) return 0;
    switch (command) {
        case UMI_DEBUG_COMMAND_START:
        case UMI_DEBUG_COMMAND_START_WITHOUT_DEBUGGING:
            return configuration_ready &&
                (state == UMI_DEBUG_CONTROLLER_IDLE ||
                 state == UMI_DEBUG_CONTROLLER_TERMINATED ||
                 state == UMI_DEBUG_CONTROLLER_FAILED);
        case UMI_DEBUG_COMMAND_CONTINUE:
            return state == UMI_DEBUG_CONTROLLER_PAUSED;
        case UMI_DEBUG_COMMAND_PAUSE:
            return state == UMI_DEBUG_CONTROLLER_RUNNING;
        case UMI_DEBUG_COMMAND_STEP_OVER:
        case UMI_DEBUG_COMMAND_STEP_INTO:
        case UMI_DEBUG_COMMAND_STEP_OUT:
        case UMI_DEBUG_COMMAND_RUN_TO_CURSOR:
            return state == UMI_DEBUG_CONTROLLER_PAUSED;
        case UMI_DEBUG_COMMAND_STOP:
        case UMI_DEBUG_COMMAND_RESTART:
            return active_session;
        default:
            return 1;
    }
}

UmiStatus umi_debug_orchestration_snapshot(
    UmiDebugOrchestration *orchestration,
    UmiDebugOrchestrationSnapshot *out_snapshot)
{
    UmiDebugResolvedConfiguration configuration;
    UmiDebugServiceSnapshot service;
    UmiStatus status;
    if (orchestration == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_service_snapshot(orchestration->service, &service);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_ORCHESTRATION_API_VERSION;
    copy_text(out_snapshot->selected_configuration_id,
              sizeof(out_snapshot->selected_configuration_id),
              orchestration->selected_configuration_id);
    copy_text(out_snapshot->selected_session_id,
              sizeof(out_snapshot->selected_session_id),
              orchestration->selected_session_id);
    out_snapshot->controller_state = controller_state(orchestration);
    out_snapshot->configuration_count = service.launch_configuration_count;
    out_snapshot->session_count = service.session_count;
    out_snapshot->breakpoint_result_count =
        umi_debug_breakpoint_query_count(orchestration->breakpoints);
    out_snapshot->watch_result_count =
        umi_debug_watch_query_count(orchestration->watches);
    out_snapshot->console_result_count =
        umi_debug_console_query_count(orchestration->console);
    out_snapshot->revision = orchestration->revision;
    out_snapshot->configuration_ready =
        umi_debug_configuration_resolver_result(orchestration->resolver,
                                                 &configuration) ==
            UMI_STATUS_OK && configuration.ready;
    out_snapshot->can_start = umi_debug_orchestration_command_enabled(
        orchestration, UMI_DEBUG_COMMAND_START);
    out_snapshot->can_continue = umi_debug_orchestration_command_enabled(
        orchestration, UMI_DEBUG_COMMAND_CONTINUE);
    out_snapshot->can_pause = umi_debug_orchestration_command_enabled(
        orchestration, UMI_DEBUG_COMMAND_PAUSE);
    out_snapshot->can_step = umi_debug_orchestration_command_enabled(
        orchestration, UMI_DEBUG_COMMAND_STEP_OVER);
    out_snapshot->can_stop = umi_debug_orchestration_command_enabled(
        orchestration, UMI_DEBUG_COMMAND_STOP);
    return UMI_STATUS_OK;
}

UmiDebugConfigurationResolver *umi_debug_orchestration_resolver(
    UmiDebugOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->resolver : NULL;
}

UmiDebugBreakpointQuery *umi_debug_orchestration_breakpoints(
    UmiDebugOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->breakpoints : NULL;
}

UmiDebugWatchQuery *umi_debug_orchestration_watches(
    UmiDebugOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->watches : NULL;
}

UmiDebugConsoleQuery *umi_debug_orchestration_console(
    UmiDebugOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->console : NULL;
}
