/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/uat_workflow.h
 *
 * PURPOSE:
 *   Orchestrate application-independent setup, actions, assertions and cleanup
 *   while retaining every planned outcome and preserving primary failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_UAT_WORKFLOW_H
#define UMICOM_TESTING_UAT_WORKFLOW_H
#include "umicom/testing/case.h"
#include "umicom/testing/result.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UAT_MAX_STEPS 64U

typedef enum UmiUatPhase {
    UMI_UAT_SETUP = 1,
    UMI_UAT_ACTION = 2,
    UMI_UAT_ASSERT = 3,
    UMI_UAT_CLEANUP = 4
} UmiUatPhase;

typedef struct UmiUatStep {
    UmiUatPhase phase;
    UmiTestCase test;
} UmiUatStep;

typedef struct UmiUatFinding {
    UmiUatPhase phase;
    int attempted;
    UmiTestResult result;
} UmiUatFinding;

typedef struct UmiUatSummary {
    size_t planned;
    size_t attempted;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t notRun;
    size_t cleanupFailed;
    int cancellationRequested;
    int accepted;
} UmiUatSummary;

typedef struct UmiUatReport UmiUatReport;

/** Adapter for UI, API, data, compilation or other test operations. The case
 * and result are borrowed only during the call; neither may be retained. The
 * adapter must honor timeout_ms and cancellation and return a terminal result.
 * It must not throw across this C boundary. A separate process watchdog is
 * still required for a blocked or crashed adapter. context stays caller-owned.
 * A success result requires explicit checks; an action alone proves no outcome.
 */
typedef UmiStatus (*UmiUatExecuteFn)(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result);

/** Copy and validate 1..64 planned steps before invoking an adapter. IDs must
 * be unique, every timeout nonzero, and at least one ASSERT is required. SETUP
 * precedes the body; ACTION/ASSERT may interleave; CLEANUP comes last. All rows
 * are required. A failure or skipped prerequisite blocks later body rows.
 *
 * Once any non-cleanup adapter has been invoked, all enabled cleanup rows are
 * attempted even after failure/cancellation. Cleanup must be idempotent and
 * tolerate partially completed setup. cleanupCancellation is a separate caller
 * token (or NULL); the cancelled primary token is never reset or substituted.
 * If no work started, cleanup is not executed. A cancelled cleanup token keeps
 * remaining cleanup rows NOT_RUN and the overall result unaccepted.
 *
 * OK means a report was produced, not that the tests passed: inspect accepted.
 * Validation/allocation failures clear *outReport and invoke no adapters. Input
 * steps are copied before callbacks; callers serialize the entire operation.
 * This orchestrator is not a filesystem/database security sandbox. Adapters
 * must enforce their configured disposable environment and resource scope.
 */
UmiStatus UmiUatRun(const UmiUatStep *steps, size_t count, UmiUatExecuteFn execute,
    void *context, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport);
void UmiUatReportDestroy(UmiUatReport *report);
/** Write JSON using the shared validated JSON writer. Output is caller-owned
 * and must not alias report storage. includeOutput must be 0 or 1: zero omits
 * adapter output (which may contain sensitive data). No file is opened. Parse
 * the buffer only on OK; any error clears it instead of returning partial JSON.
 */
UmiStatus UmiUatReportJson(const UmiUatReport *report, int includeOutput,
    char *buffer, size_t capacity);
UmiStatus UmiUatReportSummary(const UmiUatReport *report, UmiUatSummary *outSummary);
UmiStatus UmiUatReportFinding(const UmiUatReport *report, size_t index,
    UmiUatFinding *outFinding);
/** Optional adapter using the existing portable test-process runner. It inherits
 * that runner's quoted argument-text format and captured-output limits. Use a
 * dedicated process adapter when full argv/structured artifact support is needed.
 */
UmiStatus UmiUatExecuteProcess(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result);
#ifdef __cplusplus
}
#endif
#endif
