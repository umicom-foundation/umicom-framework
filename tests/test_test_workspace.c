/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_workspace.c
 *
 * PURPOSE:
 *   Verify professional Test Explorer selection, filtering, command state,
 *   operation planning, retained evidence and aggregate coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

/*
 * Exercise add test and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void add_test(UmiTestPlatformService *service, const char *id,
                     const char *name, const char *labels)
{
    UmiTestPlatformItemSnapshot item = {0};

    (void)strcpy(item.id, id);
    (void)strcpy(item.suite_id, "framework.ctest");
    (void)strcpy(item.name, name);
    (void)strcpy(item.framework, "CTest");
    (void)strcpy(item.kind, "test");
    (void)strcpy(item.labels, labels);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(
               umi_test_platform_service_item(service), &item) ==
           UMI_STATUS_OK);
}

/*
 * Exercise add result and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void add_result(UmiTestPlatformService *service, const char *id,
                       const char *item_id, int outcome, uint64_t sequence)
{
    UmiTestPlatformResultSnapshot result = {0};

    (void)strcpy(result.id, id);
    (void)strcpy(result.item_id, item_id);
    (void)strcpy(result.message, outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED
        ? "Assertion failed" : "Passed");
    result.outcome = outcome;
    result.sequence = sequence;
    result.duration_ms = 12.5;
    assert(umi_test_platform_result_registry_upsert(
               umi_test_platform_service_result(service), &result) ==
           UMI_STATUS_OK);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestPlatformService *service = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestWorkspaceSnapshot snapshot;
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformCoverageSnapshot coverage = {0};
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot result;

    assert(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    add_test(service, "framework.alpha", "Framework Alpha", "unit fast");
    add_test(service, "framework.beta", "Framework Beta", "integration");
    add_result(service, "result.alpha", "framework.alpha",
               UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U);
    add_result(service, "result.beta", "framework.beta",
               UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U);
    (void)strcpy(coverage.id, "coverage.framework");
    (void)strcpy(coverage.uri, "src/framework.c");
    coverage.lines_total = 100U;
    coverage.lines_covered = 85U;
    coverage.branches_total = 40U;
    coverage.branches_covered = 30U;
    assert(umi_test_platform_coverage_registry_upsert(
               umi_test_platform_service_coverage(service), &coverage) ==
           UMI_STATUS_OK);

    assert(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    assert(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.visible_item_count == 2U);
    assert(snapshot.passed_count == 1U);
    assert(snapshot.failed_count == 1U);
    assert(snapshot.line_coverage_basis_points == 8500U);
    assert(snapshot.branch_coverage_basis_points == 7500U);
    assert(snapshot.can_run_all);
    assert(snapshot.can_rerun_failed);
    assert(strcmp(snapshot.selected_item_id, "framework.alpha") == 0);

    assert(umi_test_workspace_set_filter(
               workspace, "beta", "", "integration", -1, 0, 0) ==
           UMI_STATUS_OK);
    assert(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.visible_item_count == 1U);
    assert(strcmp(snapshot.selected_item_id, "framework.beta") == 0);
    assert(umi_test_workspace_visible_item_at(
               workspace, 0U, &item, &result) == UMI_STATUS_OK);
    assert(strcmp(item.id, "framework.beta") == 0);
    assert(result.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED);

    assert(umi_test_workspace_set_run_mode(
               workspace, UMI_TEST_WORKSPACE_DEBUG) == UMI_STATUS_OK);
    assert(umi_test_workspace_plan_selected(workspace, &plan) ==
           UMI_STATUS_OK);
    assert(plan.selection.count == 1U);
    assert(strcmp(plan.selection.item_ids[0], "framework.beta") == 0);
    assert(umi_test_workspace_begin(workspace, &plan) == UMI_STATUS_OK);
    assert(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.can_stop);
    assert(umi_test_workspace_request_stop(workspace) == UMI_STATUS_OK);
    umi_test_workspace_finish(workspace);

    assert(umi_test_workspace_plan_failed(workspace, &plan) == UMI_STATUS_OK);
    assert(plan.selection.count == 1U);
    umi_test_workspace_clear_results(workspace);
    assert(umi_test_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.service.result_count == 0U);
    assert(!snapshot.can_rerun_failed);

    umi_test_workspace_destroy(workspace);
    umi_test_platform_service_destroy(service);
    return 0;
}
