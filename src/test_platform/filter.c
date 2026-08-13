/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/filter.c
 *
 * PURPOSE:
 *   Evaluate Test Explorer filters and build stable bounded selections.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/filter.h"

#include <ctype.h>
#include <string.h>

static int text_contains_case_insensitive(const char *text, const char *needle)
{
    const char *candidate;
    if (needle == NULL || needle[0] == '\0') return 1;
    if (text == NULL) return 0;
    for (candidate = text; *candidate != '\0'; ++candidate) {
        const char *left = candidate;
        const char *right = needle;
        while (*left != '\0' && *right != '\0' &&
               tolower((unsigned char)*left) ==
                   tolower((unsigned char)*right)) {
            ++left;
            ++right;
        }
        if (*right == '\0') return 1;
    }
    return 0;
}

static int is_test_item(const UmiTestPlatformItemSnapshot *item)
{
    return item != NULL &&
           (strcmp(item->kind, "test") == 0 ||
            strcmp(item->kind, "case") == 0);
}

static int latest_result_for_item(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *out_result)
{
    UmiTestPlatformResultSnapshot candidate;
    uint64_t best_sequence = 0U;
    uint64_t best_revision = 0U;
    size_t index;
    int found = 0;
    if (results == NULL || item_id == NULL || out_result == NULL) return 0;
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        if (umi_test_platform_result_registry_at(results, index, &candidate) !=
            UMI_STATUS_OK) {
            continue;
        }
        if (strcmp(candidate.item_id, item_id) == 0 &&
            (!found || candidate.sequence > best_sequence ||
             (candidate.sequence == best_sequence &&
              candidate.revision > best_revision))) {
            *out_result = candidate;
            best_sequence = candidate.sequence;
            best_revision = candidate.revision;
            found = 1;
        }
    }
    return found;
}

void umi_test_platform_filter_init(UmiTestPlatformFilter *filter)
{
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->outcome = -1;
}

int umi_test_platform_filter_matches(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *latest_result,
    const UmiTestPlatformFilter *filter)
{
    if (item == NULL || filter == NULL || !is_test_item(item)) return 0;
    if (!filter->include_disabled && !item->enabled) return 0;
    if (filter->suite_id[0] != '\0' &&
        strcmp(item->suite_id, filter->suite_id) != 0) {
        return 0;
    }
    if (filter->label[0] != '\0' &&
        !text_contains_case_insensitive(item->labels, filter->label)) {
        return 0;
    }
    if (filter->text[0] != '\0' &&
        !text_contains_case_insensitive(item->id, filter->text) &&
        !text_contains_case_insensitive(item->name, filter->text) &&
        !text_contains_case_insensitive(item->labels, filter->text)) {
        return 0;
    }
    if (filter->failed_only &&
        (latest_result == NULL ||
         latest_result->outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED)) {
        return 0;
    }
    if (filter->outcome >= 0 &&
        (latest_result == NULL || latest_result->outcome != filter->outcome)) {
        return 0;
    }
    return 1;
}

UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection)
{
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot latest;
    size_t index;
    if (items == NULL || filter == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    out_selection->source_revision =
        umi_test_platform_item_registry_revision(items);
    for (index = 0U; index < umi_test_platform_item_registry_count(items);
         ++index) {
        const UmiTestPlatformResultSnapshot *latest_pointer = NULL;
        if (umi_test_platform_item_registry_at(items, index, &item) !=
            UMI_STATUS_OK) {
            continue;
        }
        if (latest_result_for_item(results, item.id, &latest)) {
            latest_pointer = &latest;
        }
        if (!umi_test_platform_filter_matches(&item, latest_pointer, filter)) {
            continue;
        }
        if (out_selection->count >= UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)strncpy(out_selection->item_ids[out_selection->count],
                      item.id,
                      sizeof(out_selection->item_ids[0]) - 1U);
        out_selection->item_ids[out_selection->count]
                               [sizeof(out_selection->item_ids[0]) - 1U] = '\0';
        out_selection->count += 1U;
    }
    return UMI_STATUS_OK;
}
