/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/uat_process_fixture.h
 *
 * PURPOSE:
 *   Run named acceptance checks against an owner-configured process fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_UAT_PROCESS_FIXTURE_H
#define UMICOM_TESTING_UAT_PROCESS_FIXTURE_H

#include "umicom/testing/uat_target.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UAT_PROCESS_EXPECTED_LIMIT 8192U

typedef struct UmiUatProcessProbe {
    const char *testId;
    UmiUatOperation operation;
    const char *program;
    const char *workingDirectory;
    const char *const *arguments;
    size_t argumentCount;
    uint32_t timeoutLimitMs;
    const void *expectedStdout;
    size_t expectedStdoutLength;
} UmiUatProcessProbe;

typedef struct UmiUatProcessFixture {
    UmiUatTarget binding;
    const UmiUatProcessProbe *probes;
    size_t probeCount;
} UmiUatProcessFixture;

/** Run 1..64 named steps using the fixture owner's probe table. The plan only
 * chooses IDs, phases, and deadlines within each probe's timeout allowance.
 * Leave its program, argument text and working-directory fields empty. Probe
 * IDs must be unique. The owner supplies absolute program and directory paths
 * and a counted argument vector. Disabled or late steps must also be mapped.
 * The exact target, session and operation permissions must match before launch.
 * Missing probe IDs return NOT_FOUND. Inline commands or excessive deadlines
 * return PERMISSION_DENIED. All admission failures clear *outReport.
 *
 * The fixture runs with process-tree lifetime. Each step requires exit zero
 * and complete, byte-identical stdout. Include line endings in the expected
 * span. A zero-length span requires empty stdout. Stderr is not compared.
 * Report output contains a short comparison result without captured bytes.
 * The normal UAT report owns the accepted scope and handles cleanup and
 * cancellation. OK means a report was produced. Inspect its accepted field.
 *
 * Keep the fixture, strings, argument arrays and comparison bytes stable until
 * the call returns. Give the fixture a disposable environment and use only
 * tools and targets approved by its owner. Process lifetime manages cleanup.
 * Permissions for filesystem, device and network access belong to the host.
 */
UmiStatus UmiUatRunProcessFixture(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const UmiUatStep *steps, size_t count,
    UmiCancellationToken *cancellation, UmiCancellationToken *cleanupCancellation,
    UmiUatReport **outReport);

/* Admit one owner-selected expected exit code for every probe, in probe-table
 * order. Portable normal exits 0 through 125 are supported. A short or missing
 * table, or an out-of-range value, refuses the plan before setup can run.
 * An expected nonzero exit must still match the exact stdout and does not
 * excuse launch, cancellation, timeout or process-cleanup failures. When a
 * nonzero outcome passes, the finding's assertion exit code is zero and its
 * comparison message states the observed child exit code. The remaining
 * admission, cleanup and ownership rules are the same as the original API. */
UmiStatus UmiUatRunProcessFixtureWithExitCodes(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const int *expectedExitCodes, size_t exitCodeCount,
    const UmiUatStep *steps, size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport);

typedef struct UmiUatProcessExpectedOutcome {
    int exitCode;
    const void *stderrBytes;
    size_t stderrLength;
} UmiUatProcessExpectedOutcome;

/** Extend each owner-approved stdout expectation with an independent stderr
 * span and ordinary exit code. Supply one outcome per probe in table order.
 * Empty spans require that channel to stay empty. Each span is limited to
 * UMI_UAT_PROCESS_EXPECTED_LIMIT bytes. Both streams come from one owned
 * process and are compared before any diagnostic tail is truncated. The
 * report may identify a mismatching channel without exposing its bytes. */
UmiStatus UmiUatRunProcessFixtureWithOutcomes(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const UmiUatProcessExpectedOutcome *outcomes, size_t outcomeCount,
    const UmiUatStep *steps, size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport);

#ifdef __cplusplus
}
#endif
#endif
