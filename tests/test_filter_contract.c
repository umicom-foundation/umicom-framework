/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_filter_contract.c
 * PURPOSE: Verify shared test filters, exact outcome policy and complete selection publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
static void Item(UmiTestPlatformItemSnapshot *item, size_t index)
{
    memset(item, 0, sizeof *item);
    snprintf(item->id, sizeof item->id, "test.%04zu", index);
    strcpy(item->kind, "test"); strcpy(item->name, "Alpha Parser");
    strcpy(item->labels, "unit;FAST"); strcpy(item->suite_id, "suite");
    item->enabled = 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"arguments", "text-bound", "suite-bound", "label-bound", "item-id-bound", "item-kind-bound", "item-name-bound", "item-suite-bound", "item-labels-bound", "disabled", "case-insensitive", "suite-exact", "failed", "timed-out", "cancelled", "skipped", "not-run", "exact-outcome", "selection-empty", "selection-order", "selection-limit", "selection-exact-limit", "selection-invalid", "selection-no-results", "latest-sequence", "latest-revision", "indexed-history"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(cases[i], argv[1]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    int failed = 0;
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformSelection *selection = calloc(1U, sizeof *selection);
    UmiTestPlatformSelection *before = calloc(1U, sizeof *before);
    UmiTestPlatformFilter filter;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot result = {0};
    CHECK(selection != NULL && before != NULL);
    umi_test_platform_filter_init(&filter); Item(&item, 0U);
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiTestPlatformFilterValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_test_platform_filter_matches(NULL, NULL, &filter));
        CHECK(!umi_test_platform_filter_matches(&item, NULL, NULL));
        CHECK(umi_test_platform_filter_select(NULL, NULL, &filter, selection) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strstr(mode, "-bound") != NULL) {
        if (strcmp(mode, "text-bound") == 0) memset(filter.text, 'x', sizeof filter.text);
        if (strcmp(mode, "suite-bound") == 0) memset(filter.suite_id, 'x', sizeof filter.suite_id);
        if (strcmp(mode, "label-bound") == 0) memset(filter.label, 'x', sizeof filter.label);
        if (strcmp(mode, "item-id-bound") == 0) memset(item.id, 'x', sizeof item.id);
        if (strcmp(mode, "item-kind-bound") == 0) memset(item.kind, 'x', sizeof item.kind);
        if (strcmp(mode, "item-name-bound") == 0) memset(item.name, 'x', sizeof item.name);
        if (strcmp(mode, "item-suite-bound") == 0) memset(item.suite_id, 'x', sizeof item.suite_id);
        if (strcmp(mode, "item-labels-bound") == 0) memset(item.labels, 'x', sizeof item.labels);
        CHECK(!umi_test_platform_filter_matches(&item, &result, &filter));
        CHECK(UmiTestPlatformFilterValidate(&filter) ==
            (strncmp(mode, "item-", 5U) == 0 ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
    } else if (strcmp(mode, "disabled") == 0) {
        item.enabled = 0;
        CHECK(!umi_test_platform_filter_matches(&item, NULL, &filter));
        filter.include_disabled = 1;
        CHECK(umi_test_platform_filter_matches(&item, NULL, &filter));
    } else if (strcmp(mode, "case-insensitive") == 0) {
        strcpy(filter.text, "PARSER"); strcpy(filter.label, "fast");
        CHECK(umi_test_platform_filter_matches(&item, NULL, &filter));
        strcpy(filter.text, "absent");
        CHECK(!umi_test_platform_filter_matches(&item, NULL, &filter));
    } else if (strcmp(mode, "suite-exact") == 0) {
        strcpy(filter.suite_id, "Suite");
        CHECK(!umi_test_platform_filter_matches(&item, NULL, &filter));
        strcpy(filter.suite_id, "suite");
        CHECK(umi_test_platform_filter_matches(&item, NULL, &filter));
    } else if (strcmp(mode, "indexed-history") == 0) {
        CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
        CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 1030U; ++i) {
            Item(&item, i);
            CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
        }
        for (size_t i = 0U; i < UMI_TEST_PLATFORM_RESULT_CAPACITY; ++i) {
            const size_t test = i % 1030U;
            snprintf(result.id, sizeof result.id, "result.%04zu", i);
            snprintf(result.item_id, sizeof result.item_id, "test.%04zu", test);
            result.sequence = i / 1030U;
            result.outcome = test == 1029U ? UMI_TEST_PLATFORM_OUTCOME_FAILED :
                test == 1028U ? UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT :
                test == 1027U ? UMI_TEST_PLATFORM_OUTCOME_SKIPPED :
                i < 1030U ? UMI_TEST_PLATFORM_OUTCOME_FAILED : UMI_TEST_PLATFORM_OUTCOME_PASSED;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
        filter.failed_only = 1;
        CHECK(umi_test_platform_filter_select(items, results, &filter, selection) == UMI_STATUS_OK);
        CHECK(selection->count == 2U);
        CHECK(strcmp(selection->item_ids[0], "test.1028") == 0 && strcmp(selection->item_ids[1], "test.1029") == 0);
        CHECK(selection->source_revision == umi_test_platform_item_registry_revision(items));
    } else if (strncmp(mode, "selection-", 10U) == 0 || strncmp(mode, "latest-", 7U) == 0) {
        CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
        CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
        size_t count = strcmp(mode, "selection-empty") == 0 ? 0U : 3U;
        if (strcmp(mode, "selection-limit") == 0) count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
        if (strcmp(mode, "selection-exact-limit") == 0) count = UMI_TEST_PLATFORM_SELECTION_CAPACITY;
        for (size_t i = 0U; i < count; ++i) {
            Item(&item, count - i);
            CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
        }
        selection->count = 17U; strcpy(selection->item_ids[0], "retained"); *before = *selection;
        if (strcmp(mode, "selection-invalid") == 0) memset(filter.text, 'x', sizeof filter.text);
        if (strncmp(mode, "latest-", 7U) == 0) {
            filter.failed_only = 1;
            strcpy(result.id, "older"); strcpy(result.item_id, "test.0003");
            result.sequence = strcmp(mode, "latest-sequence") == 0 ? 50U : 1U;
            result.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
            strcpy(result.id, "newer"); result.sequence = 1U; result.outcome = UMI_TEST_PLATFORM_OUTCOME_PASSED;
            CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
        }
        const UmiStatus status = umi_test_platform_filter_select(items,
            strcmp(mode, "selection-no-results") == 0 ? NULL : results, &filter, selection);
        if (strcmp(mode, "selection-limit") == 0 || strcmp(mode, "selection-invalid") == 0) {
            CHECK(status == (strcmp(mode, "selection-limit") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT));
            CHECK(memcmp(selection, before, sizeof *selection) == 0);
        } else {
            CHECK(status == UMI_STATUS_OK);
            const size_t expected = strcmp(mode, "latest-sequence") == 0 ? 1U :
                strcmp(mode, "latest-revision") == 0 ? 0U : count;
            CHECK(selection->count == expected);
            CHECK(selection->source_revision == umi_test_platform_item_registry_revision(items));
            if (expected > 0U) {
                char first[128]; snprintf(first, sizeof first, "test.%04zu", count);
                CHECK(strcmp(selection->item_ids[0], first) == 0);
            }
            if (strcmp(mode, "selection-order") == 0) CHECK(strcmp(selection->item_ids[2], "test.0001") == 0);
        }
    } else {
        filter.failed_only = 1;
        if (strcmp(mode, "timed-out") == 0 || strcmp(mode, "exact-outcome") == 0) result.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
        if (strcmp(mode, "cancelled") == 0) result.outcome = UMI_TEST_PLATFORM_OUTCOME_CANCELLED;
        if (strcmp(mode, "skipped") == 0) result.outcome = UMI_TEST_PLATFORM_OUTCOME_SKIPPED;
        const int expected = strcmp(mode, "failed") == 0 || strcmp(mode, "timed-out") == 0 || strcmp(mode, "exact-outcome") == 0;
        CHECK(umi_test_platform_filter_matches(&item, strcmp(mode, "not-run") == 0 ? NULL : &result, &filter) == expected);
        if (strcmp(mode, "exact-outcome") == 0) {
            filter.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
            CHECK(!umi_test_platform_filter_matches(&item, &result, &filter));
            filter.outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
            CHECK(umi_test_platform_filter_matches(&item, &result, &filter));
        }
    }
done:
    free(selection); free(before);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return failed;
}
