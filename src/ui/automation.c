/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/automation.c
 *
 * PURPOSE:
 *   Store and execute safe, bounded desktop user-acceptance scenarios without
 *   coupling their steps or reports to a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/automation.h"
#include "umicom/ui/automation_session.h"

#include <stdlib.h>
#include <string.h>

#include "ui_internal.h"

struct UmiUiAutomationScenario {
    char scenario_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    UmiUiAutomationStep steps[UMI_UI_AUTOMATION_STEP_MAX];
    size_t step_count;
    int continue_on_failure;
};

struct UmiUiAutomationReport {
    char scenario_id[UMI_UI_ID_CAPACITY];
    char driver_id[UMI_UI_ID_CAPACITY];
    UmiUiAutomationStepResult results[UMI_UI_AUTOMATION_STEP_MAX];
    size_t total;
    size_t passed;
    size_t failed;
    char runId[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    size_t planned;
    UmiUiAutomationSessionState state;
    int stepInProgress;
    int cancellationRequested;
};

struct UmiUiAutomationSession {
    UmiUiAutomationDriver driver;
    UmiUiAutomationReport *report;
    int continueOnFailure;
};

/* Check fixed text storage before any string function searches past its end. */
static int automation_has_terminator(const char *text, size_t capacity)
{
    return umi_ui_text_is_valid_utf8(text, capacity);
}

/* Confirm that an operation is one of the public values before a driver sees it. */
static int automation_operation_is_valid(UmiUiAutomationOperation operation)
{
    return operation >= UMI_UI_AUTOMATION_FOCUS &&
           operation <= UMI_UI_AUTOMATION_ASSERT_SELECTED;
}

/*
 * Validate a step at the storage boundary. Stable identifiers are mandatory
 * because captions can be translated and screen coordinates move with layout.
 */
UmiStatus UmiUiAutomationStepValidate(const UmiUiAutomationStep *step)
{
    if (step == NULL ||
        !automation_has_terminator(step->step_id, sizeof(step->step_id)) ||
        !automation_has_terminator(step->target_id, sizeof(step->target_id)) ||
        !automation_has_terminator(step->value, sizeof(step->value)) ||
        !umi_ui_id_is_valid(step->step_id) ||
        !umi_ui_id_is_valid(step->target_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!automation_operation_is_valid(step->operation)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (step->operation >= UMI_UI_AUTOMATION_ASSERT_VISIBLE &&
        strcmp(step->value, "true") != 0 && strcmp(step->value, "false") != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* A driver returning OK cannot turn a contradictory observation into a pass. */
UmiStatus UmiUiAutomationObservationCheck(const UmiUiAutomationStep *step,
    const UmiUiAutomationObservation *observation)
{
    int actual = 0;
    int expected;
    UmiStatus status = UmiUiAutomationStepValidate(step);
    if (status != UMI_STATUS_OK || observation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (step->operation != UMI_UI_AUTOMATION_ASSERT_TEXT &&
        step->operation != UMI_UI_AUTOMATION_WAIT_VISIBLE &&
        step->operation != UMI_UI_AUTOMATION_WAIT_ENABLED &&
        step->operation < UMI_UI_AUTOMATION_ASSERT_VISIBLE) return UMI_STATUS_OK;
    if (!automation_has_terminator(observation->target_id, sizeof(observation->target_id)) ||
        !automation_has_terminator(observation->role_name, sizeof(observation->role_name)) ||
        !automation_has_terminator(observation->text, sizeof(observation->text)) ||
        observation->visible < 0 || observation->visible > 1 ||
        observation->enabled < 0 || observation->enabled > 1 ||
        observation->focused < 0 || observation->focused > 1 ||
        observation->selected < 0 || observation->selected > 1)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(observation->target_id, step->target_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (step->operation == UMI_UI_AUTOMATION_ASSERT_TEXT)
        return strcmp(observation->text, step->value) == 0
            ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    switch (step->operation) {
        case UMI_UI_AUTOMATION_WAIT_VISIBLE:
        case UMI_UI_AUTOMATION_ASSERT_VISIBLE: actual = observation->visible; break;
        case UMI_UI_AUTOMATION_WAIT_ENABLED:
        case UMI_UI_AUTOMATION_ASSERT_ENABLED: actual = observation->enabled; break;
        case UMI_UI_AUTOMATION_ASSERT_FOCUSED: actual = observation->focused; break;
        case UMI_UI_AUTOMATION_ASSERT_SELECTED: actual = observation->selected; break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    expected = step->operation == UMI_UI_AUTOMATION_WAIT_VISIBLE ||
        step->operation == UMI_UI_AUTOMATION_WAIT_ENABLED ||
        strcmp(step->value, "true") == 0;
    return actual == expected ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Allocate one private scenario so its large bounded step list stays off the stack. */
UmiStatus umi_ui_automation_scenario_create(
    const char *scenario_id,
    const char *title,
    UmiUiAutomationScenario **out_scenario)
{
    UmiUiAutomationScenario *scenario;

    if (out_scenario == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_scenario = NULL;

    if (!automation_has_terminator(scenario_id, UMI_UI_ID_CAPACITY) ||
        !automation_has_terminator(title, UMI_UI_TEXT_CAPACITY) ||
        !umi_ui_id_is_valid(scenario_id) || title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Allocate the large step collection on the heap to protect small stacks. */
    scenario = (UmiUiAutomationScenario *)calloc(1U, sizeof(*scenario));
    if (scenario == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    if (!umi_ui_copy_text(
            scenario->scenario_id,
            sizeof(scenario->scenario_id),
            scenario_id) ||
        !umi_ui_copy_text(scenario->title, sizeof(scenario->title), title)) {
        free(scenario);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_scenario = scenario;
    return UMI_STATUS_OK;
}

/* Release all storage owned by a scenario in one operation. */
void umi_ui_automation_scenario_destroy(UmiUiAutomationScenario *scenario)
{
    free(scenario);
}

/* Store whether independent steps may continue after an earlier step fails. */
UmiStatus umi_ui_automation_scenario_set_continue_on_failure(
    UmiUiAutomationScenario *scenario,
    int continue_on_failure)
{
    if (scenario == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Store a predictable Boolean even when the caller passes another integer. */
    scenario->continue_on_failure = continue_on_failure != 0;
    return UMI_STATUS_OK;
}

/* Validate and copy one step so caller-owned memory is never retained. */
UmiStatus umi_ui_automation_scenario_add(
    UmiUiAutomationScenario *scenario,
    const UmiUiAutomationStep *step)
{
    size_t index;
    UmiStatus status;

    if (scenario == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiUiAutomationStepValidate(step);
    if (status != UMI_STATUS_OK) return status;

    if (scenario->step_count >= UMI_UI_AUTOMATION_STEP_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Reject duplicate step names so reports always identify one exact action. */
    for (index = 0U; index < scenario->step_count; ++index) {
        if (strcmp(scenario->steps[index].step_id, step->step_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    scenario->steps[scenario->step_count] = *step;
    scenario->step_count += 1U;
    return UMI_STATUS_OK;
}

/* Read the bounded step count without exposing the scenario representation. */
size_t umi_ui_automation_scenario_count(const UmiUiAutomationScenario *scenario)
{
    return scenario != NULL ? scenario->step_count : 0U;
}

/* Copy one step into caller storage after checking the requested index. */
UmiStatus umi_ui_automation_scenario_at(
    const UmiUiAutomationScenario *scenario,
    size_t index,
    UmiUiAutomationStep *out_step)
{
    if (scenario == NULL || out_step == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= scenario->step_count) return UMI_STATUS_NOT_FOUND;

    *out_step = scenario->steps[index];
    return UMI_STATUS_OK;
}

/* Reject adapters whose size, version, name or callback does not match the contract. */
UmiStatus umi_ui_automation_driver_validate(const UmiUiAutomationDriver *driver)
{
    if (driver == NULL ||
        driver->structure_size < (uint32_t)sizeof(*driver) ||
        driver->api_version != UMI_UI_AUTOMATION_API_VERSION ||
        !automation_has_terminator(driver->driver_id, sizeof(driver->driver_id)) ||
        !umi_ui_id_is_valid(driver->driver_id) || driver->perform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* Both synchronous runs and paced sessions execute this same checked step. */
static void PerformStep(const UmiUiAutomationDriver *driver,
    UmiUiAutomationStepResult *result)
{
    result->status = driver->perform(
        driver->context,
        &result->step,
        &result->observation,
        result->message,
        sizeof(result->message));
    /* Validate before repairing report terminators: otherwise a truncated
     * or malformed adapter response could falsely satisfy an assertion. */
    if (result->status == UMI_STATUS_OK) {
        UmiStatus checked = UmiUiAutomationObservationCheck(
            &result->step, &result->observation);
        if (checked != UMI_STATUS_OK) {
            result->status = checked;
            (void)umi_ui_copy_text(result->message, sizeof(result->message),
                "The reported control state does not satisfy the assertion.");
        }
    }
    /* A third-party adapter cannot leave report strings unterminated. */
    result->message[sizeof(result->message) - 1U] = '\0';
    result->observation.target_id[
        sizeof(result->observation.target_id) - 1U] = '\0';
    result->observation.role_name[
        sizeof(result->observation.role_name) - 1U] = '\0';
    result->observation.text[
        sizeof(result->observation.text) - 1U] = '\0';
}

/* Allocate all rows before the first action. No allocation failure can lose an
 * already-executed step. Legacy callers alone may create an empty report. */
static UmiStatus CreateSession(const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario, const char *runId, int allowEmpty,
    UmiUiAutomationSession **outSession)
{
    UmiUiAutomationSession *session;
    UmiUiAutomationReport *report;
    size_t index;
    UmiStatus status;
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    if (scenario == NULL || (!allowEmpty && scenario->step_count == 0U) ||
        !automation_has_terminator(runId, UMI_UI_ID_CAPACITY) ||
        !umi_ui_id_is_valid(runId)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_automation_driver_validate(driver);
    if (status != UMI_STATUS_OK) return status;
    session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    report = calloc(1U, sizeof(*report));
    if (report == NULL) { free(session); return UMI_STATUS_OUT_OF_MEMORY; }
    /* Inputs are already validated, so bounded copies cannot truncate here. */
    (void)umi_ui_copy_text(report->scenario_id, sizeof(report->scenario_id), scenario->scenario_id);
    (void)umi_ui_copy_text(report->title, sizeof(report->title), scenario->title);
    (void)umi_ui_copy_text(report->runId, sizeof(report->runId), runId);
    (void)umi_ui_copy_text(report->driver_id, sizeof(report->driver_id), driver->driver_id);
    report->planned = scenario->step_count;
    report->state = report->planned == 0U ? UMI_UI_AUTOMATION_SESSION_COMPLETED
        : UMI_UI_AUTOMATION_SESSION_READY;
    for (index = 0U; index < report->planned; ++index)
        report->results[index].step = scenario->steps[index];
    session->driver = *driver;
    session->report = report;
    session->continueOnFailure = scenario->continue_on_failure;
    *outSession = session;
    return UMI_STATUS_OK;
}

/* A new run captures its plan without invoking any application operation. */
UmiStatus UmiUiAutomationSessionCreate(const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario, const char *runId,
    UmiUiAutomationSession **outSession)
{
    return CreateSession(driver, scenario, runId, 0, outSession);
}

/* Publish one attempted result, then decide whether the next step may run. */
UmiStatus UmiUiAutomationSessionAdvance(UmiUiAutomationSession *session)
{
    UmiUiAutomationReport *report;
    UmiUiAutomationStepResult *result;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    report = session->report;
    if (report->stepInProgress) return UMI_STATUS_BUSY;
    if (report->state != UMI_UI_AUTOMATION_SESSION_READY &&
        report->state != UMI_UI_AUTOMATION_SESSION_RUNNING) return UMI_STATUS_INVALID_STATE;
    report->state = UMI_UI_AUTOMATION_SESSION_RUNNING;
    report->stepInProgress = 1;
    result = &report->results[report->total];
    PerformStep(&session->driver, result);
    report->stepInProgress = 0;
    report->total += 1U;
    if (result->status == UMI_STATUS_OK) report->passed += 1U;
    else report->failed += 1U;
    /* Preserve the attempted result even when it requested cancellation. */
    if (report->cancellationRequested)
        report->state = UMI_UI_AUTOMATION_SESSION_CANCELLED;
    else if (result->status != UMI_STATUS_OK && !session->continueOnFailure)
        report->state = UMI_UI_AUTOMATION_SESSION_STOPPED;
    else if (report->total == report->planned)
        report->state = UMI_UI_AUTOMATION_SESSION_COMPLETED;
    return UMI_STATUS_OK;
}

/* Keep completed work intact while preventing any further planned action. */
UmiStatus UmiUiAutomationSessionCancel(UmiUiAutomationSession *session)
{
    UmiUiAutomationReport *report;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    report = session->report;
    if (report->state == UMI_UI_AUTOMATION_SESSION_READY ||
        report->state == UMI_UI_AUTOMATION_SESSION_RUNNING) {
        report->cancellationRequested = 1;
        if (!report->stepInProgress) report->state = UMI_UI_AUTOMATION_SESSION_CANCELLED;
    }
    return UMI_STATUS_OK;
}

/* Refuse teardown during a driver call that still refers to this session. */
UmiStatus UmiUiAutomationSessionDestroy(UmiUiAutomationSession *session)
{
    if (session == NULL) return UMI_STATUS_OK;
    if (session->report != NULL && session->report->stepInProgress) return UMI_STATUS_BUSY;
    free(session->report);
    free(session);
    return UMI_STATUS_OK;
}

/* Return a borrowed view; the session keeps ownership of report storage. */
const UmiUiAutomationReport *UmiUiAutomationSessionReport(
    const UmiUiAutomationSession *session)
{
    return session != NULL ? session->report : NULL;
}

/* Copy progress without exposing the mutable report or driver interface. */
UmiStatus UmiUiAutomationReportSnapshot(const UmiUiAutomationReport *report,
    UmiUiAutomationSessionSnapshot *outSnapshot)
{
    if (outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSnapshot, 0, sizeof(*outSnapshot));
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(outSnapshot->runId, report->runId, sizeof(outSnapshot->runId));
    memcpy(outSnapshot->scenarioId, report->scenario_id, sizeof(outSnapshot->scenarioId));
    memcpy(outSnapshot->title, report->title, sizeof(outSnapshot->title));
    memcpy(outSnapshot->driverId, report->driver_id, sizeof(outSnapshot->driverId));
    outSnapshot->state = report->state;
    outSnapshot->planned = report->planned;
    outSnapshot->attempted = report->total;
    outSnapshot->passed = report->passed;
    outSnapshot->failed = report->failed;
    outSnapshot->notRun = report->planned - report->total;
    outSnapshot->stepInProgress = report->stepInProgress;
    outSnapshot->cancellationRequested = report->cancellationRequested;
    return UMI_STATUS_OK;
}

/* Unattempted rows retain only their plan, never an invented observation. */
UmiStatus UmiUiAutomationReportStep(const UmiUiAutomationReport *report,
    size_t index, UmiUiAutomationStepState *outState,
    UmiUiAutomationStepResult *outResult)
{
    if (outState != NULL) *outState = UMI_UI_AUTOMATION_STEP_NOT_RUN;
    if (outResult != NULL) memset(outResult, 0, sizeof(*outResult));
    if (report == NULL || outState == NULL || outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= report->planned) return UMI_STATUS_NOT_FOUND;
    if (index < report->total) {
        *outResult = report->results[index];
        *outState = outResult->status == UMI_STATUS_OK
            ? UMI_UI_AUTOMATION_STEP_PASSED : UMI_UI_AUTOMATION_STEP_FAILED;
    } else {
        /* A callback may be populating this slot: publish only its plan. */
        outResult->step = report->results[index].step;
    }
    return UMI_STATUS_OK;
}

/* Execute scenario steps in order and retain every attempted result as evidence.
 * The old entry point still blocks and permits empty scenarios. It now shares
 * the exact executor with paced sessions, preserving one assertion policy. */
UmiStatus umi_ui_automation_run(const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario, UmiUiAutomationReport **out_report)
{
    UmiUiAutomationSession *session = NULL;
    UmiStatus status;
    if (out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_report = NULL;
    if (scenario == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = CreateSession(driver, scenario, scenario->scenario_id, 1, &session);
    if (status != UMI_STATUS_OK) return status;
    while (session->report->state == UMI_UI_AUTOMATION_SESSION_READY ||
           session->report->state == UMI_UI_AUTOMATION_SESSION_RUNNING) {
        status = UmiUiAutomationSessionAdvance(session);
        if (status != UMI_STATUS_OK) { (void)UmiUiAutomationSessionDestroy(session); return status; }
    }
    *out_report = session->report;
    session->report = NULL; /* Transfer report ownership to the legacy caller. */
    (void)UmiUiAutomationSessionDestroy(session);
    return UMI_STATUS_OK;
}

/* Release the heap-owned report after its results have been consumed. */
void umi_ui_automation_report_destroy(UmiUiAutomationReport *report)
{
    free(report);
}

/* Return the number of steps that reached the driver. */
size_t umi_ui_automation_report_total(const UmiUiAutomationReport *report)
{
    return report != NULL ? report->total : 0U;
}

/* Return the number of actions and assertions that completed successfully. */
size_t umi_ui_automation_report_passed(const UmiUiAutomationReport *report)
{
    return report != NULL ? report->passed : 0U;
}

/* Return the number of attempted steps that did not meet their expectation. */
size_t umi_ui_automation_report_failed(const UmiUiAutomationReport *report)
{
    return report != NULL ? report->failed : 0U;
}

/* Copy one durable result without allowing callers to modify report storage. */
UmiStatus umi_ui_automation_report_at(
    const UmiUiAutomationReport *report,
    size_t index,
    UmiUiAutomationStepResult *out_result)
{
    if (report == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= report->total) return UMI_STATUS_NOT_FOUND;

    *out_result = report->results[index];
    return UMI_STATUS_OK;
}

/* Runner completion and application acceptance are deliberately separate. */
UmiStatus UmiUiAutomationReportRequireSuccess(
    const UmiUiAutomationReport *report, size_t expectedStepCount)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (expectedStepCount == 0U || report->planned != expectedStepCount ||
        report->state != UMI_UI_AUTOMATION_SESSION_COMPLETED ||
        report->total != expectedStepCount ||
        report->passed != expectedStepCount || report->failed != 0U)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
