/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/filter.c
 *
 * PURPOSE:
 *   Evaluate Test Explorer filters and build stable bounded selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/filter.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* Validate at the shared boundary so native Explorer, CLI selection and service
 * callers cannot interpret unterminated fixed-size input as a C string.
 * Negative outcome values retain the existing "any outcome" convention. */
UmiStatus UmiTestPlatformFilterValidate(const UmiTestPlatformFilter *filter)
{
    if (filter == NULL || memchr(filter->text, 0, sizeof filter->text) == NULL ||
        memchr(filter->suite_id, 0, sizeof filter->suite_id) == NULL ||
        memchr(filter->label, 0, sizeof filter->label) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static int FilterItemTextValid(const UmiTestPlatformItemSnapshot *item)
{
    return memchr(item->id, 0, sizeof item->id) != NULL &&
        memchr(item->kind, 0, sizeof item->kind) != NULL &&
        memchr(item->name, 0, sizeof item->name) != NULL &&
        memchr(item->suite_id, 0, sizeof item->suite_id) != NULL &&
        memchr(item->labels, 0, sizeof item->labels) != NULL;
}



/*
 * Provide the text contains case insensitive operation used by this module and its client
 * applications.
 */
static int text_contains_case_insensitive(const char *text, const char *needle)
{
    const char *candidate;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (needle == NULL || needle[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (candidate = text; *candidate != '\0'; ++candidate) {
        const char *left = candidate;
        const char *right = needle;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*left != '\0' && *right != '\0' &&
               tolower((unsigned char)*left) ==
                   tolower((unsigned char)*right)) {
            ++left;
            ++right;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*right == '\0') return 1;
    }
    return 0;
}

/* Provide the is test item operation used by this module and its client applications. */
static int is_test_item(const UmiTestPlatformItemSnapshot *item)
{
    return item != NULL &&
           (strcmp(item->kind, "test") == 0 ||
            strcmp(item->kind, "case") == 0);
}

/*
 * Provide the latest result for item operation used by this module and its client
 * applications.
 */
/* Selection now uses one compact outcome index per capture instead of scanning full result snapshots for every item. Retained for engineering review. */
#if 0
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || item_id == NULL || out_result == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_test_platform_result_registry_at(results, index, &candidate) !=
            UMI_STATUS_OK) {
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
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
#endif


/*
 * Initialise test platform filter from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_platform_filter_init(UmiTestPlatformFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->outcome = -1;
}

/*
 * Provide the test platform filter matches operation used by this module and its client
 * applications.
 */
/* Validate public fixed-size text before matching; Failed only includes timed-out results consistently with the Explorer summary and rerun policy. Retained for engineering review. */
#if 0
int umi_test_platform_filter_matches(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *latest_result,
    const UmiTestPlatformFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || filter == NULL || !is_test_item(item)) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!filter->include_disabled && !item->enabled) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->suite_id[0] != '\0' &&
        strcmp(item->suite_id, filter->suite_id) != 0) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->label[0] != '\0' &&
        !text_contains_case_insensitive(item->labels, filter->label)) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->text[0] != '\0' &&
        !text_contains_case_insensitive(item->id, filter->text) &&
        !text_contains_case_insensitive(item->name, filter->text) &&
        !text_contains_case_insensitive(item->labels, filter->text)) {
        return 0;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (filter->failed_only &&
        (latest_result == NULL ||
         latest_result->outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED)) {
        return 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (filter->outcome >= 0 &&
        (latest_result == NULL || latest_result->outcome != filter->outcome)) {
        return 0;
    }
    return 1;
}
#endif
int umi_test_platform_filter_matches(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *latest_result,
    const UmiTestPlatformFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK ||
        !FilterItemTextValid(item) || !is_test_item(item)) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!filter->include_disabled && !item->enabled) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->suite_id[0] != '\0' &&
        strcmp(item->suite_id, filter->suite_id) != 0) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->label[0] != '\0' &&
        !text_contains_case_insensitive(item->labels, filter->label)) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->text[0] != '\0' &&
        !text_contains_case_insensitive(item->id, filter->text) &&
        !text_contains_case_insensitive(item->name, filter->text) &&
        !text_contains_case_insensitive(item->labels, filter->text)) {
        return 0;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (filter->failed_only &&
        (latest_result == NULL ||
         (latest_result->outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED &&
          latest_result->outcome != UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT))) {
        return 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (filter->outcome >= 0 &&
        (latest_result == NULL || latest_result->outcome != filter->outcome)) {
        return 0;
    }
    return 1;
}

/*
 * Provide the test platform filter select operation used by this module and its client
 * applications.
 */
/* Selection is staged privately and published only when complete; the original partial-output behaviour remains below for review. Retained for engineering review. */
#if 0
UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection)
{
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot latest;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || filter == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    out_selection->source_revision =
        umi_test_platform_item_registry_revision(items);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_test_platform_item_registry_count(items);
         ++index) {
        const UmiTestPlatformResultSnapshot *latest_pointer = NULL;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_test_platform_item_registry_at(items, index, &item) !=
            UMI_STATUS_OK) {
            continue;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (latest_result_for_item(results, item.id, &latest)) {
            latest_pointer = &latest;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_test_platform_filter_matches(&item, latest_pointer, filter)) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
#endif
/* Use the full-catalogue capture index for selection as well as presentation, retaining atomic output and exact result ordering. Retained for engineering review. */
#if 0
UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection)
{
    if (items == NULL || out_selection == NULL ||
        UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A bounded selection is published as one value. If capacity is exceeded,
     * a caller must never receive the first 1,024 IDs as if they were complete.
     * Heap storage also avoids a large automatic frame on Windows. */
    UmiTestPlatformSelection *next = (UmiTestPlatformSelection *)calloc(1U, sizeof *next);
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    next->source_revision = umi_test_platform_item_registry_revision(items);
    const uint64_t result_revision = umi_test_platform_result_registry_revision(results);
    const size_t count = umi_test_platform_item_registry_count(items);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; index < count; ++index) {
        UmiTestPlatformItemSnapshot item;
        UmiTestPlatformResultSnapshot latest;
        status = umi_test_platform_item_registry_at(items, index, &item);
        if (status != UMI_STATUS_OK) break;
        const UmiTestPlatformResultSnapshot *latest_pointer = NULL;
        /* Text-only selection does not need the result history. Keep its cost
         * independent of the number of retained runs in a large catalogue. */
        if ((filter->failed_only || filter->outcome >= 0) &&
            latest_result_for_item(results, item.id, &latest)) latest_pointer = &latest;
        if (!umi_test_platform_filter_matches(&item, latest_pointer, filter)) continue;
        if (next->count == UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        memcpy(next->item_ids[next->count++], item.id, strlen(item.id) + 1U);
    }
    /* The registries remain owner-thread objects. Revision checks reject a
     * changed source identity; they do not make concurrent mutation safe. */
    if (status == UMI_STATUS_OK &&
        (next->source_revision != umi_test_platform_item_registry_revision(items) ||
         result_revision != umi_test_platform_result_registry_revision(results)))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *out_selection = *next;
    free(next);
    return status;
}
#endif

/* Inspection and execution selection share the same indexed capture. */
static UmiStatus FilterInspectCapture(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected_id, UmiTestPlatformFilterSummary *out_summary,
    size_t visible_index, UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest, UmiTestPlatformSelection *out_selection);
UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection)
{
    if (items == NULL || out_selection == NULL ||
        UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Stage bounded IDs on the heap. Indexed capture also stages result joins,
     * so either allocation may fail without exposing a partial selection. */
    UmiTestPlatformSelection *next = calloc(1U, sizeof *next);
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiTestPlatformFilterSummary ignored;
    UmiStatus status = FilterInspectCapture(items, results, filter, NULL,
        &ignored, 0U, NULL, NULL, next);
    if (status == UMI_STATUS_OK) *out_selection = *next;
    free(next);
    return status;
}

/* Full-catalogue inspection shares the matcher while retaining exact outcome counts. */
#include "filter_inspection.inc"
