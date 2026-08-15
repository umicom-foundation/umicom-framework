/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/workspace.c
 *
 * PURPOSE:
 *   Implement a professional, toolkit-neutral testing workspace over the
 *   canonical Framework test-platform registries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/workspace.h"

#include <stdlib.h>
#include <string.h>

struct UmiTestWorkspace {
    UmiTestPlatformService *service;
    UmiTestPlatformFilter filter;
    UmiTestWorkspaceRunMode run_mode;
    char selected_item_id[128];
    char selected_session_id[128];
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus selection(const UmiTestWorkspace *workspace,
                           UmiTestPlatformSelection *out_selection)
{
    if (workspace == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_test_platform_filter_select(
        umi_test_platform_service_item(workspace->service),
        umi_test_platform_service_result(workspace->service),
        &workspace->filter, out_selection);
}

static int selection_contains(const UmiTestPlatformSelection *items,
                              const char *item_id)
{
    size_t index;

    if (items == NULL || item_id == NULL || item_id[0] == '\0') return 0;
    for (index = 0U; index < items->count; ++index) {
        if (strcmp(items->item_ids[index], item_id) == 0) return 1;
    }
    return 0;
}

static UmiStatus reconcile_selection(UmiTestWorkspace *workspace)
{
    UmiTestPlatformSelection visible;
    UmiTestPlatformRunSessionSnapshot session;
    UmiStatus status;
    size_t count;

    status = selection(workspace, &visible);
    if (status != UMI_STATUS_OK) return status;
    if (!selection_contains(&visible, workspace->selected_item_id)) {
        workspace->selected_item_id[0] = '\0';
    }
    if (workspace->selected_item_id[0] == '\0' && visible.count > 0U) {
        status = copy_text(workspace->selected_item_id,
                           sizeof(workspace->selected_item_id),
                           visible.item_ids[0]);
        if (status != UMI_STATUS_OK) return status;
    }

    count = umi_test_platform_run_session_registry_count(
        umi_test_platform_service_run_session(workspace->service));
    if (workspace->selected_session_id[0] != '\0' &&
        umi_test_platform_run_session_registry_find(
            umi_test_platform_service_run_session(workspace->service),
            workspace->selected_session_id, &session) != UMI_STATUS_OK) {
        workspace->selected_session_id[0] = '\0';
    }
    if (workspace->selected_session_id[0] == '\0' && count > 0U &&
        umi_test_platform_run_session_registry_at(
            umi_test_platform_service_run_session(workspace->service),
            count - 1U, &session) == UMI_STATUS_OK) {
        status = copy_text(workspace->selected_session_id,
                           sizeof(workspace->selected_session_id), session.id);
    }
    return status;
}

UmiStatus umi_test_workspace_create(UmiTestPlatformService *service,
                                    UmiTestWorkspace **out_workspace)
{
    UmiTestWorkspace *workspace;

    if (service == NULL || out_workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workspace = NULL;
    workspace = (UmiTestWorkspace *)calloc(1U, sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->service = service;
    workspace->run_mode = UMI_TEST_WORKSPACE_RUN;
    workspace->revision = 1U;
    umi_test_platform_filter_init(&workspace->filter);
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

void umi_test_workspace_destroy(UmiTestWorkspace *workspace)
{
    free(workspace);
}

UmiStatus umi_test_workspace_refresh(UmiTestWorkspace *workspace)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = reconcile_selection(workspace);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

static void record_outcome(UmiTestWorkspaceSnapshot *snapshot, int outcome)
{
    switch ((UmiTestPlatformOutcome)outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_PASSED:
            snapshot->passed_count += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_FAILED:
            snapshot->failed_count += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED:
            snapshot->skipped_count += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED:
            snapshot->cancelled_count += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT:
            snapshot->timed_out_count += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN:
        default:
            snapshot->not_run_count += 1U;
            break;
    }
}

UmiStatus umi_test_workspace_snapshot(
    UmiTestWorkspace *workspace,
    UmiTestWorkspaceSnapshot *out_snapshot)
{
    UmiTestPlatformSelection visible;
    UmiTestPlatformResultSnapshot result;
    UmiTestPlatformCoverageSnapshot coverage;
    UmiStatus status;
    size_t index;

    if (workspace == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = reconcile_selection(workspace);
    if (status != UMI_STATUS_OK) return status;
    status = selection(workspace, &visible);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TEST_WORKSPACE_API_VERSION;
    out_snapshot->revision = workspace->revision;
    status = umi_test_platform_service_snapshot(workspace->service,
                                                &out_snapshot->service);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->filter = workspace->filter;
    out_snapshot->run_mode = workspace->run_mode;
    (void)copy_text(out_snapshot->run_mode_label,
                    sizeof(out_snapshot->run_mode_label),
                    umi_test_workspace_run_mode_text(workspace->run_mode));
    (void)copy_text(out_snapshot->selected_item_id,
                    sizeof(out_snapshot->selected_item_id),
                    workspace->selected_item_id);
    (void)copy_text(out_snapshot->selected_session_id,
                    sizeof(out_snapshot->selected_session_id),
                    workspace->selected_session_id);
    out_snapshot->visible_item_count = visible.count;

    for (index = 0U; index < visible.count; ++index) {
        if (umi_test_platform_history_latest(
                umi_test_platform_service_result(workspace->service),
                visible.item_ids[index], &result) == UMI_STATUS_OK) {
            record_outcome(out_snapshot, result.outcome);
        } else {
            record_outcome(out_snapshot, UMI_TEST_PLATFORM_OUTCOME_NOT_RUN);
        }
    }
    for (index = 0U; index < out_snapshot->service.result_count; ++index) {
        if (umi_test_platform_result_registry_at(
                umi_test_platform_service_result(workspace->service), index,
                &result) == UMI_STATUS_OK &&
            result.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED) {
            out_snapshot->failure_result_count += 1U;
        }
    }
    for (index = 0U; index < out_snapshot->service.coverage_count; ++index) {
        if (umi_test_platform_coverage_registry_at(
                umi_test_platform_service_coverage(workspace->service), index,
                &coverage) != UMI_STATUS_OK) {
            continue;
        }
        out_snapshot->coverage_lines_total += coverage.lines_total;
        out_snapshot->coverage_lines_covered += coverage.lines_covered;
        out_snapshot->coverage_branches_total += coverage.branches_total;
        out_snapshot->coverage_branches_covered += coverage.branches_covered;
    }
    if (out_snapshot->coverage_lines_total > 0U) {
        out_snapshot->line_coverage_basis_points = (uint32_t)(
            (out_snapshot->coverage_lines_covered * 10000U) /
            out_snapshot->coverage_lines_total);
    }
    if (out_snapshot->coverage_branches_total > 0U) {
        out_snapshot->branch_coverage_basis_points = (uint32_t)(
            (out_snapshot->coverage_branches_covered * 10000U) /
            out_snapshot->coverage_branches_total);
    }

    out_snapshot->can_discover = !out_snapshot->service.operation_running;
    out_snapshot->can_run_all = visible.count > 0U &&
                                !out_snapshot->service.operation_running;
    out_snapshot->can_run_selected =
        workspace->selected_item_id[0] != '\0' &&
        !out_snapshot->service.operation_running;
    out_snapshot->can_debug_selected = out_snapshot->can_run_selected;
    out_snapshot->can_run_with_coverage = out_snapshot->can_run_all;
    out_snapshot->can_rerun_failed = out_snapshot->failed_count > 0U &&
                                     !out_snapshot->service.operation_running;
    out_snapshot->can_stop = out_snapshot->service.operation_running &&
                             !out_snapshot->service.stop_requested;
    out_snapshot->can_clear_results = out_snapshot->service.result_count > 0U &&
                                      !out_snapshot->service.operation_running;
    out_snapshot->can_clear_output = out_snapshot->service.output_count > 0U &&
                                     !out_snapshot->service.operation_running;
    out_snapshot->can_clear_coverage =
        out_snapshot->service.coverage_count > 0U &&
        !out_snapshot->service.operation_running;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_workspace_set_filter(
    UmiTestWorkspace *workspace, const char *text, const char *suite_id,
    const char *label, int outcome, int include_disabled, int failed_only)
{
    UmiStatus status;

    if (workspace == NULL || text == NULL || suite_id == NULL ||
        label == NULL || outcome < -1 ||
        outcome > UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(workspace->filter.text,
                       sizeof(workspace->filter.text), text);
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->filter.suite_id,
                           sizeof(workspace->filter.suite_id), suite_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->filter.label,
                           sizeof(workspace->filter.label), label);
    }
    if (status != UMI_STATUS_OK) return status;
    workspace->filter.outcome = outcome;
    workspace->filter.include_disabled = include_disabled != 0;
    workspace->filter.failed_only = failed_only != 0;
    workspace->revision += 1U;
    return reconcile_selection(workspace);
}

UmiStatus umi_test_workspace_select_item(UmiTestWorkspace *workspace,
                                         const char *item_id)
{
    UmiTestPlatformSelection visible;
    UmiStatus status;

    if (workspace == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = selection(workspace, &visible);
    if (status != UMI_STATUS_OK) return status;
    if (!selection_contains(&visible, item_id)) return UMI_STATUS_NOT_FOUND;
    status = copy_text(workspace->selected_item_id,
                       sizeof(workspace->selected_item_id), item_id);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

UmiStatus umi_test_workspace_select_session(UmiTestWorkspace *workspace,
                                            const char *session_id)
{
    UmiTestPlatformRunSessionSnapshot session;
    UmiStatus status;

    if (workspace == NULL || session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_test_platform_run_session_registry_find(
        umi_test_platform_service_run_session(workspace->service), session_id,
        &session);
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->selected_session_id,
                           sizeof(workspace->selected_session_id), session.id);
    }
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

UmiStatus umi_test_workspace_set_run_mode(UmiTestWorkspace *workspace,
                                          UmiTestWorkspaceRunMode mode)
{
    if (workspace == NULL || mode < UMI_TEST_WORKSPACE_RUN ||
        mode > UMI_TEST_WORKSPACE_COVERAGE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workspace->run_mode = mode;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_workspace_plan_all(UmiTestWorkspace *workspace,
                                      UmiTestPlatformOperationPlan *out_plan)
{
    if (workspace == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_platform_operation_plan_init(
        out_plan, UMI_TEST_PLATFORM_OPERATION_RUN_ALL);
    return umi_test_platform_operation_plan_all(
        out_plan, umi_test_platform_service_item(workspace->service),
        umi_test_platform_service_result(workspace->service),
        &workspace->filter);
}

UmiStatus umi_test_workspace_plan_selected(
    UmiTestWorkspace *workspace, UmiTestPlatformOperationPlan *out_plan)
{
    if (workspace == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (reconcile_selection(workspace) != UMI_STATUS_OK ||
        workspace->selected_item_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }
    umi_test_platform_operation_plan_init(
        out_plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    return umi_test_platform_operation_plan_add(
        out_plan, workspace->selected_item_id);
}

UmiStatus umi_test_workspace_plan_failed(
    UmiTestWorkspace *workspace, UmiTestPlatformOperationPlan *out_plan)
{
    if (workspace == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_platform_operation_plan_init(
        out_plan, UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED);
    return umi_test_platform_operation_plan_failed(
        out_plan, umi_test_platform_service_item(workspace->service),
        umi_test_platform_service_result(workspace->service));
}

UmiStatus umi_test_workspace_plan_repeat_selected(
    UmiTestWorkspace *workspace, uint32_t repeat_count, int stop_on_failure,
    UmiTestPlatformOperationPlan *out_plan)
{
    UmiStatus status;

    if (repeat_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_workspace_plan_selected(workspace, out_plan);
    if (status == UMI_STATUS_OK) {
        out_plan->kind = UMI_TEST_PLATFORM_OPERATION_REPEAT;
        out_plan->repeat_count = repeat_count;
        out_plan->stop_on_failure = stop_on_failure != 0;
    }
    return status;
}

UmiStatus umi_test_workspace_begin(
    UmiTestWorkspace *workspace, const UmiTestPlatformOperationPlan *plan)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_service_begin_operation(workspace->service, plan);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

UmiStatus umi_test_workspace_request_stop(UmiTestWorkspace *workspace)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_service_request_stop(workspace->service);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

void umi_test_workspace_finish(UmiTestWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_test_platform_service_finish_operation(workspace->service);
    workspace->revision += 1U;
}

void umi_test_workspace_clear_results(UmiTestWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_test_platform_result_registry_clear(
        umi_test_platform_service_result(workspace->service));
    workspace->revision += 1U;
}

void umi_test_workspace_clear_output(UmiTestWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_test_platform_output_registry_clear(
        umi_test_platform_service_output(workspace->service));
    workspace->revision += 1U;
}

void umi_test_workspace_clear_coverage(UmiTestWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_test_platform_coverage_registry_clear(
        umi_test_platform_service_coverage(workspace->service));
    workspace->revision += 1U;
}

UmiStatus umi_test_workspace_visible_item_at(
    UmiTestWorkspace *workspace, size_t visible_index,
    UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest_result)
{
    UmiTestPlatformSelection visible;
    UmiStatus status;

    if (workspace == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = selection(workspace, &visible);
    if (status != UMI_STATUS_OK) return status;
    if (visible_index >= visible.count) return UMI_STATUS_NOT_FOUND;
    status = umi_test_platform_item_registry_find(
        umi_test_platform_service_item(workspace->service),
        visible.item_ids[visible_index], out_item);
    if (status == UMI_STATUS_OK && out_latest_result != NULL) {
        (void)memset(out_latest_result, 0, sizeof(*out_latest_result));
        (void)umi_test_platform_history_latest(
            umi_test_platform_service_result(workspace->service), out_item->id,
            out_latest_result);
    }
    return status;
}

UmiStatus umi_test_workspace_result_at(
    UmiTestWorkspace *workspace, size_t newest_index,
    UmiTestPlatformResultSnapshot *out_result)
{
    size_t count;

    if (workspace == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_test_platform_result_registry_count(
        umi_test_platform_service_result(workspace->service));
    if (newest_index >= count) return UMI_STATUS_NOT_FOUND;
    return umi_test_platform_result_registry_at(
        umi_test_platform_service_result(workspace->service),
        count - newest_index - 1U, out_result);
}

UmiStatus umi_test_workspace_failure_at(
    UmiTestWorkspace *workspace, size_t newest_index,
    UmiTestPlatformResultSnapshot *out_result)
{
    UmiTestPlatformResultSnapshot candidate;
    size_t count;
    size_t index;
    size_t matched = 0U;

    if (workspace == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_test_platform_result_registry_count(
        umi_test_platform_service_result(workspace->service));
    for (index = 0U; index < count; ++index) {
        if (umi_test_workspace_result_at(workspace, index, &candidate) ==
                UMI_STATUS_OK &&
            candidate.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED) {
            if (matched == newest_index) {
                *out_result = candidate;
                return UMI_STATUS_OK;
            }
            matched += 1U;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_test_workspace_output_at(
    UmiTestWorkspace *workspace, size_t newest_index,
    UmiTestPlatformOutputSnapshot *out_output)
{
    size_t count;

    if (workspace == NULL || out_output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_test_platform_output_registry_count(
        umi_test_platform_service_output(workspace->service));
    if (newest_index >= count) return UMI_STATUS_NOT_FOUND;
    return umi_test_platform_output_registry_at(
        umi_test_platform_service_output(workspace->service),
        count - newest_index - 1U, out_output);
}

UmiStatus umi_test_workspace_coverage_at(
    UmiTestWorkspace *workspace, size_t index,
    UmiTestPlatformCoverageSnapshot *out_coverage)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_test_platform_coverage_registry_at(
        umi_test_platform_service_coverage(workspace->service), index,
        out_coverage);
}

UmiStatus umi_test_workspace_session_at(
    UmiTestWorkspace *workspace, size_t newest_index,
    UmiTestPlatformRunSessionSnapshot *out_session)
{
    size_t count;

    if (workspace == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_test_platform_run_session_registry_count(
        umi_test_platform_service_run_session(workspace->service));
    if (newest_index >= count) return UMI_STATUS_NOT_FOUND;
    return umi_test_platform_run_session_registry_at(
        umi_test_platform_service_run_session(workspace->service),
        count - newest_index - 1U, out_session);
}

const char *umi_test_workspace_run_mode_text(UmiTestWorkspaceRunMode mode)
{
    switch (mode) {
        case UMI_TEST_WORKSPACE_RUN: return "run";
        case UMI_TEST_WORKSPACE_DEBUG: return "debug";
        case UMI_TEST_WORKSPACE_COVERAGE: return "coverage";
        default: return "unknown";
    }
}
