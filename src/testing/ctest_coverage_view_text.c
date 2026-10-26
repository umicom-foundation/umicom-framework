/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_view_text.c
 *
 * PURPOSE:
 *   Present filtered coverage pages while retaining whole-review source and gate context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/testing/ctest_coverage_view.h"
#include "ctest_coverage_text_private.h"

static void ViewPage(CoverageWriter *writer, const UmiCtestCoverageView *view,
    const UmiCtestCoverageViewSummary *summary, size_t offset, size_t end) {
    char line[640];
    Append(writer, "Saved host coverage; presentation filter only. No tests or source verification performed.\n");
    Append(writer, "Requested source: "); Text(writer, UmiCtestCoverageViewSourceIdentity(view));
    (void)snprintf(line, sizeof line,
        "\nWhole review: %zu requirements | Passed: %zu | Outstanding: %zu\n"
        "Filter: %zu matching records | Outstanding matches: %zu\n"
        "Showing %zu..%zu of %zu matching records.\n"
        "Filtered rows cannot certify the whole review. Unlisted tests and hosts remain outside its gate.\n",
        summary->source.requirements, summary->source.outcomes[UMI_CTEST_COVERAGE_PASSED],
        summary->source.outstanding, summary->matches, summary->outstanding_matches,
        summary->matches ? offset + 1U : 0U, end, summary->matches);
    Append(writer, line);
    Append(writer, "Applied test/capability text: "); Text(writer, summary->filter.text_contains);
    Append(writer, "\nApplied host text: "); Text(writer, summary->filter.host_contains);
    Append(writer, "\nApplied configuration text: "); Text(writer, summary->filter.configuration_contains);
    Append(writer, "\nSelected outcomes:");
    for (unsigned i = 0U; i < UMI_CTEST_COVERAGE_OUTCOME_COUNT; ++i) {
        if ((summary->filter.outcomes & (1U << i)) == 0U) continue;
        Append(writer, " ["); Append(writer, UmiCtestCoverageOutcomeText((UmiCtestCoverageOutcome)i)); Append(writer, "]");
    }
    static const char *orders[] = {"requirement", "test name", "host/configuration", "outcome"};
    Append(writer, "\nOrder: "); Append(writer, orders[(unsigned)summary->filter.order]); Append(writer, "\n");
    if (summary->matches == 0U) Append(writer, "No matching rows. This is not a passing qualification.\n");
    for (size_t i = offset; i < end; ++i) CoverageTextRow(writer, UmiCtestCoverageViewAt(view, i));
}
UmiStatus UmiCtestCoverageViewFormat(const UmiCtestCoverageView *view,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required) {
    if (view == NULL || required == NULL || limit == 0U || limit > 128U || (out == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestCoverageViewSummary summary;
    UmiStatus status = UmiCtestCoverageViewRead(view, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (offset > summary.matches || (offset == summary.matches && offset != 0U)) return UMI_STATUS_NOT_FOUND;
    size_t count = summary.matches - offset;
    size_t end = offset + (count < limit ? count : limit);
    CoverageWriter measure = {0};
    ViewPage(&measure, view, &summary, offset, end);
    *required = measure.used;
    if (out == NULL) return UMI_STATUS_OK;
    if (capacity <= measure.used) return UMI_STATUS_CAPACITY_EXCEEDED;
    CoverageWriter writer = {out, 0U};
    ViewPage(&writer, view, &summary, offset, end);
    out[writer.used] = '\0';
    return UMI_STATUS_OK;
}
