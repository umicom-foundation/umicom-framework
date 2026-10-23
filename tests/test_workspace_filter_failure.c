/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_filter_failure.c
 * PURPOSE: Verify failed catalogue capture never commits a query, selection or summary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Rename this fixture's source unity so the ordinary library remains available.
 * Fault injection is local to the test and adds no production control switch. */
#define UmiExplorerWorkspaceService FilterFixtureWorkspaceOwner
#define umi_test_workspace_create FilterFixtureWorkspace0
#define umi_test_workspace_destroy FilterFixtureWorkspace1
#define umi_test_workspace_refresh FilterFixtureWorkspace2
#define umi_test_workspace_snapshot FilterFixtureWorkspace3
#define UmiTestWorkspaceApplyFilter FilterFixtureWorkspace4
#define umi_test_workspace_set_filter FilterFixtureWorkspace5
#define umi_test_workspace_select_item FilterFixtureWorkspace6
#define umi_test_workspace_select_session FilterFixtureWorkspace7
#define umi_test_workspace_set_run_mode FilterFixtureWorkspace8
#define umi_test_workspace_plan_all FilterFixtureWorkspace9
#define umi_test_workspace_plan_selected FilterFixtureWorkspace10
#define umi_test_workspace_plan_failed FilterFixtureWorkspace11
#define umi_test_workspace_plan_repeat_selected FilterFixtureWorkspace12
#define umi_test_workspace_begin FilterFixtureWorkspace13
#define umi_test_workspace_request_stop FilterFixtureWorkspace14
#define umi_test_workspace_finish FilterFixtureWorkspace15
#define umi_test_workspace_clear_results FilterFixtureWorkspace16
#define umi_test_workspace_clear_output FilterFixtureWorkspace17
#define umi_test_workspace_clear_coverage FilterFixtureWorkspace18
#define umi_test_workspace_visible_item_at FilterFixtureWorkspace19
#define umi_test_workspace_result_at FilterFixtureWorkspace20
#define umi_test_workspace_failure_at FilterFixtureWorkspace21
#define umi_test_workspace_output_at FilterFixtureWorkspace22
#define umi_test_workspace_coverage_at FilterFixtureWorkspace23
#define umi_test_workspace_session_at FilterFixtureWorkspace24
#define umi_test_workspace_run_mode_text FilterFixtureWorkspace25
#include "umicom/test_platform/workspace.h"
#include <stdio.h>
#include <string.h>
static UmiStatus injected_status = UMI_STATUS_OK;
static UmiStatus FilterFixtureInspect(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected, UmiTestPlatformFilterSummary *summary)
{
    if (injected_status != UMI_STATUS_OK) return injected_status;
    return UmiTestPlatformFilterInspect(items, results, filter, selected, summary);
}
#define UmiTestPlatformFilterInspect FilterFixtureInspect
#include "../src/test_platform/workspace.c"
#undef UmiTestPlatformFilterInspect
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"allocation", "stale", "revision-overflow", "retry"}; int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    UmiTestPlatformService *service = NULL; UmiTestWorkspace *workspace = NULL;
    CHECK(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    UmiTestPlatformFilter filter; umi_test_platform_filter_init(&filter); strcpy(filter.text, "new query");
    UmiTestPlatformFilterSummary summary = {0}, saved;
    summary.matched_count = 71U; saved = summary;
    UmiStatus expected = strcmp(argv[1], "stale") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OUT_OF_MEMORY;
    injected_status = expected;
    if (strcmp(argv[1], "revision-overflow") == 0) {
        workspace->revision = UINT64_MAX; injected_status = UMI_STATUS_OK; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiTestWorkspace before = *workspace;
    CHECK(UmiTestWorkspaceApplyFilter(workspace, &filter, &summary) == expected);
    CHECK(memcmp(workspace, &before, sizeof before) == 0 && memcmp(&summary, &saved, sizeof summary) == 0);
    if (strcmp(argv[1], "retry") == 0) {
        injected_status = UMI_STATUS_OK;
        CHECK(UmiTestWorkspaceApplyFilter(workspace, &filter, &summary) == UMI_STATUS_OK);
        CHECK(workspace->revision == before.revision + 1U && strcmp(workspace->filter.text, "new query") == 0);
        CHECK(summary.matched_count == 0U);
    }
done:
    umi_test_workspace_destroy(workspace); umi_test_platform_service_destroy(service);
    return failed;
}
