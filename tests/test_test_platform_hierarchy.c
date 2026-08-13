/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_hierarchy.c
 *
 * PURPOSE:
 *   Verify parent/child hierarchy and aggregate descendant failure counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

static void add_item(UmiTestPlatformItemRegistry *items,
                     const char *id,
                     const char *parent,
                     const char *kind,
                     int enabled)
{
    UmiTestPlatformItemSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.id, id);
    (void)strcpy(item.parent_id, parent);
    (void)strcpy(item.name, id);
    (void)strcpy(item.kind, kind);
    item.enabled = enabled;
    assert(umi_test_platform_item_registry_upsert(items, &item) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformResultSnapshot result;
    UmiTestPlatformHierarchyNode nodes[4];
    size_t count = 0U;

    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    add_item(items, "suite", "", "suite", 1);
    add_item(items, "group", "suite", "group", 1);
    add_item(items, "alpha", "group", "test", 1);
    add_item(items, "beta", "group", "test", 0);
    (void)memset(&result, 0, sizeof(result));
    (void)strcpy(result.id, "result.alpha");
    (void)strcpy(result.item_id, "alpha");
    result.sequence = 1U;
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    assert(umi_test_platform_result_registry_upsert(results, &result) ==
           UMI_STATUS_OK);
    assert(umi_test_platform_hierarchy_build(items, results, nodes, 4U,
                                             &count) == UMI_STATUS_OK);
    assert(count == 4U);
    assert(nodes[0].depth == 0U);
    assert(nodes[0].child_count == 1U);
    assert(nodes[0].descendant_test_count == 2U);
    assert(nodes[0].enabled_test_count == 1U);
    assert(nodes[0].failed_test_count == 1U);
    assert(nodes[1].depth == 1U);
    assert(nodes[2].depth == 2U);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
