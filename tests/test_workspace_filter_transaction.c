/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_filter_transaction.c
 * PURPOSE: Verify complete filter publication without coupling catalogue size to execution plans.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/workspace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"counts", "large", "selection-retained", "selection-replaced", "empty", "text-bound", "suite-bound", "label-bound", "invalid-outcome", "null-query", "null-output", "normalize", "failed-only"};
    const char *mode = argv[1]; int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(cases[i], mode) == 0) known = 1;
    if (!known) return 2;
    UmiTestPlatformService *service = NULL;
    UmiTestWorkspace *workspace = NULL;
    UmiTestWorkspaceSnapshot before, after;
    UmiTestPlatformFilter filter;
    UmiTestPlatformFilterSummary summary = {0}, saved;
    CHECK(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_create(service, &workspace) == UMI_STATUS_OK);
    UmiTestPlatformItemRegistry *items = umi_test_platform_service_item(service);
    UmiTestPlatformResultRegistry *results = umi_test_platform_service_result(service);
    size_t count = strcmp(mode, "large") == 0 ? 26032U : 3U;
    for (size_t i = 0U; i < count; ++i) {
        UmiTestPlatformItemSnapshot item = {0};
        snprintf(item.id, sizeof item.id, "item.%05zu", i);
        snprintf(item.name, sizeof item.name, "Example %05zu", i);
        strcpy(item.kind, "test"); strcpy(item.suite_id, "suite"); item.enabled = 1;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    UmiTestPlatformResultSnapshot result = {0};
    strcpy(result.id, "result"); strcpy(result.item_id, "item.00002");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
    CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
    CHECK(umi_test_workspace_select_item(workspace, "item.00002") == UMI_STATUS_OK);
    CHECK(umi_test_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    umi_test_platform_filter_init(&filter);
    if (strcmp(mode, "selection-replaced") == 0) strcpy(filter.text, "00001");
    if (strcmp(mode, "empty") == 0) strcpy(filter.text, "absent");
    if (strcmp(mode, "normalize") == 0) filter.include_disabled = 9;
    if (strcmp(mode, "failed-only") == 0) filter.failed_only = -2;
    if (strcmp(mode, "text-bound") == 0) memset(filter.text, 'x', sizeof filter.text);
    if (strcmp(mode, "suite-bound") == 0) memset(filter.suite_id, 'x', sizeof filter.suite_id);
    if (strcmp(mode, "label-bound") == 0) memset(filter.label, 'x', sizeof filter.label);
    if (strcmp(mode, "invalid-outcome") == 0) filter.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT + 1;
    summary.matched_count = 987U; saved = summary;
    const int invalid = strstr(mode, "-bound") != NULL || strcmp(mode, "invalid-outcome") == 0 || strncmp(mode, "null-", 5U) == 0;
    CHECK(UmiTestWorkspaceApplyFilter(workspace, strcmp(mode, "null-query") == 0 ? NULL : &filter,
        strcmp(mode, "null-output") == 0 ? NULL : &summary) == (invalid ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
    CHECK(umi_test_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    if (invalid) {
        CHECK(memcmp(&summary, &saved, sizeof summary) == 0);
        CHECK(after.revision == before.revision && memcmp(&after.filter, &before.filter, sizeof filter) == 0);
        CHECK(strcmp(after.selected_item_id, before.selected_item_id) == 0);
    } else {
        const size_t expected = strcmp(mode, "empty") == 0 ? 0U :
            (strcmp(mode, "selection-replaced") == 0 || strcmp(mode, "failed-only") == 0) ? 1U : count;
        CHECK(summary.matched_count == expected && after.visible_item_count == expected);
        CHECK(after.revision == before.revision + 1U && summary.item_count == count);
        CHECK(summary.item_revision == umi_test_platform_item_registry_revision(items));
        CHECK(summary.result_revision == umi_test_platform_result_registry_revision(results));
        CHECK(after.filter.include_disabled == (strcmp(mode, "normalize") == 0));
        CHECK(after.filter.failed_only == (strcmp(mode, "failed-only") == 0));
        const int retained = strcmp(mode, "empty") != 0 && strcmp(mode, "selection-replaced") != 0;
        CHECK(summary.selected_matches == retained);
        CHECK(strcmp(after.selected_item_id, strcmp(mode, "empty") == 0 ? "" : retained ? "item.00002" : "item.00001") == 0);
        if (retained) CHECK(summary.timed_out_count == 1U);
        if (strcmp(mode, "large") == 0) CHECK(!after.can_run_all && after.can_run_selected);
    }
done:
    umi_test_workspace_destroy(workspace); umi_test_platform_service_destroy(service);
    return failed;
}
