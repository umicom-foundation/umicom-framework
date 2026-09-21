/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_json_registry.c
 * PURPOSE: Check dynamic index growth, removal, cloning and scope isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "json_test_support.h"
int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL, *clone = NULL;
    UmiTestPlatformItemSnapshot item = {0}, found;
    REQUIRE(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 10000U; ++i) {
        REQUIRE(snprintf(item.id, sizeof(item.id), "test.%zu", i) > 0);
        REQUIRE(snprintf(item.name, sizeof(item.name), "record %zu", i) > 0);
        strcpy(item.suite_id, i % 2U == 0U ? "notes" : "bank");
        REQUIRE(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    REQUIRE(umi_test_platform_item_registry_count(items) == 10000U);
    REQUIRE(UmiTestPlatformItemRegistryClone(items, &clone) == UMI_STATUS_OK);
    REQUIRE(UmiTestPlatformItemRegistryRemoveSuite(items, "notes") == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_item_registry_count(items) == 5000U);
    for (size_t i = 0U; i < 10000U; ++i) {
        REQUIRE(snprintf(item.id, sizeof(item.id), "test.%zu", i) > 0);
        REQUIRE(umi_test_platform_item_registry_find(items, item.id, &found) ==
            (i % 2U == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
        REQUIRE(umi_test_platform_item_registry_find(clone, item.id, &found) == UMI_STATUS_OK);
    }
    UmiTestPlatformItemRegistrySwap(items, clone);
    REQUIRE(umi_test_platform_item_registry_count(items) == 10000U);
    REQUIRE(umi_test_platform_item_registry_remove(items, "test.17") == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_item_registry_find(items, "test.18", &found) == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_item_registry_find(items, "test.17", &found) == UMI_STATUS_NOT_FOUND);
    uint64_t before = umi_test_platform_item_registry_revision(items);
    memset(item.id, 'x', sizeof(item.id));
    REQUIRE(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_test_platform_item_registry_revision(items) == before);
    umi_test_platform_item_registry_clear(items);
    REQUIRE(umi_test_platform_item_registry_count(items) == 0U);
    REQUIRE(umi_test_platform_item_registry_find(items, "test.18", &found) == UMI_STATUS_NOT_FOUND);
    strcpy(item.id, "after.clear");
    REQUIRE(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_item_registry_find(items, "after.clear", &found) == UMI_STATUS_OK);
    umi_test_platform_item_registry_destroy(items); umi_test_platform_item_registry_destroy(clone);
    puts("10,000 indexed records: growth, clone, single removal, suite removal, swap and clear passed.");
    return 0;
}
