/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/filter_inspection.inc
 * PURPOSE: Inspect full test catalogues through compact indexed outcome evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included by filter.c. Inspection needs only outcome identity and ordering;
 * full output buffers remain in the canonical result registry. */
typedef struct FilterOutcomeRecord {
    char item_id[128];
    uint64_t sequence, revision;
    size_t source_index;
    int outcome;
} FilterOutcomeRecord;
static int FilterOutcomeCompare(const void *a, const void *b)
{
    const FilterOutcomeRecord *left = a, *right = b;
    const int item = strcmp(left->item_id, right->item_id);
    if (item != 0) return item;
    if (left->sequence != right->sequence) return left->sequence > right->sequence ? -1 : 1;
    if (left->revision != right->revision) return left->revision > right->revision ? -1 : 1;
    return left->source_index < right->source_index ? -1 : left->source_index > right->source_index;
}
static const FilterOutcomeRecord *FilterOutcomeFind(const FilterOutcomeRecord *records,
    size_t count, const char *id)
{
    size_t low = 0U, high = count;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        if (strcmp(records[middle].item_id, id) < 0) low = middle + 1U;
        else high = middle;
    }
    return low < count && strcmp(records[low].item_id, id) == 0 ? &records[low] : NULL;
}
/* The shared inspection capture now also serves visible-item lookup without exporting an execution selection. Retained for engineering review. */
#if 0
UmiStatus UmiTestPlatformFilterInspect(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected_id, UmiTestPlatformFilterSummary *out_summary)
{
    if (items == NULL || out_summary == NULL || UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestPlatformFilterSummary next = {0};
    next.item_count = umi_test_platform_item_registry_count(items);
    next.item_revision = umi_test_platform_item_registry_revision(items);
    next.result_revision = umi_test_platform_result_registry_revision(results);
    const size_t result_count = umi_test_platform_result_registry_count(results);
    if (next.item_count > UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY ||
        result_count > UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    FilterOutcomeRecord *records = result_count == 0U ? NULL : calloc(result_count, sizeof *records);
    if (result_count != 0U && records == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < result_count; ++i) {
        UmiTestPlatformResultSnapshot result;
        status = umi_test_platform_result_registry_at(results, i, &result);
        if (status != UMI_STATUS_OK) goto done;
        memcpy(records[i].item_id, result.item_id, sizeof records[i].item_id);
        records[i].sequence = result.sequence; records[i].revision = result.revision;
        records[i].source_index = i; records[i].outcome = result.outcome;
    }
    if (result_count > 1U) qsort(records, result_count, sizeof *records, FilterOutcomeCompare);
    for (size_t i = 0U; i < next.item_count; ++i) {
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_at(items, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        const FilterOutcomeRecord *latest = FilterOutcomeFind(records, result_count, item.id);
        UmiTestPlatformResultSnapshot result = {0};
        if (latest != NULL) result.outcome = latest->outcome;
        if (!umi_test_platform_filter_matches(&item, latest != NULL ? &result : NULL, filter)) continue;
        if (next.matched_count == 0U) memcpy(next.first_item_id, item.id, strlen(item.id) + 1U);
        ++next.matched_count;
        if (selected_id != NULL && strcmp(selected_id, item.id) == 0) next.selected_matches = 1;
        switch (latest != NULL ? (UmiTestPlatformOutcome)latest->outcome : UMI_TEST_PLATFORM_OUTCOME_NOT_RUN) {
            case UMI_TEST_PLATFORM_OUTCOME_PASSED: ++next.passed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_FAILED: ++next.failed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: ++next.skipped_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: ++next.cancelled_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: ++next.timed_out_count; break;
            default: ++next.not_run_count; break;
        }
    }
    if (next.item_revision != umi_test_platform_item_registry_revision(items) ||
        next.result_revision != umi_test_platform_result_registry_revision(results))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *out_summary = next;
done:
    free(records);
    return status;
}
#endif
/* Extend indexed capture to bounded ID selection so all shared consumers use identical result joins. Retained for engineering review. */
#if 0
static UmiStatus FilterInspectCapture(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected_id, UmiTestPlatformFilterSummary *out_summary,
    size_t visible_index, UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest)
{
    if (items == NULL || out_summary == NULL || UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestPlatformFilterSummary next = {0};
    UmiTestPlatformItemSnapshot selected = {0};
    UmiTestPlatformResultSnapshot selected_result = {0};
    int found = 0;
    next.item_count = umi_test_platform_item_registry_count(items);
    next.item_revision = umi_test_platform_item_registry_revision(items);
    next.result_revision = umi_test_platform_result_registry_revision(results);
    const size_t result_count = umi_test_platform_result_registry_count(results);
    if (next.item_count > UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY ||
        result_count > UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    FilterOutcomeRecord *records = result_count == 0U ? NULL : calloc(result_count, sizeof *records);
    if (result_count != 0U && records == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < result_count; ++i) {
        UmiTestPlatformResultSnapshot result;
        status = umi_test_platform_result_registry_at(results, i, &result);
        if (status != UMI_STATUS_OK) goto done;
        memcpy(records[i].item_id, result.item_id, sizeof records[i].item_id);
        records[i].sequence = result.sequence; records[i].revision = result.revision;
        records[i].source_index = i; records[i].outcome = result.outcome;
    }
    if (result_count > 1U) qsort(records, result_count, sizeof *records, FilterOutcomeCompare);
    for (size_t i = 0U; i < next.item_count; ++i) {
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_at(items, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        const FilterOutcomeRecord *latest = FilterOutcomeFind(records, result_count, item.id);
        UmiTestPlatformResultSnapshot result = {0};
        if (latest != NULL) result.outcome = latest->outcome;
        if (!umi_test_platform_filter_matches(&item, latest != NULL ? &result : NULL, filter)) continue;
        if (next.matched_count == 0U) memcpy(next.first_item_id, item.id, strlen(item.id) + 1U);
        if (out_item != NULL && next.matched_count == visible_index) {
            selected = item; found = 1;
            if (latest != NULL) {
                status = umi_test_platform_result_registry_at(results, latest->source_index, &selected_result);
                if (status != UMI_STATUS_OK) goto done;
            }
        }
        ++next.matched_count;
        if (selected_id != NULL && strcmp(selected_id, item.id) == 0) next.selected_matches = 1;
        switch (latest != NULL ? (UmiTestPlatformOutcome)latest->outcome : UMI_TEST_PLATFORM_OUTCOME_NOT_RUN) {
            case UMI_TEST_PLATFORM_OUTCOME_PASSED: ++next.passed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_FAILED: ++next.failed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: ++next.skipped_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: ++next.cancelled_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: ++next.timed_out_count; break;
            default: ++next.not_run_count; break;
        }
    }
    if (next.item_revision != umi_test_platform_item_registry_revision(items) ||
        next.result_revision != umi_test_platform_result_registry_revision(results))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && out_item != NULL && !found) status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK) {
        *out_summary = next;
        if (out_item != NULL) *out_item = selected;
        if (out_latest != NULL) *out_latest = selected_result;
    }
done:
    free(records);
    return status;
}
#endif

static UmiStatus FilterInspectCapture(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected_id, UmiTestPlatformFilterSummary *out_summary,
    size_t visible_index, UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest, UmiTestPlatformSelection *out_selection)
{
    if (items == NULL || out_summary == NULL || UmiTestPlatformFilterValidate(filter) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestPlatformFilterSummary next = {0};
    UmiTestPlatformItemSnapshot selected = {0};
    UmiTestPlatformResultSnapshot selected_result = {0};
    int found = 0;
    next.item_count = umi_test_platform_item_registry_count(items);
    next.item_revision = umi_test_platform_item_registry_revision(items);
    next.result_revision = umi_test_platform_result_registry_revision(results);
    /* Text-only execution selection does not consume history. Inspection
     * still indexes it because its public summary reports exact outcomes. */
    const int need_outcomes = out_selection == NULL || filter->failed_only || filter->outcome >= 0;
    const size_t result_count = need_outcomes ? umi_test_platform_result_registry_count(results) : 0U;
    if (out_selection != NULL) out_selection->source_revision = next.item_revision;
    if (next.item_count > UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY ||
        result_count > UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    FilterOutcomeRecord *records = result_count == 0U ? NULL : calloc(result_count, sizeof *records);
    if (result_count != 0U && records == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < result_count; ++i) {
        UmiTestPlatformResultSnapshot result;
        status = umi_test_platform_result_registry_at(results, i, &result);
        if (status != UMI_STATUS_OK) goto done;
        memcpy(records[i].item_id, result.item_id, sizeof records[i].item_id);
        records[i].sequence = result.sequence; records[i].revision = result.revision;
        records[i].source_index = i; records[i].outcome = result.outcome;
    }
    if (result_count > 1U) qsort(records, result_count, sizeof *records, FilterOutcomeCompare);
    for (size_t i = 0U; i < next.item_count; ++i) {
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_at(items, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        const FilterOutcomeRecord *latest = FilterOutcomeFind(records, result_count, item.id);
        UmiTestPlatformResultSnapshot result = {0};
        if (latest != NULL) result.outcome = latest->outcome;
        if (!umi_test_platform_filter_matches(&item, latest != NULL ? &result : NULL, filter)) continue;
        if (out_selection != NULL) {
            if (out_selection->count == UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto done;
            }
            memcpy(out_selection->item_ids[out_selection->count++], item.id, strlen(item.id) + 1U);
        }
        if (next.matched_count == 0U) memcpy(next.first_item_id, item.id, strlen(item.id) + 1U);
        if (out_item != NULL && next.matched_count == visible_index) {
            selected = item; found = 1;
            if (latest != NULL) {
                status = umi_test_platform_result_registry_at(results, latest->source_index, &selected_result);
                if (status != UMI_STATUS_OK) goto done;
            }
        }
        ++next.matched_count;
        if (selected_id != NULL && strcmp(selected_id, item.id) == 0) next.selected_matches = 1;
        switch (latest != NULL ? (UmiTestPlatformOutcome)latest->outcome : UMI_TEST_PLATFORM_OUTCOME_NOT_RUN) {
            case UMI_TEST_PLATFORM_OUTCOME_PASSED: ++next.passed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_FAILED: ++next.failed_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: ++next.skipped_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: ++next.cancelled_count; break;
            case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: ++next.timed_out_count; break;
            default: ++next.not_run_count; break;
        }
    }
    if (next.item_revision != umi_test_platform_item_registry_revision(items) ||
        next.result_revision != umi_test_platform_result_registry_revision(results))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && out_item != NULL && !found) status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK) {
        *out_summary = next;
        if (out_item != NULL) *out_item = selected;
        if (out_latest != NULL) *out_latest = selected_result;
    }
done:
    free(records);
    return status;
}
UmiStatus UmiTestPlatformFilterInspect(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    const char *selected_id, UmiTestPlatformFilterSummary *out_summary)
{
    return FilterInspectCapture(items, results, filter, selected_id, out_summary, 0U, NULL, NULL, NULL);
}
UmiStatus UmiTestPlatformFilterItemAt(const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results, const UmiTestPlatformFilter *filter,
    size_t visible_index, UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest)
{
    if (out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestPlatformFilterSummary ignored;
    return FilterInspectCapture(items, results, filter, NULL, &ignored, visible_index, out_item, out_latest, NULL);
}

