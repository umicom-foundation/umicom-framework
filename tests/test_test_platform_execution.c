/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_execution.c
 *
 * PURPOSE:
 *   Verify provider callbacks, repeats, result retention and stop-on-failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

static UmiStatus execute_fake(const UmiTestPlatformItemSnapshot *item,
                              uint32_t attempt,
                              void *user_data,
                              UmiTestPlatformResultSnapshot *result)
{
    int *calls = (int *)user_data;
    *calls += 1;
    /* Cap the copied id so the attempt suffix always fits the result id. */
    (void)snprintf(result->id, sizeof(result->id), "%.100s.%u", item->id,
                   (unsigned)attempt);
    result->outcome = strcmp(item->id, "beta") == 0
                          ? UMI_TEST_PLATFORM_OUTCOME_FAILED
                          : UMI_TEST_PLATFORM_OUTCOME_PASSED;
    result->duration_ms = 5.0;
    return UMI_STATUS_OK;
}

static void add_item(UmiTestPlatformItemRegistry *items, const char *id)
{
    UmiTestPlatformItemSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.id, id);
    (void)strcpy(item.name, id);
    (void)strcpy(item.kind, "test");
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(items, &item) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformOperationController controller;
    UmiTestPlatformExecutionSummary summary;
    int calls = 0;
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    add_item(items, "alpha");
    add_item(items, "beta");
    umi_test_platform_operation_plan_init(
        &plan, UMI_TEST_PLATFORM_OPERATION_REPEAT);
    assert(umi_test_platform_operation_plan_add(&plan, "alpha") ==
           UMI_STATUS_OK);
    assert(umi_test_platform_operation_plan_add(&plan, "beta") ==
           UMI_STATUS_OK);
    plan.kind = UMI_TEST_PLATFORM_OPERATION_REPEAT;
    plan.repeat_count = 3U;
    plan.stop_on_failure = 1;
    umi_test_platform_operation_controller_init(&controller);
    assert(umi_test_platform_execute(items, results, &plan, &controller,
                                    execute_fake, &calls, &summary) ==
           UMI_STATUS_OK);
    assert(summary.planned == 6U);
    assert(summary.executed == 2U);
    assert(summary.passed == 1U);
    assert(summary.failed == 1U);
    assert(summary.stopped == 1);
    assert(calls == 2);
    assert(umi_test_platform_result_registry_count(results) == 2U);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
