/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_filter_history.c
 *
 * PURPOSE:
 *   Verify explorer filters, rerun-failed selection and duration history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

static void add_test(UmiTestPlatformItemRegistry *items,
                     const char *id,
                     const char *name,
                     const char *labels,
                     int enabled)
{
    UmiTestPlatformItemSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.id, id);
    (void)strcpy(item.name, name);
    (void)strcpy(item.suite_id, "suite.unit");
    (void)strcpy(item.kind, "test");
    (void)strcpy(item.labels, labels);
    item.enabled = enabled;
    assert(umi_test_platform_item_registry_upsert(items, &item) ==
           UMI_STATUS_OK);
}

static void add_result(UmiTestPlatformResultRegistry *results,
                       const char *id,
                       const char *item_id,
                       uint64_t sequence,
                       int outcome,
                       double duration)
{
    UmiTestPlatformResultSnapshot result;
    (void)memset(&result, 0, sizeof(result));
    (void)strcpy(result.id, id);
    (void)strcpy(result.item_id, item_id);
    result.sequence = sequence;
    result.outcome = outcome;
    result.duration_ms = duration;
    assert(umi_test_platform_result_registry_upsert(results, &result) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformFilter filter;
    UmiTestPlatformSelection selection;
    UmiTestPlatformDurationHistory history;
    UmiTestPlatformResultSnapshot latest;

    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    add_test(items, "alpha", "Alpha Parser", "unit;fast", 1);
    add_test(items, "beta", "Beta Network", "integration;slow", 1);
    add_test(items, "gamma", "Gamma Disabled", "unit", 0);
    add_result(results, "alpha.1", "alpha", 1U,
               UMI_TEST_PLATFORM_OUTCOME_PASSED, 10.0);
    add_result(results, "alpha.2", "alpha", 2U,
               UMI_TEST_PLATFORM_OUTCOME_FAILED, 30.0);
    add_result(results, "beta.1", "beta", 1U,
               UMI_TEST_PLATFORM_OUTCOME_PASSED, 50.0);

    umi_test_platform_filter_init(&filter);
    (void)strcpy(filter.label, "unit");
    assert(umi_test_platform_filter_select(items, results, &filter,
                                           &selection) == UMI_STATUS_OK);
    assert(selection.count == 1U);
    assert(strcmp(selection.item_ids[0], "alpha") == 0);
    assert(umi_test_platform_history_failed_selection(items, results,
                                                      &selection) ==
           UMI_STATUS_OK);
    assert(selection.count == 1U);
    assert(strcmp(selection.item_ids[0], "alpha") == 0);
    assert(umi_test_platform_history_latest(results, "alpha", &latest) ==
           UMI_STATUS_OK);
    assert(latest.sequence == 2U);
    assert(umi_test_platform_history_duration(results, "alpha", &history) ==
           UMI_STATUS_OK);
    assert(history.sample_count == 2U);
    assert(history.latest_ms == 30.0);
    assert(history.minimum_ms == 10.0);
    assert(history.maximum_ms == 30.0);
    assert(history.average_ms == 20.0);

    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
