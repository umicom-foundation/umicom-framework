/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_assessment.c
 * Purpose: Keep incomplete, skipped and changed-source runs visibly blocked.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include <string.h>

UmiStatus UmiReleaseQualificationClose(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity,
    const UmiReleaseSourceObservation *after, const char *toolchain)
{
    UmiStatus status = UmiQualificationMatch(run, identity);
    if (status != UMI_STATUS_OK) return status;
    if (run->closed) return UMI_STATUS_INVALID_STATE;
    if (toolchain != NULL && !UmiQualificationDigestValid(toolchain)) return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t blockers = 0U;
    if (after == NULL) blockers |= UMI_QUALIFICATION_SOURCE_UNOBSERVED;
    else if (strcmp(run->inventory, UmiReleaseSourceObservationInventoryFingerprint(after)) != 0)
        blockers |= UMI_QUALIFICATION_CONTEXT_CHANGED;
    else if (strcmp(run->source, UmiReleaseSourceObservationFingerprint(after)) != 0)
        blockers |= UMI_QUALIFICATION_SOURCE_CHANGED;
    if (toolchain == NULL) blockers |= UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED;
    else if (strcmp(run->toolchain, toolchain) != 0) blockers |= UMI_QUALIFICATION_TOOLCHAIN_CHANGED;
    /* Seal failed and incomplete attempts too. Retaining their evidence prevents
     * an executor restart from silently turning an abandoned attempt into a pass. */
    if (after != NULL) {
        memcpy(run->afterInventory, UmiReleaseSourceObservationInventoryFingerprint(after), 65U);
        memcpy(run->afterSource, UmiReleaseSourceObservationFingerprint(after), 65U);
    }
    if (toolchain != NULL) memcpy(run->afterToolchain, toolchain, 65U);
    run->closeBlockers = blockers;
    run->closed = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationDescribe(const UmiReleaseQualification *run,
    UmiReleaseQualificationSummary *outSummary)
{
    if (run == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseQualificationSummary summary = {0};
    summary.targets = run->selection.targets;
    summary.tests = run->selection.tests;
    summary.closed = run->closed;
    summary.buildRecorded = run->buildRecorded;
    summary.testProcessRecorded = run->testProcessRecorded;
    summary.blockers = run->closeBlockers;
    summary.executionStatus = run->executionStatus;
    if (run->executionStatus != UMI_STATUS_OK) summary.blockers |= UMI_QUALIFICATION_EXECUTION_INTERRUPTED;
    if (run->testProcessRecorded && run->testProcess.outcome != UMI_QUALIFICATION_PASSED)
        summary.blockers |= UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED;
    if (!run->closed) summary.blockers |= UMI_QUALIFICATION_OPEN;
    if (!run->buildRecorded) summary.blockers |= UMI_QUALIFICATION_BUILD_MISSING;
    else if (run->build.outcome != UMI_QUALIFICATION_PASSED)
        summary.blockers |= UMI_QUALIFICATION_BUILD_NOT_PASSED;
    if (summary.tests == 0U) summary.blockers |= UMI_QUALIFICATION_NO_TEST_COVERAGE;
    for (size_t i = 0U; i < summary.tests; ++i) {
        if (!run->tests[i].recorded) { ++summary.missing; continue; }
        ++summary.recorded;
        switch (run->tests[i].result.outcome) {
        case UMI_QUALIFICATION_NOT_RUN: ++summary.notRun; break;
        case UMI_QUALIFICATION_PASSED: ++summary.passed; break;
        case UMI_QUALIFICATION_FAILED: ++summary.failed; break;
        case UMI_QUALIFICATION_SKIPPED: ++summary.skipped; break;
        case UMI_QUALIFICATION_CANCELLED: ++summary.cancelled; break;
        case UMI_QUALIFICATION_TIMED_OUT: ++summary.timedOut; break;
        case UMI_QUALIFICATION_DISABLED: ++summary.disabled; break;
        }
    }
    if (summary.missing != 0U) summary.blockers |= UMI_QUALIFICATION_TESTS_MISSING;
    if (summary.recorded != summary.passed) summary.blockers |= UMI_QUALIFICATION_TESTS_NOT_PASSED;
    summary.readyForOwnerReview = summary.blockers == 0U;
    *outSummary = summary;
    return UMI_STATUS_OK;
}

/* A workflow failure may happen after the final passing test, for example
 * cancellation or an output/ownership failure. Retain that terminal cause
 * separately; it must not be erased by otherwise successful test counters. */
UmiStatus UmiReleaseQualificationInterrupt(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, UmiStatus cause)
{
    UmiStatus status = UmiQualificationMatch(run, identity);
    if (status != UMI_STATUS_OK) return status;
    if (cause == UMI_STATUS_OK || (unsigned)cause > (unsigned)UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (run->closed) return UMI_STATUS_INVALID_STATE;
    if (run->executionStatus != UMI_STATUS_OK) return UMI_STATUS_ALREADY_EXISTS;
    run->executionStatus = cause;
    return UMI_STATUS_OK;
}
