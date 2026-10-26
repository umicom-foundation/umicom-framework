/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_comparison_text.c
 *
 * PURPOSE:
 *   Format exact-name saved outcome changes without treating missing records as repairs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/testing/ctest_report_comparison.h"
#include <stdio.h>
#include <string.h>

typedef struct ComparisonText {
    char *out;
    size_t used;
} ComparisonText;

static void Append(ComparisonText *writer, const char *text)
{
    size_t length = strlen(text);
    if (writer->out) memcpy(writer->out + writer->used, text, length);
    writer->used += length;
}
static void Name(ComparisonText *writer, const char *name)
{
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p) {
        char text[5] = {(char)*p, '\0'};
        if (*p < 32U || *p == 127U || *p == '\\')
            (void)snprintf(text, sizeof text, "\\x%02X", (unsigned)*p);
        Append(writer, text);
    }
}
static const char *Kind(UmiCtestReportChangeKind kind)
{
    switch (kind) {
    case UMI_CTEST_CHANGE_ADDED: return "added";
    case UMI_CTEST_CHANGE_REMOVED: return "removed";
    case UMI_CTEST_CHANGE_RECOVERED: return "recovered";
    case UMI_CTEST_CHANGE_REGRESSED: return "regressed";
    case UMI_CTEST_CHANGE_OUTCOME: return "outcome changed";
    }
    return "unknown";
}
static const char *Outcome(UmiTestState state, bool present, bool disabled)
{
    if (!present) return "absent";
    if (disabled) return "disabled";
    switch (state) {
    case UMI_TEST_STATE_PASSED: return "passed";
    case UMI_TEST_STATE_FAILED: return "failed";
    case UMI_TEST_STATE_TIMED_OUT: return "timed out";
    case UMI_TEST_STATE_SKIPPED: return "skipped";
    case UMI_TEST_STATE_NOT_RUN: return "not run";
    default: return "unknown";
    }
}
static void Page(ComparisonText *writer, const UmiCtestReportComparison *comparison,
    const UmiCtestReportComparisonSummary *summary, size_t offset, size_t end)
{
    char text[768];
    Append(writer, "Saved outcome comparison; no tests were run or source identities verified.\n");
    (void)snprintf(text, sizeof text,
        "Before: %zu | After: %zu | Unchanged outcomes: %zu | Changed: %zu\n"
        "Added: %zu | Removed: %zu | Recovered: %zu | Regressed: %zu | Other outcome changes: %zu\n"
        "Showing %zu..%zu of %zu changed records.\n",
        summary->before_tests, summary->after_tests, summary->unchanged, summary->changed,
        summary->added, summary->removed, summary->recovered, summary->regressed,
        summary->outcome_changed, summary->changed ? offset + 1U : 0U, end, summary->changed);
    Append(writer, text);
    Append(writer, "Names match exactly. Removed tests are not repairs; unchanged outcomes may still be failures.\n");
    if (!summary->changed) Append(writer, "No outcome changes. This does not establish an all-passed result.\n");
    for (size_t i = offset; i < end; ++i) {
        const UmiCtestReportChange *change = UmiCtestReportComparisonAt(comparison, i);
        Append(writer, "\n["); Append(writer, Kind(change->kind)); Append(writer, "] ");
        Name(writer, change->name);
        Append(writer, "\n  Before: ");
        Append(writer, Outcome(change->before, change->before_present, change->before_disabled));
        Append(writer, " -> After: ");
        Append(writer, Outcome(change->after, change->after_present, change->after_disabled));
        Append(writer, "\n");
    }
}
UmiStatus UmiCtestReportComparisonFormat(const UmiCtestReportComparison *comparison,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required)
{
    if (!comparison || !required || !limit || limit > 128U || (!out && capacity))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestReportComparisonSummary summary;
    UmiStatus status = UmiCtestReportComparisonReadSummary(comparison, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (offset > summary.changed || (offset == summary.changed && offset))
        return UMI_STATUS_NOT_FOUND;
    size_t count = summary.changed - offset;
    size_t end = offset + (count < limit ? count : limit);
    /* Comparison creation owns bounded records. Two bounded passes preserve
     * caller output on a short buffer without allocating whole-report text. */
    ComparisonText measure = {0};
    Page(&measure, comparison, &summary, offset, end);
    *required = measure.used;
    if (!out) return UMI_STATUS_OK;
    if (capacity <= measure.used) return UMI_STATUS_CAPACITY_EXCEEDED;
    ComparisonText writer = {out, 0};
    Page(&writer, comparison, &summary, offset, end);
    out[writer.used] = '\0';
    return UMI_STATUS_OK;
}
