/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_compare.inc
 * PURPOSE: Keep each comparison consistent with one complete database observation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/archive_compare.h"

typedef struct ArchiveComparisonSide
{
    UmiCtestJobRequest request;
    UmiTestArchiveOutcomes outcomes;
} ArchiveComparisonSide;
typedef struct ArchiveComparisonIndex
{
    size_t before, after;
    UmiTestArchiveChange change;
} ArchiveComparisonIndex;
struct UmiTestArchiveComparison
{
    UmiTestArchiveComparisonSummary summary;
    ArchiveComparisonSide *before, *after;
    ArchiveComparisonIndex *rows;
};

/* Record positions are useful only inside a run. A named test can move when
 * discovery order changes, so compare its execution location and name instead.
 * Exact byte comparison avoids guessing filesystem aliases or case rules. */
static int comparison_key(const void *left, const void *right)
{
    const ArchiveComparisonSide *a = left, *b = right;
    int order = strcmp(a->request.build_directory, b->request.build_directory);
    if (order == 0)
        order = strcmp(a->request.configuration, b->request.configuration);
    if (order == 0)
        order = strcmp(a->request.name, b->request.name);
    return order;
}
static bool comparison_cancelled(const UmiCancellationToken *token)
{
    return umi_cancellation_token_is_requested(token);
}
static void comparison_accumulate(UmiTestArchiveOutcomes *out, const UmiTestResult *result)
{
    ++out->recorded;
    switch (result->state)
    {
    case UMI_TEST_STATE_PASSED:
        ++out->passed;
        break;
    case UMI_TEST_STATE_FAILED:
        ++out->failed;
        break;
    case UMI_TEST_STATE_SKIPPED:
        ++out->skipped;
        break;
    case UMI_TEST_STATE_CANCELLED:
        ++out->cancelled;
        break;
    case UMI_TEST_STATE_TIMED_OUT:
        ++out->timed_out;
        break;
    default:
        ++out->not_run;
        break;
    }
    /* A cooperative stop is incomplete evidence, not a test reporting failure. */
    if (result->status != UMI_STATUS_OK && result->status != UMI_STATUS_CANCELLED)
        ++out->errors;
    out->duration_ms = UINT64_MAX - out->duration_ms < result->duration_ms
                           ? UINT64_MAX
                           : out->duration_ms + result->duration_ms;
}
static UmiStatus comparison_load(UmiTestArchive *archive, ArchiveRecords *records,
                                 const UmiTestArchiveEntry *entry, const UmiCancellationToken *token,
                                 ArchiveComparisonSide **out_side)
{
    ArchiveComparisonSide *side = calloc(entry->plan.request_count, sizeof(*side));
    UmiTestResult *result = calloc(1U, sizeof(*result));
    if (side == NULL || result == NULL)
    {
        free(side);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0; status == UMI_STATUS_OK && i < entry->plan.request_count; ++i)
    {
        if (comparison_cancelled(token))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = request_read(archive, records, entry, i, &side[i].request);
        side[i].outcomes.planned = entry->plan.repeat_count;
    }
    UmiCtestJobSnapshot total = {0};
    for (size_t i = 0; status == UMI_STATUS_OK && i < entry->run.completed; ++i)
    {
        if (comparison_cancelled(token))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = result_read(archive, records, entry, i, result);
        if (status == UMI_STATUS_OK)
        {
            accumulate(&total, result);
            comparison_accumulate(&side[i % entry->plan.request_count].outcomes, result);
        }
    }
    /* Metadata may have been altered outside Framework. Recompute every count
     * before classifying, rather than trusting a plausible-looking catalogue. */
    if (status == UMI_STATUS_OK && !counts_equal(&entry->run, &total))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        for (size_t i = 0; i < entry->plan.request_count; ++i)
            side[i].outcomes.never_started = side[i].outcomes.planned - side[i].outcomes.recorded;
        qsort(side, entry->plan.request_count, sizeof(*side), comparison_key);
        for (size_t i = 1; i < entry->plan.request_count; ++i)
            if (comparison_key(&side[i - 1U], &side[i]) == 0)
            {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
    }
    free(result);
    if (status != UMI_STATUS_OK)
        free(side);
    else
        *out_side = side;
    return status;
}
static bool comparison_failed(const UmiTestArchiveOutcomes *out)
{
    return out->failed != 0 || out->timed_out != 0 || out->errors != 0;
}
static bool comparison_passed(const UmiTestArchiveOutcomes *out)
{
    return out->planned != 0 && out->passed == out->planned;
}
static bool comparison_incomplete(const UmiTestArchiveOutcomes *out)
{
    return out->never_started != 0 || out->cancelled != 0 || out->not_run != 0;
}
static bool comparison_equal(const UmiTestArchiveOutcomes *a, const UmiTestArchiveOutcomes *b)
{
    return a->planned == b->planned && a->recorded == b->recorded && a->passed == b->passed &&
           a->failed == b->failed && a->skipped == b->skipped && a->cancelled == b->cancelled &&
           a->timed_out == b->timed_out && a->not_run == b->not_run && a->errors == b->errors;
}
static UmiTestArchiveChange comparison_classify(const UmiTestArchiveComparison *comparison,
                                                const ArchiveComparisonSide *a,
                                                const ArchiveComparisonSide *b)
{
    const UmiTestArchiveComparisonSummary *summary = &comparison->summary;
    if (!summary->same_source_root ||
        summary->baseline.plan.repeat_count != summary->candidate.plan.repeat_count ||
        summary->baseline.plan.stop_on_failure != summary->candidate.plan.stop_on_failure ||
        a->request.enabled != b->request.enabled || a->request.timeout_ms != b->request.timeout_ms)
        return UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
    if (comparison_passed(&a->outcomes) && comparison_failed(&b->outcomes))
        return UMI_TEST_ARCHIVE_REGRESSION;
    if (comparison_failed(&a->outcomes) && comparison_passed(&b->outcomes))
        return UMI_TEST_ARCHIVE_RECOVERED;
    if (comparison_failed(&a->outcomes) && comparison_failed(&b->outcomes))
        return UMI_TEST_ARCHIVE_PERSISTING_FAILURE;
    if (comparison_incomplete(&a->outcomes) || comparison_incomplete(&b->outcomes))
        return UMI_TEST_ARCHIVE_INCONCLUSIVE;
    return comparison_equal(&a->outcomes, &b->outcomes) ? UMI_TEST_ARCHIVE_UNCHANGED
                                                        : UMI_TEST_ARCHIVE_OUTCOME_CHANGED;
}
static UmiStatus comparison_merge(UmiTestArchiveComparison *comparison, const UmiCancellationToken *token)
{
    size_t a = 0, b = 0;
    size_t before = comparison->summary.baseline.plan.request_count;
    size_t after = comparison->summary.candidate.plan.request_count;
    /* Both bounded arrays are sorted, so matching is linear after the sort.
     * SIZE_MAX is private absence metadata, never an exposed array position. */
    while (a < before || b < after)
    {
        if (comparison_cancelled(token))
            return UMI_STATUS_CANCELLED;
        int order = a == before  ? 1
                    : b == after ? -1
                                 : comparison_key(&comparison->before[a], &comparison->after[b]);
        ArchiveComparisonIndex row = {SIZE_MAX, SIZE_MAX, UMI_TEST_ARCHIVE_UNCHANGED};
        if (order < 0)
        {
            row.before = a++;
            row.change = UMI_TEST_ARCHIVE_REMOVED;
        }
        else if (order > 0)
        {
            row.after = b++;
            row.change = UMI_TEST_ARCHIVE_ADDED;
        }
        else
        {
            row.before = a++;
            row.after = b++;
            row.change = comparison_classify(comparison, &comparison->before[row.before],
                                             &comparison->after[row.after]);
        }
        comparison->rows[comparison->summary.row_count++] = row;
        ++comparison->summary.counts[row.change];
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveCompare(UmiTestArchive *archive, uint64_t baseline_id, uint64_t candidate_id,
                                const UmiCancellationToken *token, UmiTestArchiveComparison **out_comparison)
{
    if (archive == NULL || baseline_id == 0 || candidate_id == 0 || out_comparison == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (comparison_cancelled(token))
        return UMI_STATUS_CANCELLED;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTestArchiveComparison *comparison = calloc(1U, sizeof(*comparison));
    size_t a = find(records, baseline_id), b = find(records, candidate_id);
    if (comparison == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else if (a == UMI_TEST_ARCHIVE_CAPACITY || b == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    else
    {
        comparison->summary.baseline = records->entries[a];
        comparison->summary.candidate = records->entries[b];
        const UmiTestArchiveOrigin *before = &records->entries[a].origin;
        const UmiTestArchiveOrigin *after = &records->entries[b].origin;
        comparison->summary.same_source_root = strcmp(before->source_root, after->source_root) == 0;
        comparison->summary.same_recorded_revision =
            before->source_revision[0] != '\0' &&
            strcmp(before->source_revision, after->source_revision) == 0;
        size_t capacity = records->entries[a].plan.request_count + records->entries[b].plan.request_count;
        comparison->rows = calloc(capacity, sizeof(*comparison->rows));
        if (comparison->rows == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = comparison_load(archive, records, &records->entries[a], token, &comparison->before);
    if (status == UMI_STATUS_OK)
        status = comparison_load(archive, records, &records->entries[b], token, &comparison->after);
    if (status == UMI_STATUS_OK)
        status = comparison_merge(comparison, token);
    if (status == UMI_STATUS_OK && comparison_cancelled(token))
        status = UMI_STATUS_CANCELLED;
    status = finish(archive, status);
    free(records);
    if (status != UMI_STATUS_OK)
        UmiTestArchiveComparisonDestroy(comparison);
    else
        *out_comparison = comparison;
    return status;
}
void UmiTestArchiveComparisonDestroy(UmiTestArchiveComparison *comparison)
{
    if (comparison == NULL)
        return;
    free(comparison->before);
    free(comparison->after);
    free(comparison->rows);
    free(comparison);
}
UmiStatus UmiTestArchiveComparisonRead(const UmiTestArchiveComparison *comparison,
                                       UmiTestArchiveComparisonSummary *out)
{
    if (comparison == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = comparison->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveComparisonRowAt(const UmiTestArchiveComparison *comparison, size_t index,
                                        UmiTestArchiveComparisonRow *out)
{
    if (comparison == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= comparison->summary.row_count)
        return UMI_STATUS_NOT_FOUND;
    const ArchiveComparisonIndex *selected = &comparison->rows[index];
    UmiTestArchiveComparisonRow row = {0};
    row.change = selected->change;
    row.before_present = selected->before != SIZE_MAX;
    row.after_present = selected->after != SIZE_MAX;
    if (row.before_present)
    {
        row.before_request = comparison->before[selected->before].request;
        row.before = comparison->before[selected->before].outcomes;
    }
    if (row.after_present)
    {
        row.after_request = comparison->after[selected->after].request;
        row.after = comparison->after[selected->after].outcomes;
    }
    *out = row;
    return UMI_STATUS_OK;
}
const char *UmiTestArchiveChangeText(UmiTestArchiveChange change)
{
    switch (change)
    {
    case UMI_TEST_ARCHIVE_UNCHANGED:
        return "Unchanged outcome";
    case UMI_TEST_ARCHIVE_REGRESSION:
        return "New failure after a complete pass";
    case UMI_TEST_ARCHIVE_RECOVERED:
        return "Complete pass after a failure";
    case UMI_TEST_ARCHIVE_PERSISTING_FAILURE:
        return "Failure remains";
    case UMI_TEST_ARCHIVE_ADDED:
        return "Added to selection";
    case UMI_TEST_ARCHIVE_REMOVED:
        return "Removed from selection";
    case UMI_TEST_ARCHIVE_CONDITIONS_CHANGED:
        return "Execution conditions changed";
    case UMI_TEST_ARCHIVE_INCONCLUSIVE:
        return "Incomplete evidence";
    case UMI_TEST_ARCHIVE_OUTCOME_CHANGED:
        return "Outcome changed";
    default:
        return "Unknown comparison category";
    }
}
