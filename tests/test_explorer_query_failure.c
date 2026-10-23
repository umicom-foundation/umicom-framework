/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_explorer_query_failure.c
 * PURPOSE: Verify refused query changes preserve all published Explorer representations and release staged storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "support/explorer_query_fixture.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *names[] = {"model-allocation", "filter-allocation", "sort-allocation", "group-allocation",
        "workspace-allocation", "workspace-stale", "workspace-overflow", "session-overflow", "model-overflow", "retry"};
    int mode = -1, failed = 0;
    for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
        if (strcmp(argv[1], names[i]) == 0) mode = (int)i;
    if (mode < 0) return 2;
    UmiTestPlatformService *service = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestExplorerSession *session = NULL;
    UmiTestExplorerSessionSnapshot *before = calloc(1U, sizeof *before);
    UmiTestExplorerSessionSnapshot *after = calloc(1U, sizeof *after);
    UmiTestExplorerRow first, retained;
    UmiTestPlatformFilter query;
    UmiTestPlatformFilterSummary summary = {0}, saved;
    UmiTestExplorerModel *model = NULL;
    CHECK(before != NULL && after != NULL);
    CHECK(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    UmiTestPlatformItemSnapshot item = {0};
    strcpy(item.kind, "test"); item.enabled = 1;
    for (size_t i = 0U; i < 30U; ++i) {
        snprintf(item.id, sizeof item.id, "test.%02zu", i);
        snprintf(item.name, sizeof item.name, "Test %02zu", i);
        strcpy(item.framework, i % 2U == 0U ? "alpha" : "beta");
        memset(item.command, 'x', sizeof item.command - 1U);
        CHECK(umi_test_platform_item_registry_upsert(umi_test_platform_service_item(service), &item) == UMI_STATUS_OK);
    }
    CHECK(umi_test_explorer_session_create_bound(service, workspace, &session) == UMI_STATUS_OK);
    umi_test_platform_filter_init(&query);
    CHECK(umi_test_explorer_session_set_filter(session, &query) == UMI_STATUS_OK);
    model = umi_test_explorer_session_model(session);
    CHECK(umi_test_explorer_model_select(model, "test.00", 1, 1) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_set_expanded(model, "test.00", 0) == UMI_STATUS_OK);
    if (mode == 7) session->revision = UINT64_MAX;
    if (mode == 8) model->revision = UINT64_MAX;
    CHECK(umi_test_explorer_session_snapshot(session, before) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_row_at(model, 0U, &first) == UMI_STATUS_OK);
    strcpy(query.text, "test.00");
    summary.matched_count = 987U; saved = summary;
    const size_t initial_live = live;
    int reached_success = 0;
    for (size_t point = 0U; point < 100U; ++point) {
        calls = 0U;
        reject = mode <= 3 ? point : SIZE_MAX;
        query_workspace_refusal = mode == 5 ? UMI_STATUS_INVALID_STATE :
            mode == 6 ? UMI_STATUS_CAPACITY_EXCEEDED :
            (mode == 4 || mode == 9) ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
        UmiStatus status;
        if (mode == 0) status = UmiTestExplorerModelApplyQuery(model,
            umi_test_platform_service_item(service), umi_test_platform_service_result(service),
            &query, UMI_TEST_EXPLORER_SORT_FRAMEWORK, UMI_TEST_EXPLORER_GROUP_FRAMEWORK);
        else if (mode == 2) status = umi_test_explorer_session_set_sort(session, UMI_TEST_EXPLORER_SORT_FRAMEWORK);
        else if (mode == 3) status = umi_test_explorer_session_set_group(session, UMI_TEST_EXPLORER_GROUP_FRAMEWORK);
        else status = UmiTestExplorerSessionApplyFilter(session, &query, &summary);
        reject = SIZE_MAX; query_workspace_refusal = UMI_STATUS_OK;
        if (status == UMI_STATUS_OK) { reached_success = 1; break; }
        UmiStatus expected = mode == 5 ? UMI_STATUS_INVALID_STATE :
            (mode >= 6 && mode <= 8) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OUT_OF_MEMORY;
        CHECK(status == expected && live == initial_live);
        CHECK(umi_test_explorer_session_snapshot(session, after) == UMI_STATUS_OK);
        CHECK(memcmp(before, after, sizeof *before) == 0);
        CHECK(umi_test_explorer_model_row_at(model, 0U, &retained) == UMI_STATUS_OK);
        CHECK(memcmp(&first, &retained, sizeof first) == 0);
        CHECK(memcmp(&summary, &saved, sizeof summary) == 0);
        if (mode > 3) break;
    }
    if (mode <= 3) CHECK(reached_success);
    if (mode == 9) {
        CHECK(UmiTestExplorerSessionApplyFilter(session, &query, &summary) == UMI_STATUS_OK);
        CHECK(summary.matched_count == 1U);
        CHECK(umi_test_explorer_session_snapshot(session, after) == UMI_STATUS_OK);
        CHECK(after->revision == before->revision + 1U);
        CHECK(after->explorer.revision == before->explorer.revision + 1U);
        CHECK(after->workspace.revision == before->workspace.revision + 1U);
    }
done:
    reject = SIZE_MAX; query_workspace_refusal = UMI_STATUS_OK;
    umi_test_explorer_session_destroy(session);
    umi_test_workspace_destroy(workspace);
    umi_test_platform_service_destroy(service);
    free(before); free(after);
    if (live != 0U) { fprintf(stderr, "Retained projection allocations: %zu\n", live); failed = 1; }
    return failed;
}
