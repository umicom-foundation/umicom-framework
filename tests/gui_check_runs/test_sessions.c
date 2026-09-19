/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_check_runs/test_sessions.c
 *
 * PURPOSE:
 *   Verify paced GUI check sessions, cancellation, legacy compatibility and
 *   truthful exports using controlled driver responses and output failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/automation_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

typedef struct TestDriver {
    size_t calls;
    size_t failAt;
    size_t cancelAt;
    int wrongText;
    int malformed;
    int reenter;
    UmiStatus nestedAdvance;
    UmiStatus nestedDestroy;
    UmiUiAutomationSession *session;
} TestDriver;

typedef struct Sink {
    char *text;
    size_t length;
    size_t calls;
    size_t failAt;
    UmiUiAutomationSession **destroySession;
} Sink;

/* Capture bytes and optionally fail or destroy the live session mid-export. */
static UmiStatus Write(void *context, const char *bytes, size_t length)
{
    Sink *sink = context;
    char *next;
    ++sink->calls;
    if (sink->destroySession != NULL && *sink->destroySession != NULL) {
        UmiStatus status = UmiUiAutomationSessionDestroy(*sink->destroySession);
        if (status != UMI_STATUS_OK) return status;
        *sink->destroySession = NULL;
    }
    if (sink->failAt == sink->calls) return UMI_STATUS_IO_ERROR;
    if (length > 1024U * 1024U - sink->length) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = realloc(sink->text, sink->length + length + 1U);
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    sink->text = next;
    memcpy(sink->text + sink->length, bytes, length);
    sink->length += length;
    sink->text[sink->length] = '\0';
    return UMI_STATUS_OK;
}

/* Deliberately vary observations and callback ordering to test the runner. */
static UmiStatus Perform(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out, char *message, size_t capacity)
{
    TestDriver *driver = context;
    ++driver->calls;
    (void)snprintf(out->target_id, sizeof(out->target_id), "%s", step->target_id);
    (void)snprintf(out->role_name, sizeof(out->role_name), "test-control");
    (void)snprintf(out->text, sizeof(out->text), "%s", driver->wrongText ? "old text" : step->value);
    (void)snprintf(message, capacity, "Driver detail: <private>&\"'\n");
    out->visible = out->enabled = out->focused = out->selected = 1;
    if (driver->malformed) { out->text[0] = (char)0xff; out->text[1] = '\0'; message[0] = (char)0xff; message[1] = '\0'; }
    if (driver->reenter) {
        driver->nestedAdvance = UmiUiAutomationSessionAdvance(driver->session);
        driver->nestedDestroy = UmiUiAutomationSessionDestroy(driver->session);
    }
    if (driver->calls == driver->cancelAt) (void)UmiUiAutomationSessionCancel(driver->session);
    return driver->calls == driver->failAt ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

/* Construct a valid existing driver contract around a controlled fixture. */
static UmiUiAutomationDriver Driver(TestDriver *context)
{
    UmiUiAutomationDriver result = {0};
    result.structure_size = (uint32_t)sizeof(result);
    result.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)snprintf(result.driver_id, sizeof(result.driver_id), "notes.fixture");
    result.context = context;
    result.perform = Perform;
    return result;
}
/* Build a copied plan whose assertions have deterministic expected text. */
static UmiUiAutomationScenario *Scenario(size_t count, int keepGoing)
{
    UmiUiAutomationScenario *result = NULL;
    size_t i;
    if (umi_ui_automation_scenario_create("notes.check", "Private title", &result) != UMI_STATUS_OK) return NULL;
    if (umi_ui_automation_scenario_set_continue_on_failure(result, keepGoing) != UMI_STATUS_OK) return NULL;
    for (i = 0U; i < count; ++i) {
        UmiUiAutomationStep step = {0};
        (void)snprintf(step.step_id, sizeof(step.step_id), "step-%zu", i);
        (void)snprintf(step.target_id, sizeof(step.target_id), "notes.saved-file");
        (void)snprintf(step.value, sizeof(step.value), "Expected <notes>&\"'\n");
        step.operation = UMI_UI_AUTOMATION_ASSERT_TEXT;
        if (umi_ui_automation_scenario_add(result, &step) != UMI_STATUS_OK) { umi_ui_automation_scenario_destroy(result); return NULL; }
    }
    return result;
}
/* Drive the session without letting an erroneous loop run indefinitely. */
static int Finish(UmiUiAutomationSession *session)
{
    UmiUiAutomationSessionSnapshot info;
    size_t limit = 0U;
    while (1) {
        REQUIRE(UmiUiAutomationReportSnapshot(UmiUiAutomationSessionReport(session), &info) == UMI_STATUS_OK);
        if (info.state != UMI_UI_AUTOMATION_SESSION_READY && info.state != UMI_UI_AUTOMATION_SESSION_RUNNING) break;
        REQUIRE(++limit <= UMI_UI_AUTOMATION_STEP_MAX);
        REQUIRE(UmiUiAutomationSessionAdvance(session) == UMI_STATUS_OK);
    }
    return 0;
}

/* Enumerate cancellation/failure orderings, not just the normal path. */
static int StateMatrix(void)
{
    size_t count, fail, cancel;
    int keep;
    for (count = 1U; count <= 5U; ++count) for (fail = 0U; fail <= count; ++fail)
    for (cancel = 0U; cancel <= count; ++cancel) for (keep = 0; keep <= 1; ++keep) {
        TestDriver context = {0};
        UmiUiAutomationDriver driver = Driver(&context);
        UmiUiAutomationScenario *scenario = Scenario(count, keep);
        UmiUiAutomationSession *session = NULL;
        UmiUiAutomationSessionSnapshot info;
        size_t attempted = count;
        size_t failures;
        UmiUiAutomationSessionState state = UMI_UI_AUTOMATION_SESSION_COMPLETED;
        REQUIRE(scenario != NULL);
        context.failAt = fail; context.cancelAt = cancel;
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "matrix", &session) == UMI_STATUS_OK);
        context.session = session;
        REQUIRE(Finish(session) == 0);
        if (!keep && fail != 0U && fail < attempted) attempted = fail;
        if (cancel != 0U && cancel <= attempted) { attempted = cancel; state = UMI_UI_AUTOMATION_SESSION_CANCELLED; }
        else if (!keep && fail != 0U) state = UMI_UI_AUTOMATION_SESSION_STOPPED;
        failures = fail != 0U && fail <= attempted ? 1U : 0U;
        REQUIRE(UmiUiAutomationReportSnapshot(UmiUiAutomationSessionReport(session), &info) == UMI_STATUS_OK);
        REQUIRE(info.state == state && info.attempted == attempted && info.failed == failures);
        REQUIRE(info.notRun == count - attempted && info.passed == attempted - failures);
        REQUIRE((UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_OK) ==
            (state == UMI_UI_AUTOMATION_SESSION_COMPLETED && failures == 0U));
        REQUIRE(UmiUiAutomationSessionDestroy(session) == UMI_STATUS_OK);
        umi_ui_automation_scenario_destroy(scenario);
    }
    return 0;
}

int main(int argc, char **argv)
{
    TestDriver context = {0};
    UmiUiAutomationDriver driver = Driver(&context);
    UmiUiAutomationScenario *scenario = NULL;
    UmiUiAutomationSession *session = NULL;
    UmiUiAutomationReport *legacy = NULL;
    UmiUiAutomationSessionSnapshot info;
    UmiUiAutomationStepResult row;
    UmiUiAutomationStepState rowState;
    Sink sink = {0};
    const char *name;
    size_t count = 3U;
    int keepGoing = 0;
    REQUIRE(argc == 2);
    name = argv[1];
    {
        const char *cases[] = {"ready", "invalid", "empty", "paced", "complete", "copied-plan", "captured-policy", "capacity",
            "stop", "continue", "contradiction", "cancel-before", "cancel-between", "cancel-callback", "cancel-last",
            "reentry", "terminal", "partial-count", "row-bounds", "legacy", "legacy-stop", "legacy-empty",
            "export-json", "export-html", "export-junit", "junit-cancel", "redaction", "escaping", "export-destroy",
            "export-failure", "export-invalid", "malformed-export", "xml-characters", "state-matrix"};
        size_t i; int found = 0;
        for (i = 0U; i < sizeof(cases)/sizeof(cases[0]); ++i) if (strcmp(name, cases[i]) == 0) found = 1;
        REQUIRE(found);
    }
    if (strcmp(name, "state-matrix") == 0) return StateMatrix();
    if (strcmp(name, "capacity") == 0) count = UMI_UI_AUTOMATION_STEP_MAX;
    if (strcmp(name, "empty") == 0 || strcmp(name, "legacy-empty") == 0) count = 0U;
    if (strcmp(name, "continue") == 0 || strcmp(name, "captured-policy") == 0) keepGoing = 1;
    scenario = Scenario(count, keepGoing);
    REQUIRE(scenario != NULL);
    if (strcmp(name, "xml-characters") == 0) {
        UmiUiAutomationStep step = {0};
        umi_ui_automation_scenario_destroy(scenario); scenario = NULL; count = 1U;
        REQUIRE(umi_ui_automation_scenario_create("notes.xml", "Encoding checks", &scenario) == UMI_STATUS_OK);
        strcpy(step.step_id, "characters"); strcpy(step.target_id, "notes.saved-file");
        strcpy(step.value, "control:\001 noncharacter:\xef\xbf\xbe &<\"\t");
        step.operation = UMI_UI_AUTOMATION_ASSERT_TEXT;
        REQUIRE(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);
    }

    if (strcmp(name, "empty") == 0) {
        session = (UmiUiAutomationSession *)scenario;
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "run", &session) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(session == NULL);
    } else if (strcmp(name, "invalid") == 0) {
        REQUIRE(UmiUiAutomationSessionCreate(NULL, scenario, "run", &session) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionCreate(&driver, NULL, "run", &session) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "bad id", &session) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, NULL, &session) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "run", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionAdvance(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiUiAutomationSessionDestroy(NULL) == UMI_STATUS_OK);
        REQUIRE(UmiUiAutomationSessionReport(NULL) == NULL);
        memset(&info, 0xff, sizeof(info));
        REQUIRE(UmiUiAutomationReportSnapshot(NULL, &info) == UMI_STATUS_INVALID_ARGUMENT && info.planned == 0U);
    } else if (strncmp(name, "legacy", 6U) == 0) {
        if (strcmp(name, "legacy-stop") == 0) context.failAt = 1U;
        REQUIRE(umi_ui_automation_run(&driver, scenario, &legacy) == UMI_STATUS_OK);
        REQUIRE(UmiUiAutomationReportSnapshot(legacy, &info) == UMI_STATUS_OK);
        REQUIRE(info.planned == count);
        if (strcmp(name, "legacy-stop") == 0) {
            REQUIRE(info.attempted == 1U && info.notRun == 2U && info.failed == 1U);
            REQUIRE(UmiUiAutomationReportStep(legacy, 2U, &rowState, &row) == UMI_STATUS_OK);
            REQUIRE(rowState == UMI_UI_AUTOMATION_STEP_NOT_RUN);
            REQUIRE(umi_ui_automation_report_at(legacy, 2U, &row) == UMI_STATUS_NOT_FOUND);
        } else if (count == 0U) {
            REQUIRE(UmiUiAutomationReportRequireSuccess(legacy, 0U) == UMI_STATUS_INVALID_STATE);
            REQUIRE(UmiUiAutomationReportWriteJUnit(legacy, 0, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "IncompleteRun") != NULL);
        } else REQUIRE(UmiUiAutomationReportRequireSuccess(legacy, count) == UMI_STATUS_OK);
    } else {
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "notes-run", &session) == UMI_STATUS_OK);
        context.session = session;
        if (strcmp(name, "copied-plan") == 0) { umi_ui_automation_scenario_destroy(scenario); scenario = NULL; }
        if (strcmp(name, "captured-policy") == 0) REQUIRE(umi_ui_automation_scenario_set_continue_on_failure(scenario, 0) == UMI_STATUS_OK);
        if (strcmp(name, "stop") == 0 || strcmp(name, "continue") == 0 || strcmp(name, "captured-policy") == 0) context.failAt = 1U;
        if (strcmp(name, "contradiction") == 0) context.wrongText = 1;
        if (strcmp(name, "malformed-export") == 0) context.malformed = 1;
        if (strcmp(name, "reentry") == 0) context.reenter = 1;
        if (strcmp(name, "cancel-callback") == 0) context.cancelAt = 1U;
        if (strcmp(name, "cancel-last") == 0) context.cancelAt = count;
        if (strcmp(name, "ready") == 0 || strcmp(name, "cancel-before") == 0 || strcmp(name, "junit-cancel") == 0) {
            REQUIRE(context.calls == 0U);
            if (strcmp(name, "ready") != 0) REQUIRE(UmiUiAutomationSessionCancel(session) == UMI_STATUS_OK);
        } else if (strcmp(name, "paced") == 0 || strcmp(name, "cancel-between") == 0 || strcmp(name, "partial-count") == 0) {
            REQUIRE(UmiUiAutomationSessionAdvance(session) == UMI_STATUS_OK && context.calls == 1U);
            if (strcmp(name, "cancel-between") == 0) REQUIRE(UmiUiAutomationSessionCancel(session) == UMI_STATUS_OK);
        } else REQUIRE(Finish(session) == 0);
        REQUIRE(UmiUiAutomationReportSnapshot(UmiUiAutomationSessionReport(session), &info) == UMI_STATUS_OK);
        REQUIRE(info.planned == count && info.attempted + info.notRun == count && info.passed + info.failed == info.attempted);
        REQUIRE(strcmp(info.runId, "notes-run") == 0 && strcmp(info.driverId, "notes.fixture") == 0);

        if (strcmp(name, "ready") == 0) {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_READY && info.notRun == count);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_INVALID_STATE);
        } else if (strcmp(name, "paced") == 0 || strcmp(name, "partial-count") == 0) {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_RUNNING && info.attempted == 1U);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), 1U) == UMI_STATUS_INVALID_STATE);
        } else if (strncmp(name, "cancel-", 7U) == 0 || strcmp(name, "junit-cancel") == 0) {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_CANCELLED);
            REQUIRE(UmiUiAutomationSessionAdvance(session) == UMI_STATUS_INVALID_STATE);
            REQUIRE(UmiUiAutomationSessionCancel(session) == UMI_STATUS_OK);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_INVALID_STATE);
            if (strcmp(name, "cancel-last") == 0) REQUIRE(info.attempted == count && info.failed == 0U);
            if (strcmp(name, "cancel-callback") == 0 || strcmp(name, "cancel-between") == 0) REQUIRE(info.attempted == 1U);
        } else if (strcmp(name, "stop") == 0 || strcmp(name, "contradiction") == 0 || strcmp(name, "malformed-export") == 0) {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_STOPPED && info.attempted == 1U && info.notRun == 2U);
            REQUIRE(UmiUiAutomationReportStep(UmiUiAutomationSessionReport(session), 2U, &rowState, &row) == UMI_STATUS_OK);
            REQUIRE(rowState == UMI_UI_AUTOMATION_STEP_NOT_RUN && strcmp(row.step.step_id, "step-2") == 0);
        } else if (strcmp(name, "continue") == 0 || strcmp(name, "captured-policy") == 0) {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_COMPLETED && info.failed == 1U && info.attempted == count);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_INVALID_STATE);
        } else {
            REQUIRE(info.state == UMI_UI_AUTOMATION_SESSION_COMPLETED && info.passed == count);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_OK);
        }
        if (strcmp(name, "reentry") == 0) REQUIRE(context.nestedAdvance == UMI_STATUS_BUSY && context.nestedDestroy == UMI_STATUS_BUSY);
        if (strcmp(name, "terminal") == 0) {
            REQUIRE(UmiUiAutomationSessionCancel(session) == UMI_STATUS_OK);
            REQUIRE(UmiUiAutomationSessionAdvance(session) == UMI_STATUS_INVALID_STATE);
            REQUIRE(UmiUiAutomationReportRequireSuccess(UmiUiAutomationSessionReport(session), count) == UMI_STATUS_OK);
        }
        if (strcmp(name, "row-bounds") == 0) {
            REQUIRE(UmiUiAutomationReportStep(UmiUiAutomationSessionReport(session), count, &rowState, &row) == UMI_STATUS_NOT_FOUND);
            REQUIRE(rowState == UMI_UI_AUTOMATION_STEP_NOT_RUN && row.step.step_id[0] == '\0');
            REQUIRE(UmiUiAutomationReportStep(NULL, 0U, &rowState, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(UmiUiAutomationReportStep(UmiUiAutomationSessionReport(session), 0U, NULL, &row) == UMI_STATUS_INVALID_ARGUMENT);
        }
        if (strcmp(name, "malformed-export") == 0) {
            REQUIRE(UmiUiAutomationReportWriteJson(UmiUiAutomationSessionReport(session), 1, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "[invalid UTF-8]") != NULL);
        }
        if (strcmp(name, "xml-characters") == 0) {
            REQUIRE(UmiUiAutomationReportWriteJUnit(UmiUiAutomationSessionReport(session), 1, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "&#xfffd;") != NULL && strchr(sink.text, 1) == NULL);
        }
        if (strcmp(name, "export-json") == 0 || strcmp(name, "redaction") == 0 || strcmp(name, "export-destroy") == 0) {
            if (strcmp(name, "export-destroy") == 0) sink.destroySession = &session;
            REQUIRE(UmiUiAutomationReportWriteJson(UmiUiAutomationSessionReport(session), 0, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "\"accepted\":true") != NULL && strstr(sink.text, "not_run") != NULL);
            REQUIRE(strstr(sink.text, "private") == NULL && strstr(sink.text, "Expected") == NULL && strstr(sink.text, "Private title") == NULL);
        }
        if (strcmp(name, "export-html") == 0 || strcmp(name, "escaping") == 0) {
            REQUIRE(UmiUiAutomationReportWriteHtml(UmiUiAutomationSessionReport(session), 1, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "&lt;notes&gt;&amp;&quot;&#39;") != NULL);
            REQUIRE(strstr(sink.text, "<private>") == NULL);
        }
        if (strcmp(name, "export-junit") == 0 || strcmp(name, "junit-cancel") == 0) {
            REQUIRE(UmiUiAutomationReportWriteJUnit(UmiUiAutomationSessionReport(session), 0, Write, &sink) == UMI_STATUS_OK);
            REQUIRE(strstr(sink.text, "tests=\"4\"") != NULL && strstr(sink.text, "run-completion") != NULL);
            if (strcmp(name, "junit-cancel") == 0) REQUIRE(strstr(sink.text, "failures=\"1\"") != NULL && strstr(sink.text, "skipped=\"3\"") != NULL);
            else REQUIRE(strstr(sink.text, "failures=\"0\"") != NULL);
        }
        if (strcmp(name, "export-failure") == 0) {
            sink.failAt = 3U;
            REQUIRE(UmiUiAutomationReportWriteJson(UmiUiAutomationSessionReport(session), 0, Write, &sink) == UMI_STATUS_IO_ERROR);
            REQUIRE(sink.calls == 3U);
        }
        if (strcmp(name, "export-invalid") == 0) {
            REQUIRE(UmiUiAutomationReportWriteJson(NULL, 0, Write, &sink) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(UmiUiAutomationReportWriteHtml(UmiUiAutomationSessionReport(session), 2, Write, &sink) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(UmiUiAutomationReportWriteJUnit(UmiUiAutomationSessionReport(session), 0, NULL, &sink) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(sink.calls == 0U);
        }
    }
    free(sink.text);
    umi_ui_automation_report_destroy(legacy);
    REQUIRE(UmiUiAutomationSessionDestroy(session) == UMI_STATUS_OK);
    umi_ui_automation_scenario_destroy(scenario);
    printf("PASS %s\n", name);
    return 0;
}
