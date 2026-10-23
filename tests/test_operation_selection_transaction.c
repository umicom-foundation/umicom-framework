/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operation_selection_transaction.c
 * PURPOSE: Verify selected-test plans publish only complete registry-backed selections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/operation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"order", "missing-later", "duplicate-later", "null-later", "empty-id", "long-id", "over-capacity", "empty-request", "generation-full", "generation-near-full", "alias", "repeat", "invalid-prior"};
    const char *mode = argv[1]; int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(mode, cases[i]) == 0) known = 1;
    if (!known) return 2;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformOperationPlan *plan = malloc(sizeof *plan), *before = malloc(sizeof *before);
    CHECK(plan != NULL && before != NULL);
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 3U; ++i) {
        UmiTestPlatformItemSnapshot item = {0};
        snprintf(item.id, sizeof item.id, "item.%zu", i); strcpy(item.kind, "test"); item.enabled = 1;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    }
    umi_test_platform_operation_plan_init(plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    CHECK(umi_test_platform_operation_plan_add(plan, "item.1") == UMI_STATUS_OK);
    const char *ids[] = {"item.2", "item.0"}; size_t count = 2U;
    char long_id[129]; memset(long_id, 'x', sizeof long_id - 1U); long_id[128] = 0;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "missing-later") == 0) { ids[1] = "missing"; expected = UMI_STATUS_NOT_FOUND; }
    if (strcmp(mode, "duplicate-later") == 0) { ids[1] = ids[0]; expected = UMI_STATUS_ALREADY_EXISTS; }
    if (strcmp(mode, "null-later") == 0) { ids[1] = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (strcmp(mode, "empty-id") == 0) { ids[1] = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (strcmp(mode, "long-id") == 0) { ids[1] = long_id; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(mode, "over-capacity") == 0) { count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(mode, "empty-request") == 0) { count = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (strcmp(mode, "generation-full") == 0) { plan->generation = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(mode, "generation-near-full") == 0) plan->generation = UINT64_MAX - 1U;
    if (strcmp(mode, "alias") == 0) ids[1] = plan->selection.item_ids[0];
    if (strcmp(mode, "repeat") == 0) { plan->repeat_count = 5U; plan->stop_on_failure = 1; }
    if (strcmp(mode, "invalid-prior") == 0) { plan->kind = UMI_TEST_PLATFORM_OPERATION_REPEAT + 1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    *before = *plan;
    CHECK(UmiTestPlatformOperationPlanSelected(plan, items, ids, count) == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(plan, before, sizeof *plan) == 0);
    else {
        CHECK(plan->selection.count == 2U && plan->kind == UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
        CHECK(strcmp(plan->selection.item_ids[0], "item.2") == 0);
        CHECK(strcmp(plan->selection.item_ids[1], strcmp(mode, "alias") == 0 ? "item.1" : "item.0") == 0);
        CHECK(plan->generation == before->generation + 1U);
        CHECK(plan->selection.source_revision == umi_test_platform_item_registry_revision(items));
        CHECK(plan->repeat_count == before->repeat_count && plan->stop_on_failure == before->stop_on_failure);
    }
done:
    free(plan); free(before); umi_test_platform_item_registry_destroy(items);
    return failed;
}
