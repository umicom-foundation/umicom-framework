/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_explorer_model.c
 *
 * PURPOSE:
 *   Implement the test test explorer model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework professional Test Explorer model tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/explorer_model.h"

static UmiTestPlatformItemSnapshot item(const char *id, const char *parent,
                                        const char *name)
{
    UmiTestPlatformItemSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.parent_id, parent, sizeof(value.parent_id) - 1U);
    (void)strncpy(value.name, name, sizeof(value.name) - 1U);
    (void)strncpy(value.kind, "test", sizeof(value.kind) - 1U);
    (void)strncpy(value.framework, "ctest", sizeof(value.framework) - 1U);
    (void)strncpy(value.suite_id, "framework", sizeof(value.suite_id) - 1U);
    value.enabled = 1;
    value.discovered = 1;
    return value;
}

static UmiTestPlatformResultSnapshot result(const char *id, const char *item_id,
                                            int outcome, uint64_t sequence)
{
    UmiTestPlatformResultSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.item_id, item_id, sizeof(value.item_id) - 1U);
    value.outcome = outcome;
    value.sequence = sequence;
    return value;
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestExplorerModel *model = NULL;
    UmiTestPlatformFilter filter;
    UmiTestExplorerModelSnapshot snapshot;
    UmiTestExplorerRow row;
    UmiTestPlatformItemSnapshot root = item("root", "", "Suite");
    UmiTestPlatformItemSnapshot child_a = item("a", "root", "Alpha");
    UmiTestPlatformItemSnapshot child_b = item("b", "root", "Beta");
    UmiTestPlatformResultSnapshot passed = result("result.passed", "a",
        UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U);
    UmiTestPlatformResultSnapshot failed = result("result.failed", "b",
        UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U);
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &root) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &child_a) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &child_b) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &passed) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &failed) == UMI_STATUS_OK);
    assert(umi_test_explorer_model_create(&model) == UMI_STATUS_OK);
    umi_test_platform_filter_init(&filter);
    filter.failed_only = 1;
    assert(umi_test_explorer_model_set_filter(model, &filter) == UMI_STATUS_OK);
    assert(umi_test_explorer_model_rebuild(model, items, results) == UMI_STATUS_OK);
    assert(umi_test_explorer_model_snapshot(model, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.row_count == 3U && snapshot.visible_row_count == 2U);
    assert(umi_test_explorer_model_visible_row_at(model, 1U, &row) == UMI_STATUS_OK);
    assert(strcmp(row.item.id, "b") == 0);
    assert(umi_test_explorer_model_select(model, "b", 1, 1) == UMI_STATUS_OK);
    assert(umi_test_explorer_model_collapse_all(model) == UMI_STATUS_OK);
    assert(umi_test_explorer_model_visible_count(model) == 1U);
    umi_test_explorer_model_destroy(model);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
