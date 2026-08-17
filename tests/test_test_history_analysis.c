/*-----------------------------------------------------------------------------
 * Umicom Framework professional test history analysis tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/history_analysis.h"

static UmiTestPlatformResultSnapshot result(const char *id, int outcome,
                                             uint64_t sequence, double duration)
{
    UmiTestPlatformResultSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.item_id, "case", sizeof(value.item_id) - 1U);
    value.outcome = outcome;
    value.sequence = sequence;
    value.duration_ms = duration;
    return value;
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestHistoryAnalysis *analysis = NULL;
    UmiTestHistoryPolicy policy;
    UmiTestHistoryItemAnalysis item_analysis;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot a = result("a", UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U, 10.0);
    UmiTestPlatformResultSnapshot b = result("b", UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U, 15.0);
    UmiTestPlatformResultSnapshot c = result("c", UMI_TEST_PLATFORM_OUTCOME_PASSED, 3U, 20.0);
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strncpy(item.id, "case", sizeof(item.id) - 1U);
    (void)strncpy(item.name, "Case", sizeof(item.name) - 1U);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &a) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &b) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &c) == UMI_STATUS_OK);
    umi_test_history_policy_init(&policy);
    assert(umi_test_history_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_build(analysis, items, results, &policy) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_find(analysis, "case", &item_analysis) == UMI_STATUS_OK);
    assert(item_analysis.stability == UMI_TEST_STABILITY_FLAKY);
    assert(item_analysis.sample_count == 3U && item_analysis.current_failure_streak == 0U);
    umi_test_history_analysis_destroy(analysis);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
