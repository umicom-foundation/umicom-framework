/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification.h
 * Purpose: Bind scoped execution evidence to owned selection and source identities.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_H
#define UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_H
#include "umicom/distribution/runtime/impact.h"
#include "umicom/distribution/runtime/source_observation.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseQualification UmiReleaseQualification;
typedef enum UmiReleaseQualificationOutcome {
    UMI_QUALIFICATION_NOT_RUN,
    UMI_QUALIFICATION_PASSED,
    UMI_QUALIFICATION_FAILED,
    UMI_QUALIFICATION_SKIPPED,
    UMI_QUALIFICATION_CANCELLED,
    UMI_QUALIFICATION_TIMED_OUT,
    UMI_QUALIFICATION_DISABLED
} UmiReleaseQualificationOutcome;

/** Evidence is supplied by the executor, not independently authenticated here.
 * logSha256 binds the captured output bytes; it is mandatory even for empty logs.
 * PASSED requires status OK and zero exitCode storage. A passing build also
 * requires exitCodeKnown; test observations can lack individual exit codes.
 * Other outcomes remain distinct.
 * A failed attempt cannot be overwritten by a later pass; create another run. */
typedef struct UmiReleaseQualificationResult {
    UmiReleaseQualificationOutcome outcome;
    UmiStatus status;
    int exitCode;
    uint64_t durationMs;
    char logSha256[65];
    /* JUnit reports do not contain each executable exit code. Unknown codes
     * use zero storage with this flag false; they are not asserted as exit zero. */
    bool exitCodeKnown;
} UmiReleaseQualificationResult;
typedef struct UmiReleaseQualificationIdentity {
    char execution[65]; /* Unique host-generated attempt identity; never reused. */
    char plan[65];      /* Deterministic selection, source and toolchain identity. */
} UmiReleaseQualificationIdentity;

typedef enum UmiReleaseQualificationBlocker {
    UMI_QUALIFICATION_OPEN = 1U,
    UMI_QUALIFICATION_BUILD_MISSING = 2U,
    UMI_QUALIFICATION_BUILD_NOT_PASSED = 4U,
    UMI_QUALIFICATION_TESTS_MISSING = 8U,
    UMI_QUALIFICATION_TESTS_NOT_PASSED = 16U,
    UMI_QUALIFICATION_NO_TEST_COVERAGE = 32U,
    UMI_QUALIFICATION_SOURCE_UNOBSERVED = 64U,
    UMI_QUALIFICATION_SOURCE_CHANGED = 128U,
    UMI_QUALIFICATION_CONTEXT_CHANGED = 256U,
    UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED = 512U,
    UMI_QUALIFICATION_TOOLCHAIN_CHANGED = 1024U,
    UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED = 2048U,
    UMI_QUALIFICATION_EXECUTION_INTERRUPTED = 4096U
} UmiReleaseQualificationBlocker;
typedef struct UmiReleaseQualificationSummary {
    size_t targets, tests, recorded, missing;
    size_t passed, failed, skipped, notRun, cancelled, timedOut;
    size_t disabled;
    uint32_t blockers;
    bool closed, buildRecorded, readyForOwnerReview;
    bool testProcessRecorded;
    UmiStatus executionStatus; /* First explicitly recorded interruption; OK if none. */
} UmiReleaseQualificationSummary;

/** Copy the complete plan and fingerprints. Inputs can be destroyed on return.
 * Source and plan must come from the exact same configured inventory. No-work
 * plans are UNAVAILABLE, never a passing qualification. A plan without tests
 * can be recorded but will always have NO_TEST_COVERAGE.
 *
 * execution and toolchain are lowercase SHA-256 strings. The host must generate
 * a new execution identity for each attempt and hash the compiler, configuration,
 * commands and relevant environment into toolchain. No process or file is read
 * here. Caller assertions are not provenance, signatures or build authority.
 * *outRun must be NULL; every failure preserves it. Owned name storage is bounded
 * to 64 MiB. A single owner must serialize mutation; destruction cannot race use. */
UmiStatus UmiReleaseQualificationCreate(const UmiReleaseImpactPlan *plan,
    const UmiReleaseSourceObservation *before, const char *execution,
    const char *toolchain, UmiReleaseQualification **outRun);
void UmiReleaseQualificationDestroy(UmiReleaseQualification *run);
UmiStatus UmiReleaseQualificationGetIdentity(const UmiReleaseQualification *run,
    UmiReleaseQualificationIdentity *outIdentity);
/** Borrow copied plan items, valid until destruction. NULL/out-of-range returns NULL. */
size_t UmiReleaseQualificationCount(const UmiReleaseQualification *run);
const UmiReleaseImpactItem *UmiReleaseQualificationAt(
    const UmiReleaseQualification *run, size_t index);

/** Record aggregate build outcome once for all selected targets. A supplied
 * identity must match both the attempt and plan. Nothing is launched here.
 * Duplicate results are ALREADY_EXISTS. Closed runs are immutable. */
UmiStatus UmiReleaseQualificationRecordBuild(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity,
    const UmiReleaseQualificationResult *result);
/** Exact selected test name, no normalization or truncation. Build evidence must
 * already exist. Tests after a failed build remain visible but cannot make the
 * run ready: their executables may be stale. Unknown names return NOT_FOUND.
 * Identity/result errors and duplicate records preserve all existing evidence. */
UmiStatus UmiReleaseQualificationRecordTest(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, const char *testName,
    const UmiReleaseQualificationResult *result);
/** Read copied results. Missing evidence returns NOT_FOUND and preserves output. */
UmiStatus UmiReleaseQualificationGetBuild(const UmiReleaseQualification *run,
    UmiReleaseQualificationResult *outResult);
UmiStatus UmiReleaseQualificationGetTest(const UmiReleaseQualification *run,
    const char *testName, UmiReleaseQualificationResult *outResult);

/** Record a terminal orchestration failure before sealing. cause must be non-OK.
 * The first cause is retained; duplicates return ALREADY_EXISTS. Further build/test
 * records are refused, while Close still records after observations. This prevents
 * cancellation after passing tests from producing a ready summary. */
UmiStatus UmiReleaseQualificationInterrupt(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, UmiStatus cause);

/** Seal an attempt even when incomplete or failed. NULL after/toolchain means
 * unavailable evidence and blocks review; mismatches are recorded as blockers.
 * Invalid non-NULL toolchain syntax is rejected without closing the attempt.
 * Before/after matches are observations, not an atomic snapshot or proof that
 * compiler input stayed unchanged between reads. Unlisted inputs are not covered.
 * No result can be appended after sealing. Repeated close returns INVALID_STATE. */
UmiStatus UmiReleaseQualificationClose(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity,
    const UmiReleaseSourceObservation *after, const char *toolchain);
/** A ready summary means all recorded checks satisfy this scoped contract.
 * It does not certify a product, replace the full suite, or authorize publication. */
UmiStatus UmiReleaseQualificationDescribe(const UmiReleaseQualification *run,
    UmiReleaseQualificationSummary *outSummary);

/** Borrow the original observed-source fingerprint; NULL returns NULL. */
const char *UmiReleaseQualificationSourceFingerprint(const UmiReleaseQualification *run);

typedef struct UmiReleaseQualificationNamedResult {
    const char *name;
    UmiReleaseQualificationResult result;
} UmiReleaseQualificationNamedResult;
/** Record a batch atomically. Every name/result and duplicate is checked before
 * mutation. An optional process result covers the entire selected test set, so
 * supplying it requires every selected test exactly once. It is copied atomically
 * with the results. Unknown per-test exit codes require this process evidence.
 * A failed process blocks review even when its report says every test passed.
 * A zero-sized batch without process evidence is a no-op, not coverage. Names
 * and records are borrowed only for the call; failed preflight changes nothing. */
UmiStatus UmiReleaseQualificationRecordTests(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity,
    const UmiReleaseQualificationNamedResult *results, size_t count,
    const UmiReleaseQualificationResult *testProcess);
/** Missing aggregate process evidence returns NOT_FOUND, preserving output. */
UmiStatus UmiReleaseQualificationGetTestProcess(const UmiReleaseQualification *run,
    UmiReleaseQualificationResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
