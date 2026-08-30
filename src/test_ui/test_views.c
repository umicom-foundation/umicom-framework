/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_ui/test_views.c
 *
 * PURPOSE:
 *   Project the reusable professional testing workspace into view models that
 *   GTK4, Qt, Wt, headless automation and future adapters can render equally.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_ui/test_ui.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

static size_t visible_rows(size_t count)
{
    return count < UMI_TEST_UI_VISIBLE_ROWS
        ? count : UMI_TEST_UI_VISIBLE_ROWS;
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

    if (view_id == NULL || view_kind == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_view_model_create(view_id, "umicom.test-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "umicom.view-kind", view_kind);
    }
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "summary", summary);
    }
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus set_workspace_properties(
    UmiUiViewModel *view, const UmiTestWorkspaceSnapshot *snapshot)
{
    UmiStatus status = set_integer(view, "test.revision",
                                   (int64_t)snapshot->revision);
    if (status == UMI_STATUS_OK) {
        status = set_string(view, "test.run-mode", snapshot->run_mode_label);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(view, "test.selected-item",
                            snapshot->selected_item_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(view, "test.selected-session",
                            snapshot->selected_session_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_boolean(view, "test.running",
                             snapshot->service.operation_running);
    }
    return status;
}

static const char *outcome_marker(int outcome)
{
    switch ((UmiTestPlatformOutcome)outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: return "✓";
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: return "✗";
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: return "↷";
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: return "■";
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: return "◷";
        case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN:
        default: return "○";
    }
}

UmiStatus umi_test_ui_explorer_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-explorer", "Test Explorer",
        "Discover, filter, select, run, debug and repeat provider-neutral tests.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "test.filter-text", snapshot.filter.text);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "test.filter-label",
                            snapshot.filter.label);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.visible-count",
                             (int64_t)snapshot.visible_item_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.passed-count",
                             (int64_t)snapshot.passed_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.failed-count",
                             (int64_t)snapshot.failed_count);
    }
    count = visible_rows(snapshot.visible_item_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.row-count", (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformItemSnapshot item;
        UmiTestPlatformResultSnapshot result;
        char key[64];
        char text[1800];

        status = umi_test_workspace_visible_item_at(
            workspace, index, &item, &result);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s%s — %s%s%s",
                       outcome_marker(result.outcome),
                       strcmp(item.id, snapshot.selected_item_id) == 0
                           ? "● " : "",
                       item.name, item.framework,
                       item.source_uri[0] != '\0' ? " — " : "",
                       item.source_uri);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.discover", "Discover",
        "Discover tests from the active build directory",
        snapshot.can_discover);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.test.filter", "Filter…",
        "Filter by text, label or latest outcome", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.test.select", "Select Test…",
        "Select a test by its stable identifier", count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.test.run-all", "Run All",
        "Run every visible test", snapshot.can_run_all);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.test.run-selected", "Run Selected",
        "Run the selected test", snapshot.can_run_selected);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.test.debug-selected", "Debug Selected",
        "Run the selected test using the debug mode",
        snapshot.can_debug_selected);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.test.coverage", "Run with Coverage",
        "Run visible tests and collect coverage", snapshot.can_run_with_coverage);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 7U, "studio.action.test.rerun-failed", "Rerun Failed",
        "Run tests whose latest result failed", snapshot.can_rerun_failed);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 8U, "studio.action.test.repeat-selected", "Repeat…",
        "Repeat the selected test to diagnose intermittent failures",
        snapshot.can_run_selected);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 9U, "studio.action.test.stop", "Stop",
        "Request cooperative cancellation of the active test run",
        snapshot.can_stop);
    return status;
}

UmiStatus umi_test_ui_results_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-results", "Test Results",
        "Retained outcomes and durations, newest first.", out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.service.result_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.result-count",
                             (int64_t)snapshot.service.result_count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformResultSnapshot result;
        char key[64];
        char text[1600];

        status = umi_test_workspace_result_at(workspace, index, &result);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.result.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s — %s (%.2f ms)",
                       outcome_marker(result.outcome), result.item_id,
                       result.message, result.duration_ms);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.rerun-failed", "Rerun Failed",
        "Run tests whose latest result failed", snapshot.can_rerun_failed);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.test.clear-results", "Clear Results",
        "Clear retained test-result records", snapshot.can_clear_results);
    return status;
}

UmiStatus umi_test_ui_failures_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-failures", "Test Failures",
        "Failure messages and diagnostic details from retained test results.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.failure_result_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.failure-count",
                             (int64_t)snapshot.failure_result_count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformResultSnapshot result;
        char key[64];
        char text[3328];

        status = umi_test_workspace_failure_at(workspace, index, &result);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.failure.%zu", index);
        (void)snprintf(text, sizeof(text), "✗ %s — %s%s%s",
                       result.item_id, result.message,
                       result.failure_details[0] != '\0' ? "\n" : "",
                       result.failure_details);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.rerun-failed", "Rerun Failed",
        "Run tests whose latest result failed", snapshot.can_rerun_failed);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.test.clear-results", "Clear Results",
        "Clear retained failure evidence", snapshot.can_clear_results);
    return status;
}

UmiStatus umi_test_ui_output_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-output", "Test Output",
        "Standard output, standard error and combined test streams.", out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.service.output_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.output-count",
                             (int64_t)snapshot.service.output_count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformOutputSnapshot output;
        char key[64];
        char text[2304];

        status = umi_test_workspace_output_at(workspace, index, &output);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.output.%zu", index);
        (void)snprintf(text, sizeof(text), "[%s] %s — %s", output.stream,
                       output.item_id, output.text);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.clear-output", "Clear Output",
        "Clear retained test output", snapshot.can_clear_output);
    return status;
}

UmiStatus umi_test_ui_coverage_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-coverage", "Test Coverage",
        "Line and branch coverage summaries linked to source URIs.", out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.line-coverage-basis-points",
                             snapshot.line_coverage_basis_points);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.branch-coverage-basis-points",
                             snapshot.branch_coverage_basis_points);
    }
    count = visible_rows(snapshot.service.coverage_count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformCoverageSnapshot coverage;
        char key[64];
        char text[1400];

        status = umi_test_workspace_coverage_at(workspace, index, &coverage);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.coverage.%zu", index);
        (void)snprintf(text, sizeof(text),
                       "%s — lines %llu/%llu; branches %llu/%llu",
                       coverage.uri,
                       (unsigned long long)coverage.lines_covered,
                       (unsigned long long)coverage.lines_total,
                       (unsigned long long)coverage.branches_covered,
                       (unsigned long long)coverage.branches_total);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.coverage", "Run with Coverage",
        "Run visible tests and collect coverage", snapshot.can_run_with_coverage);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.test.clear-coverage", "Clear Coverage",
        "Clear retained coverage summaries", snapshot.can_clear_coverage);
    return status;
}

UmiStatus umi_test_ui_runs_view_create(
    const char *view_id, UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTestWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "test-runs", "Test Runs",
        "Retained run sessions with aggregate outcomes and stable selection.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = umi_test_workspace_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.service.run_session_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "test.run-count",
                             (int64_t)snapshot.service.run_session_count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTestPlatformRunSessionSnapshot session;
        char key[64];
        char text[768];

        status = umi_test_workspace_session_at(workspace, index, &session);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "test.run.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%s — %zu total; %zu passed; "
                       "%zu failed; %zu skipped",
                       strcmp(session.id, snapshot.selected_session_id) == 0
                           ? "● " : "○ ",
                       session.id, session.total, session.passed,
                       session.failed, session.skipped);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.test.run-all", "Run All",
        "Start a new run for every visible test", snapshot.can_run_all);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.test.stop", "Stop",
        "Request cooperative cancellation of the active test run",
        snapshot.can_stop);
    return status;
}
