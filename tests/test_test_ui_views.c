/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_ui_views.c
 *
 * PURPOSE:
 *   Verify every professional testing pane is projected from one Framework
 *   workspace and publishes capability-aware command actions.
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

#include "umicom/test_ui/test_ui.h"
#include "umicom/ui/command_view.h"

typedef UmiStatus (*ViewFactory)(
    const char *, UmiTestWorkspace *, UmiUiViewModel **);

static void verify_view(ViewFactory factory, const char *view_id,
                        const char *expected_kind,
                        UmiTestWorkspace *workspace)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(factory(view_id, workspace, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiTestPlatformService *service = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestPlatformItemSnapshot item = {0};
    UmiTestPlatformResultSnapshot result = {0};
    UmiTestPlatformOutputSnapshot output = {0};
    UmiTestPlatformCoverageSnapshot coverage = {0};
    UmiTestPlatformRunSessionSnapshot session = {0};
    UmiUiViewModel *explorer = NULL;
    UmiUiCommandViewAction action;

    assert(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    (void)strcpy(item.id, "framework.alpha");
    (void)strcpy(item.name, "Framework Alpha");
    (void)strcpy(item.kind, "test");
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(
               umi_test_platform_service_item(service), &item) ==
           UMI_STATUS_OK);
    (void)strcpy(result.id, "result.alpha");
    (void)strcpy(result.item_id, item.id);
    (void)strcpy(result.message, "Assertion failed");
    (void)strcpy(result.failure_details, "expected 42; received 7");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.sequence = 1U;
    assert(umi_test_platform_result_registry_upsert(
               umi_test_platform_service_result(service), &result) ==
           UMI_STATUS_OK);
    (void)strcpy(output.id, "output.alpha");
    (void)strcpy(output.item_id, item.id);
    (void)strcpy(output.stream, "stderr");
    (void)strcpy(output.text, "assertion failed");
    assert(umi_test_platform_output_registry_upsert(
               umi_test_platform_service_output(service), &output) ==
           UMI_STATUS_OK);
    (void)strcpy(coverage.id, "coverage.alpha");
    (void)strcpy(coverage.uri, "src/alpha.c");
    coverage.lines_total = 10U;
    coverage.lines_covered = 8U;
    assert(umi_test_platform_coverage_registry_upsert(
               umi_test_platform_service_coverage(service), &coverage) ==
           UMI_STATUS_OK);
    (void)strcpy(session.id, "run.alpha");
    session.total = 1U;
    session.failed = 1U;
    assert(umi_test_platform_run_session_registry_upsert(
               umi_test_platform_service_run_session(service), &session) ==
           UMI_STATUS_OK);
    assert(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);

    verify_view(umi_test_ui_explorer_view_create, "tests", "test-explorer",
                workspace);
    verify_view(umi_test_ui_results_view_create, "results", "test-results",
                workspace);
    verify_view(umi_test_ui_failures_view_create, "failures", "test-failures",
                workspace);
    verify_view(umi_test_ui_output_view_create, "output", "test-output",
                workspace);
    verify_view(umi_test_ui_coverage_view_create, "coverage", "test-coverage",
                workspace);
    verify_view(umi_test_ui_runs_view_create, "runs", "test-runs", workspace);

    assert(umi_test_ui_explorer_view_create(
               "tests.actions", workspace, &explorer) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(explorer, 3U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.test.run-all") == 0);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(explorer, 7U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.test.rerun-failed") == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(explorer);

    umi_test_workspace_destroy(workspace);
    umi_test_platform_service_destroy(service);
    return 0;
}
