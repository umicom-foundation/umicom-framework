/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_catalogue.c
 * PURPOSE: Verify catalogue inspection and single-test workflows beyond execution-selection capacity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/workspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"large-snapshot", "large-select", "large-visible", "large-plan-selected", "large-plan-all", "filter-membership", "filter-refusal", "selected-removed", "empty", "timeout-only", "run-capacity", "result-outcomes", "missing-selection", "inspect-selected", "inspect-unknown", "inspect-invalid", "inspect-row", "inspect-out-of-range", "inspect-latest-sequence", "inspect-latest-revision", "inspect-result-metadata", "rerun-filtered", "rerun-passed", "rerun-timeout"};
    int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(cases[i], argv[1]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    UmiTestPlatformService *service = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestPlatformOperationPlan *plan = calloc(1U, sizeof *plan);
    UmiTestWorkspaceSnapshot snapshot, before;
    UmiTestPlatformItemSnapshot item = {0}, row;
    UmiTestPlatformResultSnapshot result = {0}, latest;
    CHECK(plan != NULL);
    CHECK(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    UmiTestPlatformItemRegistry *items = umi_test_platform_service_item(service);
    UmiTestPlatformResultRegistry *results = umi_test_platform_service_result(service);
    const size_t count = strncmp(mode, "large-", 6U) == 0 ? 26032U :
        strcmp(mode, "run-capacity") == 0 ? 1025U : strcmp(mode, "empty") == 0 ? 0U : 6U;
    for (size_t i = 0U; i < count; ++i) {
        snprintf(item.id, sizeof item.id, "test.%05zu", i);
        snprintf(item.name, sizeof item.name, "Test %05zu", i);
        strcpy(item.kind, "test"); item.enabled = 1;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    CHECK(umi_test_workspace_refresh(workspace) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.visible_item_count == count && snapshot.not_run_count == count);
    if (count > 1024U) CHECK(!snapshot.can_run_all && snapshot.can_run_selected);
    if (strcmp(mode, "large-select") == 0 || strcmp(mode, "large-plan-selected") == 0) {
        CHECK(umi_test_workspace_select_item(workspace, "test.26031") == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.selected_item_id, "test.26031") == 0 && snapshot.can_run_selected);
        CHECK(umi_test_workspace_plan_selected(workspace, plan) == UMI_STATUS_OK);
        CHECK(plan->selection.count == 1U && strcmp(plan->selection.item_ids[0], "test.26031") == 0);
    } else if (strcmp(mode, "large-visible") == 0) {
        CHECK(umi_test_workspace_visible_item_at(workspace, count - 1U, &row, &latest) == UMI_STATUS_OK);
        CHECK(strcmp(row.id, "test.26031") == 0 && latest.id[0] == 0);
    } else if (strcmp(mode, "large-plan-all") == 0 || strcmp(mode, "run-capacity") == 0) {
        CHECK(umi_test_workspace_plan_all(workspace, plan) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(plan->selection.count == 0U);
    } else if (strcmp(mode, "filter-membership") == 0) {
        CHECK(umi_test_workspace_set_filter(workspace, "00005", "", "", -1, 0, 0) == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.visible_item_count == 1U && strcmp(snapshot.selected_item_id, "test.00005") == 0);
        CHECK(umi_test_workspace_select_item(workspace, "test.00000") == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "filter-refusal") == 0) {
        CHECK(umi_test_workspace_select_item(workspace, "test.00005") == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
        char long_suite[129]; memset(long_suite, 'x', sizeof long_suite - 1U); long_suite[128] = 0;
        CHECK(umi_test_workspace_set_filter(workspace, "00000", long_suite, "", -1, 0, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(memcmp(&before.filter, &snapshot.filter, sizeof snapshot.filter) == 0);
        CHECK(snapshot.revision == before.revision && strcmp(snapshot.selected_item_id, "test.00005") == 0);
    } else if (strcmp(mode, "selected-removed") == 0) {
        CHECK(umi_test_workspace_select_item(workspace, "test.00005") == UMI_STATUS_OK);
        CHECK(umi_test_platform_item_registry_remove(items, "test.00005") == UMI_STATUS_OK);
        CHECK(umi_test_workspace_refresh(workspace) == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.selected_item_id, "test.00000") == 0);
    } else if (strcmp(mode, "missing-selection") == 0) {
        CHECK(umi_test_workspace_select_item(workspace, "missing") == UMI_STATUS_NOT_FOUND);
        CHECK(umi_test_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
        CHECK(before.revision == snapshot.revision && strcmp(before.selected_item_id, snapshot.selected_item_id) == 0);
    } else if (strcmp(mode, "empty") == 0) {
        CHECK(!snapshot.can_run_all && !snapshot.can_run_selected && snapshot.selected_item_id[0] == 0);
        CHECK(umi_test_workspace_visible_item_at(workspace, 0U, &row, NULL) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "result-outcomes") == 0 || strcmp(mode, "timeout-only") == 0) {
        const int outcomes[] = {UMI_TEST_PLATFORM_OUTCOME_NOT_RUN, UMI_TEST_PLATFORM_OUTCOME_PASSED,
            UMI_TEST_PLATFORM_OUTCOME_FAILED, UMI_TEST_PLATFORM_OUTCOME_SKIPPED,
            UMI_TEST_PLATFORM_OUTCOME_CANCELLED, UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT};
        for (size_t i = 0U; i < 6U; ++i) {
            snprintf(result.id, sizeof result.id, "result.%zu", i);
            snprintf(result.item_id, sizeof result.item_id, "test.%05zu", i);
            result.outcome = outcomes[i]; result.sequence = i + 1U;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
        if (strcmp(mode, "timeout-only") == 0)
            CHECK(umi_test_workspace_set_filter(workspace, "", "", "", UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT, 0, 1) == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.timed_out_count == 1U && snapshot.can_rerun_failed);
        if (strcmp(mode, "timeout-only") == 0) CHECK(snapshot.visible_item_count == 1U && snapshot.failed_count == 0U);
        else CHECK(snapshot.visible_item_count == 6U && snapshot.failed_count == 1U && snapshot.passed_count == 1U &&
            snapshot.cancelled_count == 1U && snapshot.skipped_count == 1U && snapshot.not_run_count == 1U);
    } else if (strncmp(mode, "inspect-latest-", 15U) == 0 || strcmp(mode, "inspect-result-metadata") == 0) {
        strcpy(result.id, "older"); strcpy(result.item_id, "test.00005");
        result.sequence = 20U; result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
        CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        strcpy(result.id, "newer");
        result.sequence = strcmp(mode, "inspect-latest-sequence") == 0 ? 19U : 20U;
        result.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
        strcpy(result.message, "Deadline expired");
        if (strcmp(mode, "inspect-result-metadata") == 0) {
            strcpy(result.session_id, "session.deadline");
            strcpy(result.attachment_id, "output.deadline");
            strcpy(result.failure_details, "Process did not finish before its deadline");
            result.duration_ms = 2500.5; result.exit_code = 124; result.flaky = 1;
        }
        CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        CHECK(umi_test_workspace_visible_item_at(workspace, 5U, &row, &latest) == UMI_STATUS_OK);
        const int earlier_sequence = strcmp(mode, "inspect-latest-sequence") == 0;
        CHECK(strcmp(latest.id, earlier_sequence ? "older" : "newer") == 0);
        CHECK(latest.outcome == (earlier_sequence ? UMI_TEST_PLATFORM_OUTCOME_FAILED : UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT));
        if (!earlier_sequence) CHECK(strcmp(latest.message, "Deadline expired") == 0);
        if (strcmp(mode, "inspect-result-metadata") == 0) {
            CHECK(umi_test_platform_result_registry_find(results, "newer", &result) == UMI_STATUS_OK);
            CHECK(memcmp(&latest, &result, sizeof latest) == 0);
        }
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.failed_count == (size_t)earlier_sequence && snapshot.timed_out_count == (size_t)!earlier_sequence);
        CHECK(snapshot.not_run_count == count - 1U);
    } else if (strncmp(mode, "rerun-", 6U) == 0) {
        for (size_t i = 0U; i < 3U; ++i) {
            snprintf(result.id, sizeof result.id, "run.%zu", i);
            snprintf(result.item_id, sizeof result.item_id, "test.%05zu", i);
            result.sequence = 1U;
            result.outcome = i == 0U ? UMI_TEST_PLATFORM_OUTCOME_FAILED :
                i == 1U ? UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT : UMI_TEST_PLATFORM_OUTCOME_PASSED;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
        const int passed = strcmp(mode, "rerun-passed") == 0;
        const int timeout = strcmp(mode, "rerun-timeout") == 0;
        const char *query = passed ? "00002" : timeout ? "00001" : "00000";
        CHECK(umi_test_workspace_set_filter(workspace, query, "", "", -1, 0, 0) == UMI_STATUS_OK);
        CHECK(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.visible_item_count == 1U && snapshot.can_rerun_failed == !passed);
        CHECK(umi_test_workspace_plan_failed(workspace, plan) == UMI_STATUS_OK);
        CHECK(plan->selection.count == (passed ? 0U : 1U));
        if (!passed) {
            CHECK(plan->kind == UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED);
            CHECK(strcmp(plan->selection.item_ids[0], timeout ? "test.00001" : "test.00000") == 0);
        }
    } else if (strncmp(mode, "inspect-", 8U) == 0) {
        UmiTestPlatformFilter filter; umi_test_platform_filter_init(&filter);
        UmiTestPlatformFilterSummary summary = {0}, saved;
        summary.matched_count = 91U; saved = summary;
        if (strcmp(mode, "inspect-invalid") == 0) {
            memset(filter.label, 'x', sizeof filter.label);
            CHECK(UmiTestPlatformFilterInspect(items, results, &filter, NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&summary, &saved, sizeof summary) == 0);
        } else if (strcmp(mode, "inspect-row") == 0 || strcmp(mode, "inspect-out-of-range") == 0) {
            memset(&row, 0, sizeof row); strcpy(row.id, "retained");
            memset(&latest, 0, sizeof latest); strcpy(latest.id, "retained-result");
            const int absent = strcmp(mode, "inspect-out-of-range") == 0;
            CHECK(UmiTestPlatformFilterItemAt(items, results, &filter, absent ? count : 5U, &row, &latest) ==
                (absent ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
            CHECK(strcmp(row.id, absent ? "retained" : "test.00005") == 0);
            if (absent) CHECK(strcmp(latest.id, "retained-result") == 0);
            else CHECK(latest.id[0] == 0);
        } else {
            const int absent = strcmp(mode, "inspect-unknown") == 0;
            CHECK(UmiTestPlatformFilterInspect(items, results, &filter, absent ? "missing" : "test.00005", &summary) == UMI_STATUS_OK);
            CHECK(summary.matched_count == count && summary.selected_matches == !absent);
            CHECK(strcmp(summary.first_item_id, "test.00000") == 0);
        }
    }
done:
    free(plan); umi_test_workspace_destroy(workspace); umi_test_platform_service_destroy(service);
    return failed;
}
