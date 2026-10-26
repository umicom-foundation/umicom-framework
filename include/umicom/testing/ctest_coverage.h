/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_coverage.h
 *
 * PURPOSE:
 *   Review required host coverage without treating skips or stale results as passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_CTEST_COVERAGE_H
#define UMICOM_TESTING_CTEST_COVERAGE_H
#include "umicom/testing/ctest_report.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CTEST_COVERAGE_HOST_CAPACITY 64U
#define UMI_CTEST_COVERAGE_IDENTITY_CAPACITY 128U
#define UMI_CTEST_COVERAGE_MAX_HOSTS 256U
#define UMI_CTEST_COVERAGE_MAX_REQUIREMENTS 100000U

/* Identity text is caller-supplied metadata, never an authenticated attestation.
 * Keep complete source/build fingerprints rather than a mutable branch name. */
typedef struct UmiCtestCoverageHost {
    char host[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char configuration[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char source_identity[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
    const UmiCtestReport *report;
} UmiCtestCoverageHost;

typedef struct UmiCtestCoverageRequirement {
    char name[UMI_TEST_NAME_CAPACITY];
    char host[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char configuration[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    /* Human description of the required environment, for example GTK display
     * or Linux renameat2. It is an annotation, not a capability probe. */
    char capability[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
} UmiCtestCoverageRequirement;

typedef enum UmiCtestCoverageOutcome {
    UMI_CTEST_COVERAGE_PASSED = 0,
    UMI_CTEST_COVERAGE_FAILED,
    UMI_CTEST_COVERAGE_TIMED_OUT,
    UMI_CTEST_COVERAGE_SKIPPED,
    UMI_CTEST_COVERAGE_DISABLED,
    UMI_CTEST_COVERAGE_NOT_RUN,
    UMI_CTEST_COVERAGE_MISSING_HOST,
    UMI_CTEST_COVERAGE_MISSING_TEST,
    UMI_CTEST_COVERAGE_SOURCE_MISMATCH,
    UMI_CTEST_COVERAGE_OUTCOME_COUNT
} UmiCtestCoverageOutcome;

/* Stable presentation labels; unknown values return "unknown". */
const char *UmiCtestCoverageOutcomeText(UmiCtestCoverageOutcome outcome);

typedef struct UmiCtestCoverageRow {
    UmiCtestCoverageRequirement requirement;
    UmiCtestCoverageOutcome outcome;
    char observed_source_identity[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
    /* False for a missing host/test or mismatched source. In those cases
     * observation is zeroed and must not be interpreted as a test outcome. */
    bool has_observation;
    UmiCtestReportSnapshot observation;
} UmiCtestCoverageRow;

typedef struct UmiCtestCoverageSummary {
    size_t requirements;
    size_t outcomes[UMI_CTEST_COVERAGE_OUTCOME_COUNT];
    size_t outstanding;
} UmiCtestCoverageSummary;

typedef struct UmiCtestCoverageReview UmiCtestCoverageReview;

/* Requirements use exact decoded names, host IDs and configurations. A host
 * observation from a different source identity never satisfies a requirement.
 * Host/configuration pairs and name/host/configuration triples must be unique.
 * Strings must be terminated, nonempty printable UTF-8; capability may be empty.
 * Counts are bounded by the constants above. Empty requirements are valid but
 * cannot satisfy RequirePassed. Input arrays may be NULL only for count zero.
 *
 * Inputs are borrowed only during creation and must stay immutable then. The
 * review owns copied rows in requirement order; destroy reports independently
 * after return. It performs no I/O, execution, capability discovery, timestamp
 * freshness check or identity authentication. Its gate covers only explicitly
 * required rows, not the complete test inventory or unlisted host results.
 * out becomes NULL on failure. Input/output storage must not overlap. */
UmiStatus UmiCtestCoverageReviewCreate(const char *source_identity,
    const UmiCtestCoverageRequirement *requirements, size_t requirement_count,
    const UmiCtestCoverageHost *hosts, size_t host_count,
    UmiCtestCoverageReview **out);
/* Borrow the exact requested source identity until review destruction. */
const char *UmiCtestCoverageReviewSourceIdentity(const UmiCtestCoverageReview *review);
UmiStatus UmiCtestCoverageReviewRead(const UmiCtestCoverageReview *review,
    UmiCtestCoverageSummary *out);
const UmiCtestCoverageRow *UmiCtestCoverageReviewAt(const UmiCtestCoverageReview *review,
    size_t index);
/* OK only for a nonempty requirement list with every row PASSED.
 * This does not replace UmiCtestReportRequirePassed for a complete run. */
UmiStatus UmiCtestCoverageRequirePassed(const UmiCtestCoverageReview *review);
/* Format 1..128 rows in requirement order as escaped text. NULL,0 queries
 * bytes excluding the terminator. Insufficient capacity leaves output unchanged
 * and reports the needed bytes; invalid arguments leave required unchanged.
 * Empty reviews accept offset zero. Other out-of-range offsets return NOT_FOUND.
 * Inputs/output must not overlap. No I/O or execution is performed. */
UmiStatus UmiCtestCoverageReviewFormat(const UmiCtestCoverageReview *review,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required);
void UmiCtestCoverageReviewDestroy(UmiCtestCoverageReview *review);
#ifdef __cplusplus
}
#endif
#endif
