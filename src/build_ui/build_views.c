/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build_ui/build_views.c
 *
 * PURPOSE:
 *   Project the professional build-and-task workspace into toolkit-neutral
 *   view models consumed by GTK4, Qt, Wt, headless tests and future adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build_ui/build_ui.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

static size_t visible_rows(size_t count)
{
    return count < UMI_BUILD_UI_VISIBLE_ROWS
        ? count : UMI_BUILD_UI_VISIBLE_ROWS;
}

static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value,
                                               text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_boolean(UmiUiViewModel *view, const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view, size_t index,
                            const char *action_id, const char *label,
                            const char *tooltip, int enabled)
{
    UmiUiCommandViewAction action = {0};

    (void)snprintf(action.action_id, sizeof(action.action_id), "%s",
                   action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = enabled != 0;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus create_view(const char *view_id, const char *view_kind,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;

    if (view_id == NULL || view_kind == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_view_model_create(view_id, "umicom.build-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "umicom.view-kind", view_kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus set_workspace_properties(
    UmiUiViewModel *view, const UmiBuildWorkspaceSnapshot *snapshot)
{
    UmiStatus status = set_integer(view, "build.revision",
                                   (int64_t)snapshot->revision);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "build.profile-id", snapshot->profile_id);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "build.directory",
                            snapshot->build_directory);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "build.configuration",
                            snapshot->configuration);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "build.selected-node",
                            snapshot->selected_node_id);
    if (status == UMI_STATUS_OK)
        status = set_integer(view, "build.selected-operation",
                             (int64_t)snapshot->selected_operation_id);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "build.selected-artifact",
                            snapshot->selected_artifact_id);
    return status;
}

UmiStatus umi_build_ui_dashboard_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "build-dashboard", "Build Dashboard",
        "Configure, build, test, run, install and monitor the active profile.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "build.source-directory",
                            snapshot.source_directory);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "build.generator", snapshot.generator);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "build.target", snapshot.build_target);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.progress-basis-points",
                             (int64_t)snapshot.graph.progress_basis_points);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.ready-count",
                             (int64_t)snapshot.graph.ready_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.running-count",
                             (int64_t)snapshot.graph.running_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.failed-count",
                             (int64_t)snapshot.graph.failed_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.history-count",
                             (int64_t)snapshot.history_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.artifact-count",
                             (int64_t)snapshot.artifact_count);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "build.has-latest-result",
                             snapshot.has_latest_result);
    if (status == UMI_STATUS_OK && snapshot.has_latest_result)
        status = set_string(*out_view, "build.latest-phase",
                            umi_build_phase_text(snapshot.latest_phase));
    if (status == UMI_STATUS_OK && snapshot.has_latest_result)
        status = set_string(*out_view, "build.latest-state",
                            umi_build_state_text(snapshot.latest_state));
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.build.configure", "Configure",
        "Configure the active CMake profile", snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.build.compile", "Build",
        "Compile the active workspace", snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.build.test", "Test",
        "Run CTest for the active build profile", snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.build.run", "Run",
        "Start the configured executable", snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.build.install", "Install",
        "Install into the configured local staging prefix",
        snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.build.clean", "Clean",
        "Clean the active build profile", snapshot.can_cancel == 0);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.build.run-all", "Run Build Plan",
        "Execute every ready node in the dependency-aware build plan",
        snapshot.can_execute_all);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 7U, "studio.action.build.cancel", "Stop",
        "Request cooperative cancellation of the active build operation",
        snapshot.can_cancel);
    return status;
}

UmiStatus umi_build_ui_graph_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "build-graph", "Build Graph",
        "Dependency order, incremental state, progress, retries and blockers.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "build.filter-text",
                            snapshot.filter_text);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "build.filter-state",
            umi_build_workspace_node_filter_text(snapshot.node_filter));
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.visible-node-count",
                             (int64_t)snapshot.visible_node_count);
    count = visible_rows(snapshot.visible_node_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiBuildGraphNodeSnapshot node;
        char key[64];
        char text[480];

        status = umi_build_workspace_visible_node_at(workspace, index, &node);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "build.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%s — %s — %s — %u.%02u%% — attempt %u/%u",
            strcmp(node.node_id, snapshot.selected_node_id) == 0 ? "* " : "",
            node.label, umi_build_phase_text(node.phase),
            umi_build_node_state_text(node.state),
            node.progress_basis_points / 100U,
            node.progress_basis_points % 100U,
            node.attempt_count, node.maximum_attempts);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.build.filter", "Filter…",
        "Filter graph nodes by text or lifecycle state", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.build.select-node", "Select Node…",
        "Select a graph node by its stable identifier", count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.build.run-next", "Run Next",
        "Execute the next ready graph node", snapshot.can_execute_next);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.build.run-all", "Run All",
        "Execute all ready nodes in dependency order",
        snapshot.can_execute_all);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.build.retry", "Retry Selected",
        "Retry the selected failed, cancelled, timed-out or blocked node",
        snapshot.can_retry_selected);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.build.invalidate", "Invalidate…",
        "Invalidate a node after its input revision changes",
        snapshot.has_selected_node && !snapshot.can_cancel);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.build.refresh", "Refresh",
        "Refresh dependency readiness and retained selections", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 7U, "studio.action.build.cancel", "Stop",
        "Request cooperative cancellation", snapshot.can_cancel);
    return status;
}

UmiStatus umi_build_ui_history_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    UmiBuildResult *result = NULL;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "build-history", "Build History",
        "Retained configure, build, test, run, clean and install operations.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    count = visible_rows(snapshot.history_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.history-count",
                             (int64_t)snapshot.history_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.row-count", (int64_t)count);
    if (status == UMI_STATUS_OK && count > 0U)
        status = umi_build_result_create(&result);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        char key[64];
        char text[480];

        status = umi_build_workspace_history_at(workspace, index, result);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "build.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s#%llu — %s — %s — %llums — exit %d — %zu diagnostics",
            result->operation_id == snapshot.selected_operation_id ? "* " : "",
            (unsigned long long)result->operation_id,
            umi_build_phase_text(result->phase),
            umi_build_state_text(result->state),
            (unsigned long long)result->duration_ms,
            result->exit_code, result->diagnostics.count);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.build.select-operation",
        "Select Operation…", "Select retained build evidence by operation ID",
        count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.build.clear-history", "Clear History",
        "Clear retained build results while preserving the graph",
        snapshot.can_clear_history);
    umi_build_result_destroy(result);
    return status;
}

UmiStatus umi_build_ui_output_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    UmiBuildResult *result = NULL;
    UmiStatus status = create_view(
        view_id, "build-output", "Build Output",
        "Command, transcript and diagnostics for the selected build operation.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = umi_build_result_create(&result);
    if (status == UMI_STATUS_OK)
        status = umi_build_workspace_selected_result(workspace, result);
    if (status == UMI_STATUS_NOT_FOUND) {
        status = set_boolean(*out_view, "build.has-output", 0);
    } else if (status == UMI_STATUS_OK) {
        char command[480];
        char output[480];
        (void)snprintf(command, sizeof(command), "%.470s", result->command);
        (void)snprintf(output, sizeof(output), "%.470s", result->output);
        status = set_boolean(*out_view, "build.has-output", 1);
        if (status == UMI_STATUS_OK)
            status = set_string(*out_view, "build.phase",
                                umi_build_phase_text(result->phase));
        if (status == UMI_STATUS_OK)
            status = set_string(*out_view, "build.state",
                                umi_build_state_text(result->state));
        if (status == UMI_STATUS_OK)
            status = set_string(*out_view, "build.command", command);
        if (status == UMI_STATUS_OK)
            status = set_string(*out_view, "build.output", output);
        if (status == UMI_STATUS_OK)
            status = set_integer(*out_view, "build.diagnostic-count",
                                 (int64_t)result->diagnostics.count);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.build.select-operation",
        "Select Operation…", "Choose the operation whose output is displayed",
        snapshot.history_count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.build.clear-history", "Clear Output",
        "Clear retained build history and output evidence",
        snapshot.can_clear_history);
    umi_build_result_destroy(result);
    return status;
}

UmiStatus umi_build_ui_artifacts_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "build-artifacts", "Build Artifacts",
        "Executables, libraries, packages and evidence produced by build nodes.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    count = visible_rows(snapshot.artifact_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.artifact-count",
                             (int64_t)snapshot.artifact_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "build.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiBuildArtifactSnapshot artifact;
        char key[64];
        char text[480];

        status = umi_build_workspace_artifact_at(workspace, index, &artifact);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "build.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s%.80s — %.60s — %.250s — %llu bytes",
            strcmp(artifact.artifact_id, snapshot.selected_artifact_id) == 0
                ? "* " : "",
            artifact.artifact_id, artifact.kind, artifact.path,
            (unsigned long long)artifact.size_bytes);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.build.select-artifact",
        "Select Artifact…", "Select an artifact by its stable identifier",
        count > 0U);
    return status;
}

UmiStatus umi_build_ui_tasks_view_create(
    const char *view_id, UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiBuildWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "build-tasks", "Build Tasks",
        "Worker capacity, queued work, completions, failures and cancellation.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.workers",
                             (int64_t)snapshot.tasks.worker_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.capacity",
                             (int64_t)snapshot.tasks.capacity);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.queued",
                             (int64_t)snapshot.tasks.queued);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.running",
                             (int64_t)snapshot.tasks.running);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.submitted",
                             (int64_t)snapshot.tasks.submitted);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.completed",
                             (int64_t)snapshot.tasks.completed);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.failed",
                             (int64_t)snapshot.tasks.failed);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "task.cancelled",
                             (int64_t)snapshot.tasks.cancelled);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.tasks.refresh", "Refresh",
        "Refresh shared build and task state", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.tasks.wait", "Wait for Idle",
        "Wait for queued background work to finish",
        snapshot.tasks.queued > 0U || snapshot.tasks.running > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.build.cancel", "Stop",
        "Request cancellation of active build work", snapshot.can_cancel);
    return status;
}
