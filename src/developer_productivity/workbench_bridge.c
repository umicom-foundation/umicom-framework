/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/workbench_bridge.c
 *
 * PURPOSE:
 *   Implement command bindings that turn Source Control, Terminal and Problems
 *   menu/toolbar entries into real Framework-backed operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/workbench_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProductivityWorkbenchBridge {
    UmiDeveloperWorkbench *workbench;
    UmiDeveloperSourceControl *source_control;
    UmiDeveloperTerminalWorkspace *terminal;
    UmiDeveloperDiagnosticPipeline *diagnostics;
    UmiDeveloperNavigationService *navigation;
    UmiDeveloperOutputChannels *output;
    uint64_t terminal_sequence;
    size_t problem_cursor;
    int has_problem_cursor;
};

static int source_control_available(void *user_data, const char *argument)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    (void)argument;
    return bridge != NULL && bridge->source_control != NULL;
}

static int source_control_argument_available(
    void *user_data,
    const char *argument)
{
    return source_control_available(user_data, argument) &&
        argument != NULL && argument[0] != '\0';
}

static UmiStatus source_control_refresh(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    UmiDeveloperSourceControlSnapshot snapshot;
    UmiStatus status;

    (void)argument;

    status = umi_developer_source_control_refresh(bridge->source_control);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_snapshot(
        bridge->source_control, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s: %zu changes, %zu staged",
            snapshot.branch.name[0] != '\0'
                ? snapshot.branch.name
                : "(detached)",
            snapshot.change_count,
            snapshot.staged_count);
    }

    return UMI_STATUS_OK;
}

static UmiStatus source_control_stage_all(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;
    status = umi_developer_source_control_stage_all(bridge->source_control);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "All changes staged."
                : "Unable to stage all changes.");
    }

    return status;
}

static UmiStatus source_control_commit(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    char commit_id[UMI_VCS_ID_CAPACITY];
    UmiStatus status;

    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_source_control_commit(
        bridge->source_control,
        argument,
        commit_id,
        sizeof(commit_id));

    if (status == UMI_STATUS_OK &&
        out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "Committed %s",
            commit_id);
    }

    return status;
}

static UmiStatus source_control_pull(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;
    status = umi_developer_source_control_pull(bridge->source_control);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
            status == UMI_STATUS_OK ? "Pull completed." : "Pull failed.");
    }
    return status;
}

static UmiStatus source_control_push(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    UmiStatus status;

    (void)argument;
    status = umi_developer_source_control_push(bridge->source_control);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
            status == UMI_STATUS_OK ? "Push completed." : "Push failed.");
    }
    return status;
}

static UmiStatus source_control_diff(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    char diff[UMI_DEVELOPER_PRODUCTIVITY_OUTPUT_CAPACITY];
    UmiStatus status;

    status = umi_developer_source_control_diff(
        bridge->source_control,
        argument,
        0,
        diff,
        sizeof(diff));
    if (status != UMI_STATUS_OK) return status;

    (void)umi_developer_output_channel_append(
        bridge->output,
        "source-control",
        "Source Control",
        diff);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "Loaded diff for %s into Source Control output.",
            argument);
    }

    return UMI_STATUS_OK;
}

static int terminal_available(void *user_data, const char *argument)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    (void)argument;
    return bridge != NULL && bridge->terminal != NULL;
}

static int terminal_has_active(void *user_data, const char *argument)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    (void)argument;
    return bridge != NULL && bridge->terminal != NULL &&
        umi_developer_terminal_workspace_active(bridge->terminal) != NULL;
}

static UmiStatus terminal_new(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    char session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    const char *working_directory =
        argument != NULL && argument[0] != '\0' ? argument : ".";
    UmiStatus status;

    bridge->terminal_sequence += 1U;
    (void)snprintf(
        session_id,
        sizeof(session_id),
        "terminal.%llu",
        (unsigned long long)bridge->terminal_sequence);

    status = umi_developer_terminal_workspace_open(
        bridge->terminal,
        session_id,
        "Terminal",
        working_directory);

    if (status == UMI_STATUS_OK &&
        out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "Opened %s",
            session_id);
    }

    return status;
}

static UmiStatus terminal_kill(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    UmiDeveloperTerminalWorkspaceSnapshot snapshot;
    UmiStatus status;

    (void)argument;

    status = umi_developer_terminal_workspace_snapshot(
        bridge->terminal, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.active_session_id[0] == '\0') return UMI_STATUS_NOT_FOUND;

    status = umi_developer_terminal_workspace_close(
        bridge->terminal,
        snapshot.active_session_id);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            status == UMI_STATUS_OK
                ? "Terminal closed."
                : "Unable to close terminal.");
    }

    return status;
}

static int diagnostics_available(void *user_data, const char *argument)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;
    (void)argument;
    return bridge != NULL && bridge->diagnostics != NULL;
}

static UmiStatus problem_navigate(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    int forward,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProblemStore *store;
    UmiDeveloperProblem problem;
    size_t index = 0U;
    UmiStatus status;

    store = umi_developer_diagnostic_pipeline_problems(
        bridge->diagnostics);

    if (forward) {
        const size_t after =
            bridge->has_problem_cursor
                ? bridge->problem_cursor
                : (size_t)-1;
        status = umi_developer_problem_next(
            store,
            after,
            UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING,
            &index,
            &problem);
    } else {
        const size_t before =
            bridge->has_problem_cursor
                ? bridge->problem_cursor
                : umi_developer_problem_store_count(store);
        status = umi_developer_problem_previous(
            store,
            before,
            UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING,
            &index,
            &problem);
    }

    if (status != UMI_STATUS_OK) return status;

    bridge->problem_cursor = index;
    bridge->has_problem_cursor = 1;

    if (problem.location.uri[0] != '\0') {
        (void)umi_developer_navigation_service_visit(
            bridge->navigation,
            &problem.location);
    }

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s:%zu:%zu: %s",
            problem.location.uri,
            problem.location.line,
            problem.location.column,
            problem.message);
    }

    return UMI_STATUS_OK;
}

static UmiStatus problem_next(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    (void)argument;
    return problem_navigate(
        (UmiDeveloperProductivityWorkbenchBridge *)user_data,
        1,
        out_message,
        message_capacity);
}

static UmiStatus problem_previous(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    (void)argument;
    return problem_navigate(
        (UmiDeveloperProductivityWorkbenchBridge *)user_data,
        0,
        out_message,
        message_capacity);
}

static UmiStatus output_clear(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;

    (void)argument;
    umi_developer_output_channels_clear_all(bridge->output);

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", "Output cleared.");
    }

    return UMI_STATUS_OK;
}

static UmiStatus problems_clear(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge =
        (UmiDeveloperProductivityWorkbenchBridge *)user_data;

    (void)argument;
    umi_developer_problem_store_clear(
        umi_developer_diagnostic_pipeline_problems(bridge->diagnostics));
    bridge->has_problem_cursor = 0;

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", "Problems cleared.");
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_productivity_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperDiagnosticPipeline *diagnostics,
    UmiDeveloperNavigationService *navigation,
    UmiDeveloperOutputChannels *output,
    UmiDeveloperProductivityWorkbenchBridge **out_bridge)
{
    UmiDeveloperProductivityWorkbenchBridge *bridge;

    if (workbench == NULL || diagnostics == NULL ||
        navigation == NULL || output == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;
    bridge = (UmiDeveloperProductivityWorkbenchBridge *)calloc(
        1U, sizeof(*bridge));
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->workbench = workbench;
    bridge->diagnostics = diagnostics;
    bridge->navigation = navigation;
    bridge->output = output;

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

void umi_developer_productivity_workbench_bridge_destroy(
    UmiDeveloperProductivityWorkbenchBridge *bridge)
{
    free(bridge);
}

UmiStatus umi_developer_productivity_workbench_bridge_set_source_control(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperSourceControl *source_control)
{
    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bridge->source_control = source_control;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_productivity_workbench_bridge_set_terminal(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperTerminalWorkspace *terminal)
{
    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bridge->terminal = terminal;
    return UMI_STATUS_OK;
}

static UmiStatus bind(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled)
{
    return umi_developer_workbench_bind_action(
        bridge->workbench,
        command_id,
        handler,
        enabled,
        bridge);
}

UmiStatus umi_developer_productivity_workbench_bridge_bind(
    UmiDeveloperProductivityWorkbenchBridge *bridge)
{
    UmiStatus status;

    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = bind(bridge, "source-control.refresh",
                  source_control_refresh, source_control_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "source-control.stage-all",
                  source_control_stage_all, source_control_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "source-control.commit",
                  source_control_commit, source_control_argument_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "source-control.pull",
                  source_control_pull, source_control_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "source-control.push",
                  source_control_push, source_control_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "source-control.diff",
                  source_control_diff, source_control_argument_available);
    if (status != UMI_STATUS_OK) return status;

    status = bind(bridge, "terminal.new",
                  terminal_new, terminal_available);
    if (status != UMI_STATUS_OK) return status;
    status = bind(bridge, "terminal.kill",
                  terminal_kill, terminal_has_active);
    if (status != UMI_STATUS_OK) return status;

    status = bind(
        bridge, "navigate.problem.next", problem_next, diagnostics_available);
    if (status != UMI_STATUS_OK) return status;

    status = bind(
        bridge, "navigate.problem.previous",
        problem_previous,
        diagnostics_available);
    if (status != UMI_STATUS_OK) return status;

    status = bind(
        bridge, "output.clear", output_clear, diagnostics_available);
    if (status != UMI_STATUS_OK) return status;

    return bind(
        bridge, "problems.clear", problems_clear, diagnostics_available);
}
