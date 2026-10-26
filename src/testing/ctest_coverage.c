/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage.c
 *
 * PURPOSE:
 *   Own host coverage decisions using immutable reports and exact source identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_coverage.h"
#include "umicom/testing/ctest_report_view.h"
#include <stdlib.h>
#include <string.h>

struct UmiCtestCoverageReview {
    UmiCtestCoverageSummary summary;
    char source_identity[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
    UmiCtestCoverageRow *rows;
};

static UmiStatus Text(const char *text, size_t capacity, bool optional)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0;
    while (length < capacity && text[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0) return optional ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    /* Reuse the imported test-name scalar grammar, not a second UTF-8 parser.
     * The escaped value is validation scratch and is never executed. */
    char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    return UmiCtestEscapeName(text, escaped, sizeof escaped);
}

static int RequirementOrder(const void *left, const void *right)
{
    const UmiCtestCoverageRequirement *const *a = left;
    const UmiCtestCoverageRequirement *const *b = right;
    int order = strcmp((*a)->host, (*b)->host);
    if (order == 0) order = strcmp((*a)->configuration, (*b)->configuration);
    return order != 0 ? order : strcmp((*a)->name, (*b)->name);
}

static const UmiCtestReportEntry *Find(const UmiCtestReportView *view, size_t count,
    const char *name)
{
    size_t first = 0, last = count;
    while (first < last) {
        size_t middle = first + (last - first) / 2U;
        const UmiCtestReportEntry *entry = UmiCtestReportViewAt(view, middle);
        if (entry == NULL) return NULL;
        int order = strcmp(entry->name, name);
        if (order < 0) first = middle + 1U;
        else if (order > 0) last = middle;
        else return entry;
    }
    return NULL;
}

static UmiStatus Classify(const UmiCtestReportEntry *entry, UmiCtestCoverageOutcome *out)
{
    switch (UmiCtestReportEntryOutcome(entry)) {
    case UMI_CTEST_VIEW_PASSED: *out = UMI_CTEST_COVERAGE_PASSED; break;
    case UMI_CTEST_VIEW_FAILED: *out = UMI_CTEST_COVERAGE_FAILED; break;
    case UMI_CTEST_VIEW_TIMED_OUT: *out = UMI_CTEST_COVERAGE_TIMED_OUT; break;
    case UMI_CTEST_VIEW_SKIPPED: *out = UMI_CTEST_COVERAGE_SKIPPED; break;
    case UMI_CTEST_VIEW_DISABLED: *out = UMI_CTEST_COVERAGE_DISABLED; break;
    case UMI_CTEST_VIEW_NOT_RUN: *out = UMI_CTEST_COVERAGE_NOT_RUN; break;
    default: return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestCoverageReviewCreate(const char *source_identity,
    const UmiCtestCoverageRequirement *requirements, size_t requirement_count,
    const UmiCtestCoverageHost *hosts, size_t host_count, UmiCtestCoverageReview **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if ((requirement_count != 0 && requirements == NULL) ||
        (host_count != 0 && hosts == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (requirement_count > UMI_CTEST_COVERAGE_MAX_REQUIREMENTS ||
        host_count > UMI_CTEST_COVERAGE_MAX_HOSTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = Text(source_identity, UMI_CTEST_COVERAGE_IDENTITY_CAPACITY, false);
    if (status != UMI_STATUS_OK) return status;
    UmiCtestCoverageReview *review = calloc(1U, sizeof *review);
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(review->source_identity, source_identity, strlen(source_identity) + 1U);
    UmiCtestReportView **views = host_count ? calloc(host_count, sizeof *views) : NULL;
    size_t *counts = host_count ? calloc(host_count, sizeof *counts) : NULL;
    const UmiCtestCoverageRequirement **sorted = requirement_count ?
        malloc(requirement_count * sizeof *sorted) : NULL;
    review->rows = requirement_count ? calloc(requirement_count, sizeof *review->rows) : NULL;
    if ((host_count && (!views || !counts)) ||
        (requirement_count && (!sorted || !review->rows))) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }
    /* Index each immutable report once. Binary name lookup avoids repeatedly
     * scanning a large report for every required host/test combination. */
    UmiCtestReportFilter filter;
    UmiCtestReportFilterInit(&filter);
    filter.outcomes = UMI_CTEST_VIEW_ALL;
    filter.order = UMI_CTEST_REPORT_NAME_ORDER;
    for (size_t h = 0; h < host_count; ++h) {
        status = Text(hosts[h].host, sizeof hosts[h].host, false);
        if (status == UMI_STATUS_OK)
            status = Text(hosts[h].configuration, sizeof hosts[h].configuration, false);
        if (status == UMI_STATUS_OK)
            status = Text(hosts[h].source_identity, sizeof hosts[h].source_identity, false);
        if (status != UMI_STATUS_OK) goto cleanup;
        for (size_t earlier = 0; earlier < h; ++earlier) {
            if (!strcmp(hosts[h].host, hosts[earlier].host) &&
                !strcmp(hosts[h].configuration, hosts[earlier].configuration)) {
                status = UMI_STATUS_ALREADY_EXISTS;
                goto cleanup;
            }
        }
        status = UmiCtestReportViewCreate(hosts[h].report, &filter, &views[h]);
        UmiCtestReportViewSummary summary;
        if (status == UMI_STATUS_OK) status = UmiCtestReportViewRead(views[h], &summary);
        if (status != UMI_STATUS_OK) goto cleanup;
        counts[h] = summary.matches;
    }
    for (size_t i = 0; i < requirement_count; ++i) {
        const UmiCtestCoverageRequirement *r = &requirements[i];
        status = Text(r->name, sizeof r->name, false);
        if (status == UMI_STATUS_OK) status = Text(r->host, sizeof r->host, false);
        if (status == UMI_STATUS_OK) status = Text(r->configuration, sizeof r->configuration, false);
        if (status == UMI_STATUS_OK) status = Text(r->capability, sizeof r->capability, true);
        if (status != UMI_STATUS_OK) goto cleanup;
        review->rows[i].requirement = *r;
        sorted[i] = r;
    }
    if (requirement_count > 1U) {
        qsort(sorted, requirement_count, sizeof *sorted, RequirementOrder);
        for (size_t i = 1; i < requirement_count; ++i)
            if (RequirementOrder(&sorted[i - 1U], &sorted[i]) == 0) {
                status = UMI_STATUS_ALREADY_EXISTS;
                goto cleanup;
            }
    }
    for (size_t i = 0; i < requirement_count; ++i) {
        UmiCtestCoverageRow *row = &review->rows[i];
        row->outcome = UMI_CTEST_COVERAGE_MISSING_HOST;
        for (size_t h = 0; h < host_count; ++h) {
            if (strcmp(row->requirement.host, hosts[h].host) ||
                strcmp(row->requirement.configuration, hosts[h].configuration)) continue;
            memcpy(row->observed_source_identity, hosts[h].source_identity,
                strlen(hosts[h].source_identity) + 1U);
            if (strcmp(source_identity, hosts[h].source_identity)) {
                row->outcome = UMI_CTEST_COVERAGE_SOURCE_MISMATCH;
                break;
            }
            const UmiCtestReportEntry *entry = Find(views[h], counts[h], row->requirement.name);
            if (entry == NULL) row->outcome = UMI_CTEST_COVERAGE_MISSING_TEST;
            else {
                status = Classify(entry, &row->outcome);
                if (status != UMI_STATUS_OK) goto cleanup;
                row->has_observation = true;
                row->observation = entry->result;
            }
            break;
        }
        ++review->summary.outcomes[row->outcome];
        if (row->outcome != UMI_CTEST_COVERAGE_PASSED) ++review->summary.outstanding;
    }
    review->summary.requirements = requirement_count;
    *out = review;
    review = NULL;
cleanup:
    if (views != NULL)
        for (size_t h = 0; h < host_count; ++h) UmiCtestReportViewDestroy(views[h]);
    free(views);
    free(counts);
    free(sorted);
    UmiCtestCoverageReviewDestroy(review);
    return status;
}

const char *UmiCtestCoverageReviewSourceIdentity(const UmiCtestCoverageReview *review)
{
    return review ? review->source_identity : NULL;
}

UmiStatus UmiCtestCoverageReviewRead(const UmiCtestCoverageReview *review,
    UmiCtestCoverageSummary *out)
{
    if (!review || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = review->summary;
    return UMI_STATUS_OK;
}

const UmiCtestCoverageRow *UmiCtestCoverageReviewAt(const UmiCtestCoverageReview *review,
    size_t index)
{
    return review && index < review->summary.requirements ? &review->rows[index] : NULL;
}

UmiStatus UmiCtestCoverageRequirePassed(const UmiCtestCoverageReview *review)
{
    if (!review) return UMI_STATUS_INVALID_ARGUMENT;
    return review->summary.requirements && review->summary.outstanding == 0 ?
        UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

void UmiCtestCoverageReviewDestroy(UmiCtestCoverageReview *review)
{
    if (review == NULL) return;
    free(review->rows);
    free(review);
}

const char *UmiCtestCoverageOutcomeText(UmiCtestCoverageOutcome outcome)
{
    switch (outcome) {
    case UMI_CTEST_COVERAGE_PASSED: return "passed";
    case UMI_CTEST_COVERAGE_FAILED: return "failed";
    case UMI_CTEST_COVERAGE_TIMED_OUT: return "timed out";
    case UMI_CTEST_COVERAGE_SKIPPED: return "skipped";
    case UMI_CTEST_COVERAGE_DISABLED: return "disabled";
    case UMI_CTEST_COVERAGE_NOT_RUN: return "not run";
    case UMI_CTEST_COVERAGE_MISSING_HOST: return "missing host";
    case UMI_CTEST_COVERAGE_MISSING_TEST: return "missing test";
    case UMI_CTEST_COVERAGE_SOURCE_MISMATCH: return "source mismatch";
    default: return "unknown";
    }
}
