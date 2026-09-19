/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/gui_recording/main.c
 *
 * PURPOSE:
 *   Explain outcome checks and report export with a temporary Notes file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/interaction_recording.h"
#include <stdio.h>
#include <string.h>

/* This example driver is a controlled file fixture, not a desktop driver.
 * A native test replaces it with the real widget adapter and a file assertion.
 * The FILE belongs to main and stays open until both steps have finished. */
typedef struct NotesFixture { FILE *file; int refuseSave; } NotesFixture;
static const char NOTES_TEXT[] = "Umicom Notes: planning the next application.\n";

static UmiStatus Perform(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *observation, char *message, size_t capacity)
{
    NotesFixture *fixture = context;
    (void)observation;
    if (step->operation == UMI_UI_AUTOMATION_CLICK) {
        /* A no-op click deliberately returns OK to demonstrate why a separate
         * postcondition is essential. It is not a production Save handler. */
        if (!fixture->refuseSave &&
            (fputs(NOTES_TEXT, fixture->file) == EOF || fflush(fixture->file) != 0))
            return UMI_STATUS_IO_ERROR;
        (void)snprintf(message, capacity, "Save request returned.");
        return UMI_STATUS_OK;
    }
    if (step->operation == UMI_UI_AUTOMATION_ASSERT_TEXT) {
        char saved[128] = {0};
        if (fseek(fixture->file, 0L, SEEK_SET) != 0) return UMI_STATUS_IO_ERROR;
        size_t length = fread(saved, 1U, sizeof(saved) - 1U, fixture->file);
        if (ferror(fixture->file)) return UMI_STATUS_IO_ERROR;
        (void)snprintf(message, capacity, "The saved file must contain the expected Notes text.");
        return length == strlen(NOTES_TEXT) && strcmp(saved, NOTES_TEXT) == 0
            ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_NOT_IMPLEMENTED;
}

/* The report writer borrows stdout. main checks a final flush as well as
 * individual writes, since buffered output can fail after the last callback. */
static UmiStatus Write(void *context, const char *bytes, size_t length)
{
    return fwrite(bytes, 1U, length, context) == length ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
int main(int argc, char **argv)
{
    NotesFixture fixture = {NULL, 0};
    UmiUiAutomationScenario *scenario = NULL;
    UmiUiAutomationReport *report = NULL;
    UmiUiRecording *recording = NULL;
    UmiUiAutomationDriver driver = {0};
    UmiUiAutomationStep step = {0};
    UmiStatus status, acceptance = UMI_STATUS_INVALID_STATE;
    int result = 2, html = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--fail-save") == 0) fixture.refuseSave = 1;
        else if (strcmp(argv[i], "--html") == 0) html = 1;
        else { fputs("Usage: umicom-recording-example [--fail-save] [--html]\n", stderr); return 2; }
    }
    fixture.file = tmpfile();
    if (fixture.file == NULL) { perror("temporary Notes file"); return 2; }
    driver.structure_size = (uint32_t)sizeof(driver);
    driver.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)snprintf(driver.driver_id, sizeof(driver.driver_id), "notes.file-fixture");
    driver.context = &fixture; driver.perform = Perform;
    status = umi_ui_automation_scenario_create("notes.save", "Check a Notes save", &scenario);
    if (status != UMI_STATUS_OK) goto cleanup;
    (void)snprintf(step.step_id, sizeof(step.step_id), "save-request");
    (void)snprintf(step.target_id, sizeof(step.target_id), "notes.save");
    step.operation = UMI_UI_AUTOMATION_CLICK;
    status = umi_ui_automation_scenario_add(scenario, &step);
    if (status != UMI_STATUS_OK) goto cleanup;
    (void)snprintf(step.step_id, sizeof(step.step_id), "saved-file-check");
    (void)snprintf(step.target_id, sizeof(step.target_id), "notes.saved-file");
    step.operation = UMI_UI_AUTOMATION_ASSERT_TEXT;
    status = umi_ui_automation_scenario_add(scenario, &step);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = umi_ui_automation_run(&driver, scenario, &report);
    if (status != UMI_STATUS_OK) goto cleanup;
    acceptance = UmiUiAutomationReportRequireSuccess(report, 2U);
    status = UmiUiRecordingCreate("umicom-notes", "source example; file fixture", 4U, &recording);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiUiRecordingSetActive(recording, 1);
    if (status != UMI_STATUS_OK) goto cleanup;
    for (size_t i = 0U; i < umi_ui_automation_report_total(report); ++i) {
        UmiUiAutomationStepResult saved;
        status = umi_ui_automation_report_at(report, i, &saved);
        if (status == UMI_STATUS_OK) status = UmiUiRecordingAppendAutomation(recording, (uint64_t)i, &saved);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    (void)UmiUiRecordingSetActive(recording, 0);
    status = html ? UmiUiRecordingWriteHtml(recording, Write, stdout)
                  : UmiUiRecordingWriteJson(recording, Write, stdout);
    if (status != UMI_STATUS_OK || fflush(stdout) != 0) goto cleanup;
    result = acceptance == UMI_STATUS_OK ? 0 : 1;
cleanup:
    if (result == 2) fputs("The example could not complete its run or export.\n", stderr);
    UmiUiRecordingDestroy(recording);
    umi_ui_automation_report_destroy(report);
    umi_ui_automation_scenario_destroy(scenario);
    if (fclose(fixture.file) != 0) result = 2;
    return result;
}
