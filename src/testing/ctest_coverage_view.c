/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_view.c
 *
 * PURPOSE:
 *   Index immutable coverage requirements for deterministic outcome and host navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/testing/ctest_coverage_view.h"
#include <stdlib.h>
#include <string.h>
typedef struct CoverageViewRow {
    const UmiCtestCoverageRow *row;
    size_t source_index;
} CoverageViewRow;
struct UmiCtestCoverageView {
    const UmiCtestCoverageReview *review;
    UmiCtestCoverageViewSummary summary;
    CoverageViewRow *rows;
};
void UmiCtestCoverageFilterInit(UmiCtestCoverageFilter *filter) {
    if (filter != NULL) {
        memset(filter, 0, sizeof *filter);
        filter->outcomes = UMI_CTEST_COVERAGE_VIEW_OUTSTANDING;
    }
}
static UmiStatus FilterText(const char *text, size_t capacity) {
    size_t size = 0U;
    while (size < capacity && text[size] != '\0') ++size;
    if (size == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (size == 0U) return UMI_STATUS_OK;
    char scratch[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    return UmiCtestEscapeName(text, scratch, sizeof scratch);
}
UmiStatus UmiCtestCoverageFilterValidate(const UmiCtestCoverageFilter *filter) {
    if (filter == NULL || (filter->outcomes & ~UMI_CTEST_COVERAGE_VIEW_ALL) != 0U ||
        filter->order < UMI_CTEST_COVERAGE_REQUIREMENT_ORDER || filter->order > UMI_CTEST_COVERAGE_OUTCOME_ORDER)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = FilterText(filter->text_contains, sizeof filter->text_contains);
    if (status == UMI_STATUS_OK) status = FilterText(filter->host_contains, sizeof filter->host_contains);
    if (status == UMI_STATUS_OK) status = FilterText(filter->configuration_contains, sizeof filter->configuration_contains);
    return status;
}
static unsigned char Fold(unsigned char c) {
    return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c;
}
static bool Contains(const char *text, const char *part) {
    if (*part == '\0') return true;
    for (size_t start = 0U; text[start] != '\0'; ++start) {
        size_t n = 0U;
        while (part[n] != '\0' && text[start + n] != '\0' &&
            Fold((unsigned char)text[start + n]) == Fold((unsigned char)part[n])) ++n;
        if (part[n] == '\0') return true;
    }
    return false;
}
static int Original(const CoverageViewRow *a, const CoverageViewRow *b) {
    return (a->source_index > b->source_index) - (a->source_index < b->source_index);
}
static int ByName(const void *left, const void *right) {
    const CoverageViewRow *a = left, *b = right;
    int order = strcmp(a->row->requirement.name, b->row->requirement.name);
    return order != 0 ? order : Original(a, b);
}
static int ByHost(const void *left, const void *right) {
    const CoverageViewRow *a = left, *b = right;
    int order = strcmp(a->row->requirement.host, b->row->requirement.host);
    if (order == 0) order = strcmp(a->row->requirement.configuration, b->row->requirement.configuration);
    return order != 0 ? order : Original(a, b);
}
static int ByOutcome(const void *left, const void *right) {
    const CoverageViewRow *a = left, *b = right;
    int order = (a->row->outcome > b->row->outcome) - (a->row->outcome < b->row->outcome);
    return order != 0 ? order : Original(a, b);
}
void UmiCtestCoverageViewDestroy(UmiCtestCoverageView *view) {
    if (view != NULL) { free(view->rows); free(view); }
}
UmiStatus UmiCtestCoverageViewCreate(const UmiCtestCoverageReview *review,
    const UmiCtestCoverageFilter *filter, UmiCtestCoverageView **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiCtestCoverageFilterValidate(filter);
    if (status != UMI_STATUS_OK) return status;
    UmiCtestCoverageSummary source;
    status = UmiCtestCoverageReviewRead(review, &source);
    if (status != UMI_STATUS_OK) return status;
    if (source.requirements > UMI_CTEST_COVERAGE_MAX_REQUIREMENTS ||
        source.requirements > SIZE_MAX / sizeof(CoverageViewRow)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCtestCoverageView *view = calloc(1U, sizeof *view);
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->review = review; view->summary.source = source;
    view->summary.filter = *filter;
    if (source.requirements != 0U) {
        view->rows = calloc(source.requirements, sizeof *view->rows);
        if (view->rows == NULL) { free(view); return UMI_STATUS_OUT_OF_MEMORY; }
    }
    for (size_t i = 0U; i < source.requirements; ++i) {
        const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, i);
        if (row == NULL || row->outcome < UMI_CTEST_COVERAGE_PASSED || row->outcome >= UMI_CTEST_COVERAGE_OUTCOME_COUNT) {
            UmiCtestCoverageViewDestroy(view); return UMI_STATUS_INVALID_STATE;
        }
        if ((filter->outcomes & (1U << (unsigned)row->outcome)) != 0U &&
            (Contains(row->requirement.name, filter->text_contains) || Contains(row->requirement.capability, filter->text_contains)) &&
            Contains(row->requirement.host, filter->host_contains) &&
            Contains(row->requirement.configuration, filter->configuration_contains)) {
            view->rows[view->summary.matches++] = (CoverageViewRow){row, i};
            if (row->outcome != UMI_CTEST_COVERAGE_PASSED) ++view->summary.outstanding_matches;
        }
    }
    if (view->summary.matches > 1U && filter->order != UMI_CTEST_COVERAGE_REQUIREMENT_ORDER) {
        int (*compare)(const void *, const void *) = filter->order == UMI_CTEST_COVERAGE_NAME_ORDER
            ? ByName : filter->order == UMI_CTEST_COVERAGE_HOST_ORDER ? ByHost : ByOutcome;
        qsort(view->rows, view->summary.matches, sizeof *view->rows, compare);
    }
    *out = view;
    return UMI_STATUS_OK;
}
UmiStatus UmiCtestCoverageViewRead(const UmiCtestCoverageView *view, UmiCtestCoverageViewSummary *out) {
    if (view == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = view->summary; return UMI_STATUS_OK;
}
const UmiCtestCoverageRow *UmiCtestCoverageViewAt(const UmiCtestCoverageView *view, size_t index) {
    return view != NULL && index < view->summary.matches ? view->rows[index].row : NULL;
}
const char *UmiCtestCoverageViewSourceIdentity(const UmiCtestCoverageView *view) {
    return view != NULL ? UmiCtestCoverageReviewSourceIdentity(view->review) : NULL;
}
