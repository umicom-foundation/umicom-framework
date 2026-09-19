/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/gui_recording/check_sessions.c
 *
 * PURPOSE:
 *   Teach repeatable acceptance runs using a Notes save request followed by
 *   independent file and draft checks. Export reports without hiding a stop.
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
#include <string.h>

static const char SAVED_NOTE[] = "Umicom Notes: release checklist\nSaved: yes\n";
static const char OTHER_DRAFT[] = "Keep this second note unsaved.";

typedef struct NotesFixture {
    FILE *savedFile;
    int failSave;
    char otherDraft[UMI_UI_DESCRIPTION_CAPACITY];
} NotesFixture;

/* This fixture is deliberately independent of GTK and Studio. A graphical
 * host can pass its existing UmiUiAutomationDriver to the same session API. */
static UmiStatus PerformNotesStep(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out, char *message, size_t messageCapacity)
{
    NotesFixture *notes = context;
    (void)snprintf(out->target_id, sizeof(out->target_id), "%s", step->target_id);
    (void)snprintf(out->role_name, sizeof(out->role_name), "notes-file-fixture");
    if (strcmp(step->target_id, "notes.save") == 0) {
        /* --fail-save models a handler that says OK but does no useful work. */
        if (!notes->failSave) {
            if (fwrite(SAVED_NOTE, 1U, strlen(SAVED_NOTE), notes->savedFile) != strlen(SAVED_NOTE) ||
                fflush(notes->savedFile) != 0) return UMI_STATUS_IO_ERROR;
        }
        (void)snprintf(message, messageCapacity, "Save handler returned.");
    } else if (strcmp(step->target_id, "notes.saved-file") == 0) {
        size_t bytes;
        if (fseek(notes->savedFile, 0L, SEEK_SET) != 0) return UMI_STATUS_IO_ERROR;
        bytes = fread(out->text, 1U, sizeof(out->text) - 1U, notes->savedFile);
        if (ferror(notes->savedFile)) return UMI_STATUS_IO_ERROR;
        out->text[bytes] = '\0';
        (void)snprintf(message, messageCapacity, "Read the actual saved bytes.");
    } else if (strcmp(step->target_id, "notes.other-draft") == 0) {
        (void)snprintf(out->text, sizeof(out->text), "%s", notes->otherDraft);
        (void)snprintf(message, messageCapacity, "Read the separate draft.");
    } else return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

/* Adapt a caller-owned FILE to the Framework output callback. */
static UmiStatus WriteBytes(void *context, const char *bytes, size_t length)
{
    return fwrite(bytes, 1U, length, context) == length ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
/* Keep each scenario step small and explicitly identify its intended target. */
static UmiStatus Add(UmiUiAutomationScenario *scenario, const char *id,
    const char *target, UmiUiAutomationOperation operation, const char *value)
{
    UmiUiAutomationStep step = {0};
    /* These are fixed teaching strings within the public field capacities. */
    (void)snprintf(step.step_id, sizeof(step.step_id), "%s", id);
    (void)snprintf(step.target_id, sizeof(step.target_id), "%s", target);
    (void)snprintf(step.value, sizeof(step.value), "%s", value);
    step.operation = operation;
    return umi_ui_automation_scenario_add(scenario, &step);
}

typedef UmiStatus (*ExportFn)(const UmiUiAutomationReport *, int, UmiUiRecordingWriteFn, void *);
/* Export to a new file beside the other reports, preserving earlier runs. */
static UmiStatus SaveReport(const UmiUiAutomationReport *report,
    const char *directory, const char *filename, ExportFn exportReport)
{
    char path[2048];
    FILE *file;
    UmiStatus status;
    int length = snprintf(path, sizeof(path), "%s/%s", directory, filename);
    if (length < 0 || (size_t)length >= sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Never overwrite earlier evidence. Choose an empty output directory. */
    file = fopen(path, "wx");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    status = exportReport(report, 0, WriteBytes, file);
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    return status;
}

/* Exit 0: accepted. Exit 1: failed or cancelled checks. Exit 2: setup/export
 * failure. Keeping these outcomes separate lets automated tests detect errors
 * in the test setup rather than treating every nonzero exit as useful evidence. */
int main(int argc, char **argv)
{
    NotesFixture notes = {0};
    UmiUiAutomationDriver driver = {0};
    UmiUiAutomationScenario *scenario = NULL;
    UmiUiAutomationSession *session = NULL;
    UmiUiAutomationSessionSnapshot info;
    const UmiUiAutomationReport *report;
    const char *directory = NULL;
    int cancel = 0, exitCode = 2, i;
    UmiStatus status;
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--fail-save") == 0) notes.failSave = 1;
        else if (strcmp(argv[i], "--cancel") == 0) cancel = 1;
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) directory = argv[++i];
        else { fprintf(stderr, "Usage: %s [--fail-save | --cancel] [--output EMPTY_DIRECTORY]\n", argv[0]); return 2; }
    }
    if (directory != NULL && directory[0] == '\0') { fprintf(stderr, "Choose an existing empty output directory.\n"); return 2; }
    if (notes.failSave && cancel) { fprintf(stderr, "Choose one failure exercise at a time.\n"); return 2; }
    notes.savedFile = tmpfile();
    if (notes.savedFile == NULL) { fprintf(stderr, "Cannot create the temporary Notes file.\n"); return 2; }
    (void)snprintf(notes.otherDraft, sizeof(notes.otherDraft), "%s", OTHER_DRAFT);
    driver.structure_size = (uint32_t)sizeof(driver);
    driver.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)snprintf(driver.driver_id, sizeof(driver.driver_id), "notes.file-fixture");
    driver.context = &notes;
    driver.perform = PerformNotesStep;
    status = umi_ui_automation_scenario_create("notes.save-checks", "Save a Notes document", &scenario);
    if (status == UMI_STATUS_OK) status = Add(scenario, "request-save", "notes.save", UMI_UI_AUTOMATION_INVOKE_COMMAND, "");
    if (status == UMI_STATUS_OK) status = Add(scenario, "verify-file", "notes.saved-file", UMI_UI_AUTOMATION_ASSERT_TEXT, SAVED_NOTE);
    if (status == UMI_STATUS_OK) status = Add(scenario, "verify-other-draft", "notes.other-draft", UMI_UI_AUTOMATION_ASSERT_TEXT, OTHER_DRAFT);
    if (status == UMI_STATUS_OK) status = UmiUiAutomationSessionCreate(&driver, scenario,
        cancel ? "notes-cancelled" : (notes.failSave ? "notes-failed" : "notes-passed"), &session);
    /* The session owns its plan now; it does not borrow scenario storage. */
    umi_ui_automation_scenario_destroy(scenario);
    if (status != UMI_STATUS_OK) goto cleanup;
    report = UmiUiAutomationSessionReport(session);
    while (1) {
        status = UmiUiAutomationReportSnapshot(report, &info);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (info.state != UMI_UI_AUTOMATION_SESSION_READY && info.state != UMI_UI_AUTOMATION_SESSION_RUNNING) break;
        /* A graphical host would return to its event loop between these calls.
         * Cancellation is cooperative; it cannot interrupt an active driver. */
        status = UmiUiAutomationSessionAdvance(session);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (cancel) {
            status = UmiUiAutomationSessionCancel(session);
            if (status != UMI_STATUS_OK) goto cleanup;
        }
    }
    if (directory != NULL) {
        status = SaveReport(report, directory, "checks.json", UmiUiAutomationReportWriteJson);
        if (status == UMI_STATUS_OK) status = SaveReport(report, directory, "checks.html", UmiUiAutomationReportWriteHtml);
        if (status == UMI_STATUS_OK) status = SaveReport(report, directory, "junit.xml", UmiUiAutomationReportWriteJUnit);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    status = UmiUiAutomationReportWriteJson(report, 0, WriteBytes, stdout);
    if (fflush(stdout) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) goto cleanup;
    exitCode = UmiUiAutomationReportRequireSuccess(report, 3U) == UMI_STATUS_OK ? 0 : 1;
cleanup:
    if (status != UMI_STATUS_OK) fprintf(stderr, "Notes check setup/export failed: %s\n", umi_status_text(status));
    (void)UmiUiAutomationSessionDestroy(session);
    if (fclose(notes.savedFile) != 0) exitCode = 2;
    return exitCode;
}
