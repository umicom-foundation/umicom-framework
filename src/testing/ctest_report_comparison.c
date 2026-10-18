/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_comparison.c
 * Purpose: Match bounded saved test identities and preserve inventory changes.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_report_comparison.h"
#include <stdlib.h>
#include <string.h>

struct UmiCtestReportComparison {
    UmiCtestReportComparisonSummary summary;
    UmiCtestReportChange *changes;
};

static int CompareNames(const void *left, const void *right)
{
    const UmiCtestReportEntry *const *a = left;
    const UmiCtestReportEntry *const *b = right;
    return strcmp((*a)->name, (*b)->name);
}

/* Sort borrowed indexes, never the immutable imported reports. Exact decoded
 * names avoid treating punctuation, Unicode or regex syntax as selections. */
static UmiStatus SortedEntries(const UmiCtestReport *report, size_t count,
    const UmiCtestReportEntry ***out)
{
    *out = NULL;
    if (count == 0U) return UMI_STATUS_OK;
    const UmiCtestReportEntry **entries = malloc(count * sizeof *entries);
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i) {
        entries[i] = UmiCtestReportEntryAt(report, i);
        if (entries[i] == NULL) { free(entries); return UMI_STATUS_INVALID_STATE; }
    }
    qsort(entries, count, sizeof *entries, CompareNames);
    *out = entries;
    return UMI_STATUS_OK;
}

static void AppendChange(UmiCtestReportComparison *comparison,
    const UmiCtestReportEntry *before, const UmiCtestReportEntry *after)
{
    UmiCtestReportComparisonSummary *summary = &comparison->summary;
    if (before != NULL && after != NULL && before->result.state == after->result.state &&
        before->disabled == after->disabled) {
        ++summary->unchanged;
        return;
    }
    UmiCtestReportChange *change = &comparison->changes[summary->changed++];
    memcpy(change->name, before != NULL ? before->name : after->name, sizeof change->name);
    change->before_present = before != NULL;
    change->after_present = after != NULL;
    change->before = before == NULL ? UMI_TEST_STATE_NOT_RUN : before->result.state;
    change->after = after == NULL ? UMI_TEST_STATE_NOT_RUN : after->result.state;
    change->before_disabled = before != NULL && before->disabled;
    change->after_disabled = after != NULL && after->disabled;
    if (before == NULL) { change->kind = UMI_CTEST_CHANGE_ADDED; ++summary->added; }
    else if (after == NULL) { change->kind = UMI_CTEST_CHANGE_REMOVED; ++summary->removed; }
    else if (after->result.state == UMI_TEST_STATE_PASSED) {
        change->kind = UMI_CTEST_CHANGE_RECOVERED; ++summary->recovered;
    } else if (before->result.state == UMI_TEST_STATE_PASSED) {
        change->kind = UMI_CTEST_CHANGE_REGRESSED; ++summary->regressed;
    } else {
        change->kind = UMI_CTEST_CHANGE_OUTCOME; ++summary->outcome_changed;
    }
}

UmiStatus UmiCtestReportCompare(const UmiCtestReport *before, const UmiCtestReport *after,
    UmiCtestReportComparison **outComparison)
{
    if (outComparison == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outComparison = NULL;
    UmiCtestReportSummary left, right;
    UmiStatus status = UmiCtestReportReadSummary(before, &left);
    if (status == UMI_STATUS_OK) status = UmiCtestReportReadSummary(after, &right);
    if (status != UMI_STATUS_OK) return status;
    if (left.tests > UMI_CTEST_COLLECTION_MAX_TESTS || right.tests > UMI_CTEST_COLLECTION_MAX_TESTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCtestReportComparison *comparison = calloc(1U, sizeof *comparison);
    if (comparison == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    comparison->summary.before_tests = left.tests;
    comparison->summary.after_tests = right.tests;
    size_t capacity = left.tests + right.tests;
    const UmiCtestReportEntry **a = NULL, **b = NULL;
    if (capacity != 0U) {
        comparison->changes = calloc(capacity, sizeof *comparison->changes);
        if (comparison->changes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = SortedEntries(before, left.tests, &a);
    if (status == UMI_STATUS_OK) status = SortedEntries(after, right.tests, &b);
    if (status == UMI_STATUS_OK) {
        size_t i = 0U, j = 0U;
        while (i < left.tests || j < right.tests) {
            int order = i == left.tests ? 1 : j == right.tests ? -1 : strcmp(a[i]->name, b[j]->name);
            if (order < 0) AppendChange(comparison, a[i++], NULL);
            else if (order > 0) AppendChange(comparison, NULL, b[j++]);
            else { AppendChange(comparison, a[i++], b[j++]); }
        }
        *outComparison = comparison;
    } else UmiCtestReportComparisonDestroy(comparison);
    free(a);
    free(b);
    return status;
}

UmiStatus UmiCtestReportComparisonReadSummary(const UmiCtestReportComparison *comparison,
    UmiCtestReportComparisonSummary *outSummary)
{
    if (comparison == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = comparison->summary;
    return UMI_STATUS_OK;
}

const UmiCtestReportChange *UmiCtestReportComparisonAt(const UmiCtestReportComparison *comparison,
    size_t index)
{
    return comparison == NULL || index >= comparison->summary.changed ? NULL : &comparison->changes[index];
}

void UmiCtestReportComparisonDestroy(UmiCtestReportComparison *comparison)
{
    if (comparison == NULL) return;
    free(comparison->changes);
    free(comparison);
}
