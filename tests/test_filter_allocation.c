/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_filter_allocation.c
 * PURPOSE: Verify filter selection keeps caller output unchanged when staging allocation fails.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/filter.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int reject_allocation;
/* Exercise both selection staging and result-index allocation independently. Retained for engineering review. */
#if 0
static void *FilterTestCalloc(size_t count, size_t size)
{
    return reject_allocation ? NULL : calloc(count, size);
}
#endif

static size_t allocation_calls, reject_call;
static void *FilterTestCalloc(size_t count, size_t size)
{
    ++allocation_calls;
    return reject_allocation || (reject_call != 0U && allocation_calls == reject_call)
        ? NULL : calloc(count, size);
}
/* Allocation interception is confined to this production-source fixture.
 * The product API remains independent of test-only global controls. */
#define umi_test_platform_filter_init FilterTestInit
#define UmiTestPlatformFilterValidate FilterTestValidate
#define umi_test_platform_filter_matches FilterTestMatches
#define umi_test_platform_filter_select FilterTestSelect
#define UmiTestPlatformFilterInspect FilterTestInspect
#define UmiTestPlatformFilterItemAt FilterTestItemAt
#define calloc FilterTestCalloc
#include "../src/test_platform/filter.c"
#undef calloc
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
int main(void)
{
    int failed = 0;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformSelection *selection = calloc(1U, sizeof *selection);
    UmiTestPlatformSelection *before = calloc(1U, sizeof *before);
    UmiTestPlatformFilter filter;
    umi_test_platform_filter_init(&filter);
    CHECK(selection != NULL && before != NULL);
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    UmiTestPlatformItemSnapshot item = {0};
    strcpy(item.id, "kept"); strcpy(item.kind, "test"); item.enabled = 1;
    CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    selection->count = 19U; strcpy(selection->item_ids[0], "previous"); *before = *selection;
    reject_allocation = 1;
    CHECK(umi_test_platform_filter_select(items, NULL, &filter, selection) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(memcmp(selection, before, sizeof *selection) == 0);
    reject_allocation = 0;
    CHECK(umi_test_platform_filter_select(items, NULL, &filter, selection) == UMI_STATUS_OK);
    CHECK(selection->count == 1U && strcmp(selection->item_ids[0], "kept") == 0);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    UmiTestPlatformResultSnapshot result = {0};
    strcpy(result.id, "latest"); strcpy(result.item_id, "kept"); result.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
    CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
    *before = *selection;
    filter.failed_only = 1; allocation_calls = 0U; reject_call = 2U;
    CHECK(umi_test_platform_filter_select(items, results, &filter, selection) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(allocation_calls == 2U && memcmp(selection, before, sizeof *selection) == 0);
    reject_call = 0U;
    CHECK(umi_test_platform_filter_select(items, results, &filter, selection) == UMI_STATUS_OK);
    CHECK(selection->count == 1U && strcmp(selection->item_ids[0], "kept") == 0);
    UmiTestPlatformFilterSummary summary = {0}, saved;
    summary.matched_count = 21U; saved = summary;
    reject_allocation = 1;
    CHECK(UmiTestPlatformFilterInspect(items, results, &filter, "kept", &summary) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(memcmp(&summary, &saved, sizeof summary) == 0);
    reject_allocation = 0;
    CHECK(UmiTestPlatformFilterInspect(items, results, &filter, "kept", &summary) == UMI_STATUS_OK);
    CHECK(summary.matched_count == 1U && summary.timed_out_count == 1U && summary.selected_matches);
done:
    free(selection); free(before); umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return failed;
}
