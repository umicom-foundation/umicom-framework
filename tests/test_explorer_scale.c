/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_explorer_scale.c
 * PURPOSE: Exercise full-catalogue projection, hierarchy depth, stable state and exact snapshot ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/explorer_page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
static void MakeItem(UmiTestPlatformItemSnapshot *item, size_t index)
{
    memset(item, 0, sizeof *item);
    snprintf(item->id, sizeof item->id, "test.%05zu", index);
    snprintf(item->name, sizeof item->name, "Test %05zu", index);
    strcpy(item->kind, "test"); strcpy(item->framework, "ctest");
    strcpy(item->suite_id, "suite"); item->enabled = 1; item->discovered = 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"empty", "large-flat", "catalogue-limit", "deep-chain", "cycle", "orphan", "ancestor-filter", "source-sort", "outcome-sort", "duration-sort", "framework-sort", "group-suite", "group-framework", "group-outcome", "result-sequence", "result-revision", "snapshot-fields", "state-retained", "state-removed-return", "empty-return", "collapse-expand", "missing-selection", "missing-expansion", "selection-limit", "invalid-filter-text", "invalid-filter-suite", "invalid-filter-label", "page-tail", "page-stale", "owner-lifetime"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(cases[i], argv[1]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    int failed = 0;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestExplorerModel *model = NULL;
    UmiTestExplorerPage *page = NULL;
    UmiTestPlatformSelection *selection = (UmiTestPlatformSelection *)calloc(1U, sizeof *selection);
    UmiTestPlatformItemSnapshot item, expected;
    UmiTestExplorerRow row;
    UmiTestExplorerModelSnapshot summary, before;
    CHECK(selection != NULL);
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_create(&model) == UMI_STATUS_OK);
    size_t count = 3U;
    if (strcmp(mode, "empty") == 0) count = 0U;
    if (strcmp(mode, "large-flat") == 0 || strcmp(mode, "page-tail") == 0) count = 26032U;
    if (strcmp(mode, "catalogue-limit") == 0) count = UMI_TEST_EXPLORER_MODEL_MAX_CAPACITY;
    if (strcmp(mode, "deep-chain") == 0 || strcmp(mode, "ancestor-filter") == 0) count = 16384U;
    if (strcmp(mode, "cycle") == 0) count = 5000U;
    if (strcmp(mode, "selection-limit") == 0) count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
    for (size_t i = 0U; i < count; ++i) {
        MakeItem(&item, i);
        /* Reverse names exercise sorting independently of insertion order. */
        if (strcmp(mode, "large-flat") == 0) snprintf(item.name, sizeof item.name, "Test %05zu", count - i);
        if ((strcmp(mode, "deep-chain") == 0 || strcmp(mode, "ancestor-filter") == 0) && i > 0U)
            snprintf(item.parent_id, sizeof item.parent_id, "test.%05zu", i - 1U);
        if (strcmp(mode, "cycle") == 0)
            snprintf(item.parent_id, sizeof item.parent_id, "test.%05zu", (i + count - 1U) % count);
        if (strcmp(mode, "orphan") == 0) strcpy(item.parent_id, "absent");
        if (strcmp(mode, "source-sort") == 0) { strcpy(item.source_uri, "file.c"); item.source_line = (uint32_t)(count - i); }
        if (strcmp(mode, "framework-sort") == 0 || strcmp(mode, "group-framework") == 0)
            snprintf(item.framework, sizeof item.framework, "framework.%zu", count - i);
        if (strcmp(mode, "group-suite") == 0) snprintf(item.suite_id, sizeof item.suite_id, "suite.%zu", count - i);
        if (strcmp(mode, "snapshot-fields") == 0) {
#define FILL(field) memset(item.field, 'x', sizeof item.field - 1U); item.field[sizeof item.field - 1U] = 0
            FILL(uri); FILL(source_uri); FILL(labels); FILL(command); FILL(working_directory);
#undef FILL
            item.source_line = 456U; item.timeout_ms = 987U; item.last_outcome = 3;
            item.last_duration_ms = 9.25; item.enabled = 1; item.discovered = 1;
        }
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    if (strstr(mode, "result-") || strstr(mode, "outcome") || strcmp(mode, "duration-sort") == 0) {
        for (size_t i = 0U; i < 3U; ++i) {
            UmiTestPlatformResultSnapshot result = {0};
            snprintf(result.id, sizeof result.id, "result.%zu", i);
            snprintf(result.item_id, sizeof result.item_id, "test.%05zu", strstr(mode, "result-") ? 0U : i);
            result.sequence = strcmp(mode, "result-revision") == 0 ? 1U :
                strcmp(mode, "result-sequence") == 0 ? (i == 0U ? 50U : 1U) : i + 1U;
            result.duration_ms = (double)i;
            result.outcome = i == 2U ? UMI_TEST_PLATFORM_OUTCOME_FAILED : UMI_TEST_PLATFORM_OUTCOME_PASSED;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
    }
    if (strcmp(mode, "source-sort") == 0) CHECK(umi_test_explorer_model_set_sort(model, UMI_TEST_EXPLORER_SORT_SOURCE) == UMI_STATUS_OK);
    if (strcmp(mode, "outcome-sort") == 0) CHECK(umi_test_explorer_model_set_sort(model, UMI_TEST_EXPLORER_SORT_OUTCOME) == UMI_STATUS_OK);
    if (strcmp(mode, "duration-sort") == 0) CHECK(umi_test_explorer_model_set_sort(model, UMI_TEST_EXPLORER_SORT_DURATION) == UMI_STATUS_OK);
    if (strcmp(mode, "framework-sort") == 0) CHECK(umi_test_explorer_model_set_sort(model, UMI_TEST_EXPLORER_SORT_FRAMEWORK) == UMI_STATUS_OK);
    if (strcmp(mode, "group-suite") == 0) CHECK(umi_test_explorer_model_set_group(model, UMI_TEST_EXPLORER_GROUP_SUITE) == UMI_STATUS_OK);
    if (strcmp(mode, "group-framework") == 0) CHECK(umi_test_explorer_model_set_group(model, UMI_TEST_EXPLORER_GROUP_FRAMEWORK) == UMI_STATUS_OK);
    if (strcmp(mode, "group-outcome") == 0) CHECK(umi_test_explorer_model_set_group(model, UMI_TEST_EXPLORER_GROUP_OUTCOME) == UMI_STATUS_OK);
    if (strcmp(mode, "ancestor-filter") == 0) {
        UmiTestPlatformFilter filter;
        umi_test_platform_filter_init(&filter);
        snprintf(filter.text, sizeof filter.text, "test.%05zu", count - 1U);
        CHECK(umi_test_explorer_model_set_filter(model, &filter) == UMI_STATUS_OK);
    }
    CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_snapshot(model, &summary) == UMI_STATUS_OK);
    CHECK(summary.row_count == count && summary.visible_row_count == count);
    if (strcmp(mode, "empty") == 0) {
        CHECK(summary.root_count == 0U);
        CHECK(umi_test_explorer_model_visible_row_at(model, 0U, &row) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "catalogue-limit") == 0 || strcmp(mode, "page-tail") == 0) {
        CHECK(UmiTestExplorerPageCreate(model, SIZE_MAX, 300U, &page) == UMI_STATUS_OK);
        UmiTestExplorerPageSummary page_summary;
        CHECK(UmiTestExplorerPageRead(page, &page_summary) == UMI_STATUS_OK);
        CHECK(page_summary.offset + page_summary.count == count && !page_summary.has_next);
        CHECK(UmiTestExplorerPageRowAt(page, page_summary.count - 1U, &row) == UMI_STATUS_OK);
        char tail[128]; snprintf(tail, sizeof tail, "test.%05zu", count - 1U);
        CHECK(strcmp(row.item.id, tail) == 0);
    } else if (strcmp(mode, "large-flat") == 0) {
        CHECK(umi_test_explorer_model_visible_row_at(model, 0U, &row) == UMI_STATUS_OK);
        CHECK(strcmp(row.item.id, "test.26031") == 0);
    } else if (strcmp(mode, "deep-chain") == 0 || strcmp(mode, "ancestor-filter") == 0 || strcmp(mode, "cycle") == 0) {
        CHECK(summary.root_count == 1U);
        CHECK(umi_test_explorer_model_row_at(model, count - 1U, &row) == UMI_STATUS_OK && row.depth == count - 1U);
        CHECK(umi_test_explorer_model_collapse_all(model) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_visible_count(model) == 1U);
        CHECK(umi_test_explorer_model_expand_all(model) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_visible_count(model) == count);
    } else if (strcmp(mode, "orphan") == 0) CHECK(summary.root_count == count);
    else if (strstr(mode, "-sort") || strncmp(mode, "group-", 6U) == 0) {
        CHECK(umi_test_explorer_model_visible_row_at(model, 0U, &row) == UMI_STATUS_OK);
        CHECK(strcmp(row.item.id, "test.00002") == 0);
    } else if (strstr(mode, "result-")) {
        CHECK(umi_test_explorer_model_row_at(model, 0U, &row) == UMI_STATUS_OK);
        CHECK(row.has_result && strcmp(row.latest_result.id,
            strcmp(mode, "result-sequence") == 0 ? "result.0" : "result.2") == 0);
    } else if (strcmp(mode, "snapshot-fields") == 0) {
        CHECK(umi_test_platform_item_registry_find(items, "test.00000", &expected) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_row_at(model, 0U, &row) == UMI_STATUS_OK);
#define SAME(field) CHECK(strcmp(row.item.field, expected.field) == 0)
        SAME(id); SAME(parent_id); SAME(suite_id); SAME(name); SAME(uri); SAME(source_uri);
        SAME(framework); SAME(kind); SAME(labels); SAME(command); SAME(working_directory);
#undef SAME
        CHECK(row.item.timeout_ms == 987U && row.item.source_line == 456U && row.item.last_duration_ms == 9.25);
        CHECK(row.item.revision == expected.revision && row.item.api_version == expected.api_version);
    } else if (strncmp(mode, "invalid-filter-", 15U) == 0) {
        UmiTestPlatformFilter filter;
        umi_test_platform_filter_init(&filter);
        if (strcmp(mode, "invalid-filter-text") == 0) memset(filter.text, 'x', sizeof filter.text);
        if (strcmp(mode, "invalid-filter-suite") == 0) memset(filter.suite_id, 'x', sizeof filter.suite_id);
        if (strcmp(mode, "invalid-filter-label") == 0) memset(filter.label, 'x', sizeof filter.label);
        CHECK(umi_test_explorer_model_set_filter(model, &filter) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_test_explorer_model_revision(model) == summary.revision);
    } else if (strcmp(mode, "selection-limit") == 0) {
        for (size_t i = 0U; i < count; ++i) {
            char id[128]; snprintf(id, sizeof id, "test.%05zu", i);
            CHECK(umi_test_explorer_model_select(model, id, 1, 0) == UMI_STATUS_OK);
        }
        selection->count = 17U;
        CHECK(umi_test_explorer_model_selection(model, selection) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(selection->count == 17U);
    } else if (strcmp(mode, "page-stale") == 0 || strcmp(mode, "owner-lifetime") == 0) {
        CHECK(UmiTestExplorerPageCreate(model, 0U, 300U, &page) == UMI_STATUS_OK);
        if (strcmp(mode, "page-stale") == 0) {
            CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
            CHECK(UmiTestExplorerPageValidate(page, model) == UMI_STATUS_INVALID_STATE);
        } else {
            umi_test_explorer_model_destroy(model); model = NULL;
            umi_test_platform_item_registry_destroy(items); items = NULL;
            CHECK(UmiTestExplorerPageRowAt(page, 2U, &row) == UMI_STATUS_OK);
            CHECK(strcmp(row.item.id, "test.00002") == 0);
        }
    } else {
        CHECK(umi_test_explorer_model_select(model, "test.00001", 1, 1) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_set_expanded(model, "test.00001", 0) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_snapshot(model, &before) == UMI_STATUS_OK);
        if (strcmp(mode, "missing-selection") == 0) {
            CHECK(umi_test_explorer_model_select(model, "missing", 1, 1) == UMI_STATUS_NOT_FOUND);
            CHECK(umi_test_explorer_model_revision(model) == before.revision);
        } else if (strcmp(mode, "missing-expansion") == 0) {
            CHECK(umi_test_explorer_model_set_expanded(model, "missing", 1) == UMI_STATUS_NOT_FOUND);
            CHECK(umi_test_explorer_model_revision(model) == before.revision);
        } else if (strcmp(mode, "collapse-expand") == 0) {
            CHECK(umi_test_explorer_model_collapse_all(model) == UMI_STATUS_OK);
            CHECK(umi_test_explorer_model_expand_all(model) == UMI_STATUS_OK);
        } else {
            if (strcmp(mode, "empty-return") == 0) umi_test_platform_item_registry_clear(items);
            if (strcmp(mode, "state-removed-return") == 0)
                CHECK(umi_test_platform_item_registry_remove(items, "test.00001") == UMI_STATUS_OK);
            CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
            if (strcmp(mode, "state-retained") != 0) {
                MakeItem(&item, 1U);
                CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
                CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
            }
        }
        CHECK(umi_test_explorer_model_selection(model, selection) == UMI_STATUS_OK);
        CHECK(selection->count == 1U && strcmp(selection->item_ids[0], "test.00001") == 0);
        if (strncmp(mode, "state-", 6U) == 0 || strcmp(mode, "empty-return") == 0) {
            int found = 0;
            for (size_t i = 0U; i < umi_test_explorer_model_count(model); ++i) {
                CHECK(umi_test_explorer_model_row_at(model, i, &row) == UMI_STATUS_OK);
                if (strcmp(row.item.id, "test.00001") == 0) { CHECK(!row.expanded && row.selected); found = 1; }
            }
            CHECK(found);
        }
    }
done:
    free(selection); UmiTestExplorerPageDestroy(page); umi_test_explorer_model_destroy(model);
    umi_test_platform_result_registry_destroy(results); umi_test_platform_item_registry_destroy(items);
    return failed;
}
