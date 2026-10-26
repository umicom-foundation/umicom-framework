/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_coverage_view.h
 *
 * PURPOSE:
 *   Filter required host coverage without changing whole-review qualification decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TESTING_CTEST_COVERAGE_VIEW_H
#define UMICOM_TESTING_CTEST_COVERAGE_VIEW_H
#include "umicom/testing/ctest_coverage.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CTEST_COVERAGE_VIEW_ALL ((1U << UMI_CTEST_COVERAGE_OUTCOME_COUNT) - 1U)
#define UMI_CTEST_COVERAGE_VIEW_OUTSTANDING (UMI_CTEST_COVERAGE_VIEW_ALL & ~1U)
typedef enum UmiCtestCoverageOrder {
    UMI_CTEST_COVERAGE_REQUIREMENT_ORDER = 0,
    UMI_CTEST_COVERAGE_NAME_ORDER,
    UMI_CTEST_COVERAGE_HOST_ORDER,
    UMI_CTEST_COVERAGE_OUTCOME_ORDER
} UmiCtestCoverageOrder;
typedef struct UmiCtestCoverageFilter {
    unsigned outcomes; /* Bit N selects UmiCtestCoverageOutcome N. Zero selects no rows. */
    UmiCtestCoverageOrder order;
    char text_contains[256]; /* Test name OR capability; then AND the other restrictions. */
    char host_contains[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char configuration_contains[UMI_CTEST_COVERAGE_HOST_CAPACITY];
} UmiCtestCoverageFilter;
typedef struct UmiCtestCoverageViewSummary {
    UmiCtestCoverageSummary source;
    size_t matches;
    size_t outstanding_matches;
    UmiCtestCoverageFilter filter; /* Accepted filter copy, independent of entry edits. */
} UmiCtestCoverageViewSummary;
typedef struct UmiCtestCoverageView UmiCtestCoverageView;
/* Default: all outstanding rows in requirement order, with no text restrictions. */
void UmiCtestCoverageFilterInit(UmiCtestCoverageFilter *filter);
/* Require bounded printable UTF-8 strings. Empty restrictions are allowed.
 * Searches fold ASCII letters only and treat punctuation literally, not as regex. */
UmiStatus UmiCtestCoverageFilterValidate(const UmiCtestCoverageFilter *filter);
/* Borrow an immutable review. Destroy every view before its review. Filter values
 * are consumed during creation and may then change. Results keep source order
 * for equal sort keys. Failure clears out; inputs/output must not overlap.
 * This is presentation only: no I/O, execution or qualification gate. A filtered
 * set of passes never changes UmiCtestCoverageRequirePassed(review). */
UmiStatus UmiCtestCoverageViewCreate(const UmiCtestCoverageReview *review,
    const UmiCtestCoverageFilter *filter, UmiCtestCoverageView **out);
void UmiCtestCoverageViewDestroy(UmiCtestCoverageView *view);
UmiStatus UmiCtestCoverageViewRead(const UmiCtestCoverageView *view, UmiCtestCoverageViewSummary *out);
const UmiCtestCoverageRow *UmiCtestCoverageViewAt(const UmiCtestCoverageView *view, size_t index);
/* Exact source identity borrowed from the review; NULL for NULL view. */
const char *UmiCtestCoverageViewSourceIdentity(const UmiCtestCoverageView *view);
/* 1..128 escaped rows per page, with whole-review counts and filter scope.
 * NULL/0 queries length excluding NUL. A short buffer is unchanged. Invalid
 * arguments preserve required; empty views accept only offset zero.
 * Caller outputs and borrowed review storage must not overlap. */
UmiStatus UmiCtestCoverageViewFormat(const UmiCtestCoverageView *view,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
