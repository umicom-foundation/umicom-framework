/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_explorer_query.c
 * PURPOSE: Exercise coherent query publication, exact filtering and retained Explorer presentation state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/explorer_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *names[] = {"filter", "clear", "suite", "disabled", "sort", "group", "combined",
        "selection", "empty", "invalid-text", "invalid-sort", "invalid-group", "invalid-outcome",
        "null-output", "null-filter", "binding", "bound-create"};
    int mode = -1, failed = 0;
    for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
        if (strcmp(argv[1], names[i]) == 0) mode = (int)i;
    if (mode < 0) return 2;
    UmiTestPlatformService *service = NULL, *other = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestExplorerSession *session = NULL, *refused = NULL;
    UmiTestExplorerSessionSnapshot *before = calloc(1U, sizeof *before);
    UmiTestExplorerSessionSnapshot *after = calloc(1U, sizeof *after);
    UmiTestExplorerModel *model = NULL;
    UmiTestPlatformFilter query;
    UmiTestPlatformFilterSummary summary = {0}, saved;
    CHECK(before != NULL && after != NULL);
    CHECK(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    UmiTestPlatformItemSnapshot item = {0};
    strcpy(item.kind, "test"); item.enabled = 1;
    strcpy(item.id, "alpha"); strcpy(item.name, "Alpha"); strcpy(item.framework, "zeta"); strcpy(item.suite_id, "suite.a");
    CHECK(umi_test_platform_item_registry_upsert(umi_test_platform_service_item(service), &item) == UMI_STATUS_OK);
    strcpy(item.id, "beta"); strcpy(item.name, "Beta"); strcpy(item.framework, "alpha"); strcpy(item.suite_id, "suite.b");
    CHECK(umi_test_platform_item_registry_upsert(umi_test_platform_service_item(service), &item) == UMI_STATUS_OK);
    strcpy(item.id, "disabled"); strcpy(item.name, "Disabled"); item.enabled = 0;
    CHECK(umi_test_platform_item_registry_upsert(umi_test_platform_service_item(service), &item) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_session_create_bound(service, workspace, &session) == UMI_STATUS_OK);
    umi_test_platform_filter_init(&query);
    CHECK(umi_test_explorer_session_set_filter(session, &query) == UMI_STATUS_OK);
    model = umi_test_explorer_session_model(session);
    CHECK(umi_test_explorer_model_select(model, "alpha", 1, 1) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_set_expanded(model, "alpha", 0) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_session_snapshot(session, before) == UMI_STATUS_OK);
    summary.matched_count = 777U; saved = summary;
    if (mode == 15 || mode == 16) {
        CHECK(umi_test_platform_service_create(&other) == UMI_STATUS_OK);
        CHECK(UmiTestExplorerSessionBindingMatches(session, service, workspace));
        CHECK(!UmiTestExplorerSessionBindingMatches(session, other, workspace));
        CHECK(!UmiTestExplorerSessionBindingMatches(NULL, service, workspace));
        if (mode == 16) {
            CHECK(umi_test_explorer_session_create_bound(other, workspace, &refused) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(refused == NULL);
        }
        goto done;
    }
    if (mode == 0 || mode == 7) strcpy(query.text, "beta");
    if (mode == 1) {
        strcpy(query.text, "beta"); CHECK(umi_test_explorer_session_set_filter(session, &query) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_session_snapshot(session, before) == UMI_STATUS_OK);
        query.text[0] = 0;
    }
    if (mode == 2) strcpy(query.suite_id, "suite.b");
    if (mode == 3) query.include_disabled = 9;
    if (mode == 8) strcpy(query.text, "no match");
    if (mode == 9) memset(query.text, 'x', sizeof query.text);
    if (mode == 12) query.outcome = 999;
    UmiStatus status;
    if (mode == 4 || mode == 10) status = umi_test_explorer_session_set_sort(session,
        mode == 10 ? (UmiTestExplorerSortMode)999 : UMI_TEST_EXPLORER_SORT_FRAMEWORK);
    else if (mode == 5 || mode == 11) status = umi_test_explorer_session_set_group(session,
        mode == 11 ? (UmiTestExplorerGroupMode)999 : UMI_TEST_EXPLORER_GROUP_FRAMEWORK);
    else if (mode == 6) status = UmiTestExplorerModelApplyQuery(model,
        umi_test_platform_service_item(service), umi_test_platform_service_result(service), &query,
        UMI_TEST_EXPLORER_SORT_FRAMEWORK, UMI_TEST_EXPLORER_GROUP_FRAMEWORK);
    else status = UmiTestExplorerSessionApplyFilter(session, mode == 14 ? NULL : &query, mode == 13 ? NULL : &summary);
    CHECK(umi_test_explorer_session_model(session) == model);
    CHECK(umi_test_explorer_session_snapshot(session, after) == UMI_STATUS_OK);
    if (mode >= 9) {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, after, sizeof *before) == 0);
        CHECK(memcmp(&summary, &saved, sizeof summary) == 0);
    } else {
        CHECK(status == UMI_STATUS_OK);
        CHECK(after->explorer.revision == before->explorer.revision + 1U);
        CHECK(after->revision == before->revision + (mode == 6 ? 0U : 1U));
        if (mode == 4 || mode == 5 || mode == 6) {
            UmiTestExplorerRow row;
            CHECK(umi_test_explorer_model_visible_row_at(model, 0U, &row) == UMI_STATUS_OK);
            CHECK(strcmp(row.item.id, "beta") == 0);
            CHECK(after->workspace.revision == before->workspace.revision);
        } else {
            CHECK(after->workspace.revision == before->workspace.revision + 1U);
            CHECK(strcmp(after->workspace.filter.text, after->explorer.filter.text) == 0);
            CHECK(strcmp(after->workspace.filter.suite_id, after->explorer.filter.suite_id) == 0);
            size_t expected = mode == 3 ? 3U : mode == 8 ? 0U : mode == 1 ? 2U : 1U;
            CHECK(summary.matched_count == expected && after->explorer.visible_row_count == expected);
            CHECK(after->workspace.visible_item_count == expected);
            CHECK(after->workspace.filter.include_disabled == (mode == 3 ? 1 : 0));
        }
        CHECK(after->explorer.selected_count == 1U);
        if (mode == 7) {
            query.text[0] = 0;
            CHECK(umi_test_explorer_session_set_filter(session, &query) == UMI_STATUS_OK);
            UmiTestExplorerRow row;
            CHECK(umi_test_explorer_model_visible_row_at(model, 0U, &row) == UMI_STATUS_OK);
            CHECK(strcmp(row.item.id, "alpha") == 0 && row.selected && !row.expanded);
        }
    }
done:
    umi_test_explorer_session_destroy(refused);
    umi_test_explorer_session_destroy(session);
    umi_test_workspace_destroy(workspace);
    umi_test_platform_service_destroy(other);
    umi_test_platform_service_destroy(service);
    free(before); free(after);
    return failed;
}
