/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/automation_session.h
 *
 * PURPOSE:
 *   Run an existing GUI scenario one step at a time, cancel between steps,
 *   and retain every planned action in an exportable acceptance report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_AUTOMATION_SESSION_H
#define UMICOM_UI_AUTOMATION_SESSION_H

#include "umicom/ui/automation.h"
#include "umicom/ui/interaction_recording.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A completed sequence can still contain failed checks. STOPPED means an
 * earlier failure prevented later actions; CANCELLED never means accepted.
 * See examples/gui_recording/check_sessions.c for a complete Notes example. */
typedef enum UmiUiAutomationSessionState {
    UMI_UI_AUTOMATION_SESSION_READY = 1,
    UMI_UI_AUTOMATION_SESSION_RUNNING = 2,
    UMI_UI_AUTOMATION_SESSION_COMPLETED = 3,
    UMI_UI_AUTOMATION_SESSION_STOPPED = 4,
    UMI_UI_AUTOMATION_SESSION_CANCELLED = 5
} UmiUiAutomationSessionState;

/** NOT_RUN retains the planned action without inventing a driver result. */
typedef enum UmiUiAutomationStepState {
    UMI_UI_AUTOMATION_STEP_NOT_RUN = 0,
    UMI_UI_AUTOMATION_STEP_PASSED = 1,
    UMI_UI_AUTOMATION_STEP_FAILED = 2
} UmiUiAutomationStepState;

/** Copied progress. planned = attempted + notRun; attempted = passed + failed.
 * stepInProgress says that a driver callback is executing on the owner thread.
 * Identifiers are metadata: do not put credentials or document text in them. */
typedef struct UmiUiAutomationSessionSnapshot {
    char runId[UMI_UI_ID_CAPACITY];
    char scenarioId[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char driverId[UMI_UI_ID_CAPACITY];
    UmiUiAutomationSessionState state;
    size_t planned;
    size_t attempted;
    size_t passed;
    size_t failed;
    size_t notRun;
    int stepInProgress;
    int cancellationRequested;
} UmiUiAutomationSessionSnapshot;

typedef struct UmiUiAutomationSession UmiUiAutomationSession;

/** Copy a nonempty validated scenario and the driver interface. runId must be
 * a valid stable UI identifier; the caller chooses a distinct ID for each run.
 * The scenario may be changed or destroyed after success. The driver context
 * is BORROWED: it must remain alive until no more Advance calls are possible.
 * Creation performs no actions. On failure *outSession is NULL.
 * All session operations use one owning thread; this API creates no threads. */
UmiStatus UmiUiAutomationSessionCreate(const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario, const char *runId,
    UmiUiAutomationSession **outSession);

/** Perform at most one action through the existing driver and observation
 * checks. OK means the step was recorded; inspect the report for acceptance.
 * Call from an owner-thread event-loop tick to yield BETWEEN actions. A single
 * callback may block; this API cannot interrupt it or enforce a hard timeout.
 * Nested Advance returns BUSY. Terminal sessions return INVALID_STATE. */
UmiStatus UmiUiAutomationSessionAdvance(UmiUiAutomationSession *session);

/** Stop further actions. A request made from the active driver callback takes
 * effect after that callback returns and its result is recorded. Cancellation
 * does not undo completed actions. Repeating it or cancelling a terminal run
 * is harmless and does not rewrite the terminal result. Owner thread only. */
UmiStatus UmiUiAutomationSessionCancel(UmiUiAutomationSession *session);

/** Destroy session and its report. NULL succeeds. Returns BUSY inside the
 * session's driver callback so reentrant teardown cannot free active memory. */
UmiStatus UmiUiAutomationSessionDestroy(UmiUiAutomationSession *session);

/** Borrow the session's report until session destruction. Do not call the
 * legacy report_destroy on this pointer. Use report accessors to copy data. */
const UmiUiAutomationReport *UmiUiAutomationSessionReport(
    const UmiUiAutomationSession *session);

/** Copy metadata and progress from a session or legacy synchronous report.
 * Clears the output on failure. Snapshotting does not execute a driver. */
UmiStatus UmiUiAutomationReportSnapshot(const UmiUiAutomationReport *report,
    UmiUiAutomationSessionSnapshot *outSnapshot);

/** Copy any PLANNED row (zero-based), including unrun rows. Outputs are cleared
 * on failure. A NOT_RUN row has the planned step but no meaningful status or
 * observation: always inspect outState before reading those fields.
 * Legacy report_at continues to expose only attempted rows. */
UmiStatus UmiUiAutomationReportStep(const UmiUiAutomationReport *report,
    size_t index, UmiUiAutomationStepState *outState,
    UmiUiAutomationStepResult *outResult);

/** Export a COPIED snapshot through the existing recording write callback.
 * includeText=0 omits scenario title, step values, driver messages and observed
 * text. includeText=1 explicitly includes them; review such reports before
 * sharing. Malformed driver text is replaced, never emitted as broken UTF-8.
 * The first sink error is returned and no further writes are attempted.
 * These functions do not open files or upload anything. The caller closes its
 * own sink and may cancel/destroy the original session from a sink callback:
 * export uses its independent snapshot, not the now-changing live report. */
UmiStatus UmiUiAutomationReportWriteJson(const UmiUiAutomationReport *report,
    int includeText, UmiUiRecordingWriteFn write, void *context);
UmiStatus UmiUiAutomationReportWriteHtml(const UmiUiAutomationReport *report,
    int includeText, UmiUiRecordingWriteFn write, void *context);
/** JUnit contains every planned step plus ONE run-completion testcase. Unrun
 * steps are skipped; the completion case fails for an empty/incomplete/stopped/
 * cancelled run. Thus cancellation cannot look like a green skipped-only run.
 * No timing values are invented. Check RequireSuccess for process exit status. */
UmiStatus UmiUiAutomationReportWriteJUnit(const UmiUiAutomationReport *report,
    int includeText, UmiUiRecordingWriteFn write, void *context);

#ifdef __cplusplus
}
#endif
#endif
