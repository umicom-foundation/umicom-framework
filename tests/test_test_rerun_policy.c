/*-----------------------------------------------------------------------------
 * Umicom Framework professional rerun policy tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/rerun_policy.h"

static UmiTestPlatformItemSnapshot make_item(const char *id)
{
    UmiTestPlatformItemSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.name, id, sizeof(value.name) - 1U);
    value.enabled = 1;
    return value;
}

static UmiTestPlatformResultSnapshot make_result(const char *id,
    const char *item_id, int outcome, uint64_t sequence, int flaky)
{
    UmiTestPlatformResultSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.item_id, item_id, sizeof(value.item_id) - 1U);
    value.outcome = outcome;
    value.sequence = sequence;
    value.flaky = flaky;
    value.duration_ms = 1500.0;
    return value;
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestRerunPlan *plan = NULL;
    UmiTestRerunPolicy policy;
    UmiTestRerunPlanSnapshot snapshot;
    UmiTestPlatformOperationPlan operation;
    UmiTestPlatformItemSnapshot a = make_item("a");
    UmiTestPlatformItemSnapshot b = make_item("b");
    UmiTestPlatformResultSnapshot a1 = make_result("a1", "a",
        UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U, 0);
    UmiTestPlatformResultSnapshot a2 = make_result("a2", "a",
        UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U, 1);
    UmiTestPlatformResultSnapshot b1 = make_result("b1", "b",
        UMI_TEST_PLATFORM_OUTCOME_PASSED, 3U, 0);
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &a) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &b) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &a1) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &a2) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &b1) == UMI_STATUS_OK);
    umi_test_rerun_policy_init(&policy);
    policy.reason_mask |= UMI_TEST_RERUN_REASON_SLOW;
    policy.repeat_count = 3U;
    assert(umi_test_rerun_plan_create(&plan) == UMI_STATUS_OK);
    assert(umi_test_rerun_plan_build(plan, items, results, &policy) == UMI_STATUS_OK);
    assert(umi_test_rerun_plan_snapshot(plan, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.candidate_count == 2U && snapshot.failed_count == 1U);
    assert(umi_test_rerun_plan_operation(plan, &policy, &operation) == UMI_STATUS_OK);
    assert(operation.repeat_count == 3U && operation.selection.count == 2U);
    umi_test_rerun_plan_destroy(plan);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
