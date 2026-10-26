/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_text.c
 *
 * PURPOSE:
 *   Format bounded host-coverage pages without changing saved observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_coverage.h"
#include <stdio.h>
#include <string.h>

typedef struct CoverageWriter {
    char *text;
    size_t used;
} CoverageWriter;

static void Append(CoverageWriter *writer, const char *text)
{
    size_t length = strlen(text);
    if (writer->text) memcpy(writer->text + writer->used, text, length);
    writer->used += length;
}
static void Text(CoverageWriter *writer, const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        /* Escape separators as well as control bytes. A copied page must not
         * let imported text look like another host/outcome line. */
        char escaped[5] = {(char)*p, '\0'};
        if (*p < 32U || *p == 127U || *p == '\\')
            (void)snprintf(escaped, sizeof escaped, "\\x%02X", (unsigned)*p);
        Append(writer, escaped);
    }
}
static void Page(CoverageWriter *writer, const UmiCtestCoverageReview *review,
    const UmiCtestCoverageSummary *summary, size_t offset, size_t end)
{
    char line[640];
    Append(writer, "Saved host coverage; no tests were run and source identities are not verified.\n");
    Append(writer, "Requested source: ");
    Text(writer, UmiCtestCoverageReviewSourceIdentity(review));
    (void)snprintf(line, sizeof line,
        "\nRequirements: %zu | Passed: %zu | Outstanding: %zu\n"
        "Showing %zu..%zu of %zu required records.\n"
        "Only listed requirements are reviewed. Unlisted tests and hosts are outside this gate.\n",
        summary->requirements, summary->outcomes[UMI_CTEST_COVERAGE_PASSED],
        summary->outstanding, summary->requirements ? offset + 1U : 0U, end, summary->requirements);
    Append(writer, line);
    if (!summary->requirements) Append(writer, "No requirements: empty coverage cannot pass.\n");
    for (size_t i = offset; i < end; ++i) {
        const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, i);
        Append(writer, "\n[");
        Append(writer, UmiCtestCoverageOutcomeText(row->outcome));
        Append(writer, "] ");
        Text(writer, row->requirement.host);
        Append(writer, " / ");
        Text(writer, row->requirement.configuration);
        Append(writer, " : ");
        Text(writer, row->requirement.name);
        if (row->requirement.capability[0]) {
            Append(writer, "\n  Requires: ");
            Text(writer, row->requirement.capability);
        }
        if (row->observed_source_identity[0]) {
            Append(writer, "\n  Observed source: ");
            Text(writer, row->observed_source_identity);
        }
        if (row->has_observation) {
            (void)snprintf(line, sizeof line, "\n  Saved duration: %llu ms",
                (unsigned long long)row->observation.duration_ms);
            Append(writer, line);
        } else {
            Append(writer, "\n  No matching observation for the requested source.");
        }
        Append(writer, "\n");
    }
}
UmiStatus UmiCtestCoverageReviewFormat(const UmiCtestCoverageReview *review,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required)
{
    if (!review || !required || !limit || limit > 128U || (!out && capacity))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestCoverageSummary summary;
    UmiStatus status = UmiCtestCoverageReviewRead(review, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (offset > summary.requirements || (offset == summary.requirements && offset))
        return UMI_STATUS_NOT_FOUND;
    size_t count = summary.requirements - offset;
    size_t end = offset + (count < limit ? count : limit);
    /* Creation bounds all copied fields. The page limit bounds both passes,
     * so no unbounded whole-report allocation or partial output is needed. */
    CoverageWriter measure = {0};
    Page(&measure, review, &summary, offset, end);
    *required = measure.used;
    if (!out) return UMI_STATUS_OK;
    if (capacity <= measure.used) return UMI_STATUS_CAPACITY_EXCEEDED;
    CoverageWriter writer = {out, 0};
    Page(&writer, review, &summary, offset, end);
    out[writer.used] = '\0';
    return UMI_STATUS_OK;
}
