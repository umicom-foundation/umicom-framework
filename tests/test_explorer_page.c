/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_explorer_page.c
 * PURPOSE: Verify model ordering, page bounds, retained ownership and invalidation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/explorer_page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"empty", "first", "middle", "last", "aligned", "oversized", "single", "exact-boundary", "order", "filter", "hierarchy", "outcomes", "immutable", "stale-sort", "stale-selection", "stale-rebuild", "wrong-model", "arguments"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(cases[i], argv[1]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    int failed = 0;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestExplorerModel *model = NULL, *other = NULL;
    UmiTestExplorerPage *page = NULL, *second = NULL;
    UmiTestExplorerPageSummary s;
    UmiTestExplorerRow row, expected;
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_create(&model) == UMI_STATUS_OK);
    size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "exact-boundary") == 0 ? 600U : 607U;
    for (size_t i = 0U; i < count; ++i) {
        UmiTestPlatformItemSnapshot item = {0};
        (void)snprintf(item.id, sizeof item.id, "test.%04zu", i);
        (void)snprintf(item.name, sizeof item.name, "Test %04zu", count - i);
        strcpy(item.kind, "test"); strcpy(item.framework, "ctest"); item.enabled = 1;
        if (strcmp(mode, "hierarchy") == 0 && i != 0U) strcpy(item.parent_id, "test.0000");
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
        if (i < 6U) {
            const int outcomes[] = {UMI_TEST_PLATFORM_OUTCOME_NOT_RUN, UMI_TEST_PLATFORM_OUTCOME_PASSED,
                UMI_TEST_PLATFORM_OUTCOME_FAILED, UMI_TEST_PLATFORM_OUTCOME_SKIPPED,
                UMI_TEST_PLATFORM_OUTCOME_CANCELLED, UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT};
            UmiTestPlatformResultSnapshot result = {0};
            (void)snprintf(result.id, sizeof result.id, "result.%zu", i);
            strcpy(result.item_id, item.id); result.outcome = outcomes[i]; result.sequence = i + 1U;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
    }
    UmiTestPlatformFilter filter;
    umi_test_platform_filter_init(&filter);
    if (strcmp(mode, "filter") == 0) strcpy(filter.text, "test.0606");
    CHECK(umi_test_explorer_model_set_filter(model, &filter) == UMI_STATUS_OK);
    CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
    size_t offset = 0U, size = 300U;
    if (strcmp(mode, "middle") == 0) offset = 300U;
    if (strcmp(mode, "last") == 0 || strcmp(mode, "exact-boundary") == 0) offset = 600U;
    if (strcmp(mode, "aligned") == 0) offset = 599U;
    if (strcmp(mode, "oversized") == 0 || strcmp(mode, "filter") == 0) offset = SIZE_MAX;
    if (strcmp(mode, "single") == 0) { offset = 606U; size = 1U; }
    CHECK(UmiTestExplorerPageCreate(model, offset, size, &page) == UMI_STATUS_OK);
    CHECK(UmiTestExplorerPageRead(page, &s) == UMI_STATUS_OK);
    CHECK(s.page_size == size && s.count <= size);
    CHECK(UmiTestExplorerPageValidate(page, model) == UMI_STATUS_OK);
    if (strcmp(mode, "empty") == 0) CHECK(s.count == 0U && s.page_count == 0U && !s.has_previous && !s.has_next);
    else if (strcmp(mode, "single") == 0) CHECK(s.offset == 606U && s.count == 1U && s.page_count == 607U && !s.has_next);
    else if (strcmp(mode, "filter") == 0) CHECK(s.count == 1U && s.offset == 0U && s.page_count == 1U);
    else if (strcmp(mode, "last") == 0 || strcmp(mode, "oversized") == 0)
        CHECK(s.offset == 600U && s.count == 7U && s.previous_offset == 300U && !s.has_next);
    else if (strcmp(mode, "middle") == 0 || strcmp(mode, "aligned") == 0)
        CHECK(s.offset == 300U && s.count == 300U && s.has_previous && s.has_next && s.next_offset == 600U);
    else if (strcmp(mode, "exact-boundary") == 0)
        CHECK(s.offset == 300U && s.count == 300U && s.page_count == 2U && !s.has_next);
    else CHECK(s.offset == 0U && s.count == 300U && s.has_next && s.next_offset == 300U);
    for (size_t i = 0U; i < s.count; ++i) {
        CHECK(UmiTestExplorerPageRowAt(page, i, &row) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_visible_row_at(model, s.offset + i, &expected) == UMI_STATUS_OK);
        CHECK(strcmp(row.item.id, expected.item.id) == 0 && row.depth == expected.depth);
    }
    if (strcmp(mode, "order") == 0) {
        CHECK(UmiTestExplorerPageRowAt(page, 0U, &row) == UMI_STATUS_OK);
        CHECK(strcmp(row.item.id, "test.0606") == 0);
    } else if (strcmp(mode, "hierarchy") == 0) {
        CHECK(UmiTestExplorerPageRowAt(page, 0U, &row) == UMI_STATUS_OK && row.depth == 0U);
        CHECK(UmiTestExplorerPageRowAt(page, 1U, &row) == UMI_STATUS_OK && row.depth == 1U);
    } else if (strcmp(mode, "outcomes") == 0) {
        const int outcomes[] = {UMI_TEST_PLATFORM_OUTCOME_NOT_RUN, UMI_TEST_PLATFORM_OUTCOME_PASSED,
            UMI_TEST_PLATFORM_OUTCOME_FAILED, UMI_TEST_PLATFORM_OUTCOME_SKIPPED,
            UMI_TEST_PLATFORM_OUTCOME_CANCELLED, UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT};
        CHECK(UmiTestExplorerPageCreate(model, 600U, 300U, &second) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 6U; ++i) {
            CHECK(UmiTestExplorerPageRowAt(second, 6U - i, &row) == UMI_STATUS_OK);
            CHECK(row.has_result && row.latest_result.outcome == outcomes[i]);
        }
        CHECK(s.model.failed_count == 2U && s.model.passed_count == 1U && s.model.skipped_count == 1U);
    } else if (strcmp(mode, "immutable") == 0) {
        CHECK(UmiTestExplorerPageRowAt(page, 0U, &expected) == UMI_STATUS_OK);
        umi_test_explorer_model_destroy(model); model = NULL;
        CHECK(UmiTestExplorerPageRowAt(page, 0U, &row) == UMI_STATUS_OK && strcmp(row.item.id, expected.item.id) == 0);
    } else if (strncmp(mode, "stale-", 6U) == 0) {
        if (strcmp(mode, "stale-sort") == 0) CHECK(umi_test_explorer_model_set_sort(model, UMI_TEST_EXPLORER_SORT_SOURCE) == UMI_STATUS_OK);
        if (strcmp(mode, "stale-selection") == 0) CHECK(umi_test_explorer_model_select(model, "test.0001", 1, 1) == UMI_STATUS_OK);
        if (strcmp(mode, "stale-rebuild") == 0) CHECK(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
        CHECK(UmiTestExplorerPageValidate(page, model) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "wrong-model") == 0) {
        CHECK(umi_test_explorer_model_create(&other) == UMI_STATUS_OK);
        CHECK(umi_test_explorer_model_rebuild(other, items, results) == UMI_STATUS_OK);
        CHECK(UmiTestExplorerPageValidate(page, other) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "arguments") == 0) {
        second = page;
        CHECK(UmiTestExplorerPageCreate(model, 0U, 0U, &second) == UMI_STATUS_INVALID_ARGUMENT && second == NULL);
        CHECK(UmiTestExplorerPageCreate(model, 0U, 301U, &second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestExplorerPageCreate(NULL, 0U, 1U, &second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestExplorerPageCreate(model, 0U, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memset(&row, 0, sizeof row); strcpy(row.item.id, "unchanged");
        CHECK(UmiTestExplorerPageRowAt(page, SIZE_MAX, &row) == UMI_STATUS_NOT_FOUND && strcmp(row.item.id, "unchanged") == 0);
        CHECK(UmiTestExplorerPageRead(NULL, &s) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestExplorerPageValidate(page, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
done:
    UmiTestExplorerPageDestroy(second); UmiTestExplorerPageDestroy(page);
    umi_test_explorer_model_destroy(other); umi_test_explorer_model_destroy(model);
    umi_test_platform_result_registry_destroy(results); umi_test_platform_item_registry_destroy(items);
    return failed;
}
