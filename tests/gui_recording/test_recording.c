/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_recording/test_recording.c
 * PURPOSE: Exercise recording limits, privacy, export and acceptance outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/interaction_recording.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Buffer { char text[65536]; size_t used, calls, failAt; } Buffer;
static UmiStatus Write(void *ctx, const char *bytes, size_t length)
{
    Buffer *b = ctx;
    ++b->calls;
    if (b->failAt != 0U && b->calls >= b->failAt) return UMI_STATUS_IO_ERROR;
    if (length >= sizeof(b->text) - b->used) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(b->text + b->used, bytes, length); b->used += length; b->text[b->used] = '\0';
    return UMI_STATUS_OK;
}
static UmiUiRecordingEntry Entry(void)
{
    UmiUiRecordingEntry e = {0};
    e.kind = UMI_UI_RECORDING_INPUT; e.elapsedMs = 10U;
    strcpy(e.targetId, "notes.save"); strcpy(e.action, "pointer-release");
    return e;
}
static UmiStatus Drive(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out, char *message, size_t capacity)
{
    int *saved = context;
    (void)snprintf(out->target_id, sizeof(out->target_id), "%s", step->target_id);
    (void)snprintf(out->role_name, sizeof(out->role_name), "notes-file");
    (void)snprintf(message, capacity, "Fixture operation");
    if (step->operation == UMI_UI_AUTOMATION_CLICK && strcmp(step->value, "no-op") != 0) *saved = 1;
    (void)snprintf(out->text, sizeof(out->text), "%s", *saved ? "saved" : "unsaved");
    if (step->operation == UMI_UI_AUTOMATION_ASSERT_TEXT && *saved != 1) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
static int Acceptance(const char *mode)
{
    UmiUiAutomationScenario *s = NULL; UmiUiAutomationReport *r = NULL;
    UmiUiAutomationDriver d = {0}; UmiUiAutomationStep step = {0}; int saved = 0;
    d.structure_size = (uint32_t)sizeof(d); d.api_version = UMI_UI_AUTOMATION_API_VERSION;
    strcpy(d.driver_id, "recording-fixture"); d.context = &saved; d.perform = Drive;
    CHECK(umi_ui_automation_scenario_create("notes.save", "Save a practice note", &s) == UMI_STATUS_OK);
    if (strcmp(mode, "empty") != 0) {
        strcpy(step.step_id, "save"); strcpy(step.target_id, "notes.save"); step.operation = UMI_UI_AUTOMATION_CLICK;
        if (strcmp(mode, "no-op") == 0) strcpy(step.value, "no-op");
        CHECK(umi_ui_automation_scenario_add(s, &step) == UMI_STATUS_OK);
        strcpy(step.step_id, "check"); step.operation = UMI_UI_AUTOMATION_ASSERT_TEXT;
        strcpy(step.value, "saved");
        CHECK(umi_ui_automation_scenario_add(s, &step) == UMI_STATUS_OK);
    }
    CHECK(umi_ui_automation_run(&d, s, &r) == UMI_STATUS_OK);
    if (strcmp(mode, "pass") == 0) CHECK(UmiUiAutomationReportRequireSuccess(r, 2U) == UMI_STATUS_OK);
    else CHECK(UmiUiAutomationReportRequireSuccess(r, umi_ui_automation_scenario_count(s)) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiUiAutomationReportRequireSuccess(r, 99U) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiUiAutomationReportRequireSuccess(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    umi_ui_automation_report_destroy(r); umi_ui_automation_scenario_destroy(s); return 0;
}
int main(int argc, char **argv)
{
    const char *test = argc > 1 ? argv[1] : "lifecycle";
    UmiUiRecording *r = NULL; UmiUiRecordingEntry e = Entry(), got;
    Buffer *b = calloc(1U, sizeof(*b));
    CHECK(b != NULL);
    if (strncmp(test, "acceptance-", 11U) == 0) { free(b); return Acceptance(test + 11U); }
    if (strcmp(test, "invalid-create") == 0) {
        CHECK(UmiUiRecordingCreate("notes", "", 0U, &r) == UMI_STATUS_INVALID_ARGUMENT && r == NULL);
        CHECK(UmiUiRecordingCreate("notes", "", UMI_UI_RECORDING_MAX_ENTRIES + 1U, &r) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiRecordingCreate(NULL, "", 1U, &r) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiRecordingCreate("", "", 1U, &r) == UMI_STATUS_INVALID_ARGUMENT);
        free(b); return 0;
    }
    CHECK(UmiUiRecordingCreate("umicom.notes", "fixture-build", 4U, &r) == UMI_STATUS_OK);
    CHECK(!UmiUiRecordingIsActive(r));
    CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiUiRecordingSetActive(r, 1) == UMI_STATUS_OK);
    if (strcmp(test, "lifecycle") == 0) {
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
        CHECK(UmiUiRecordingSetActive(r, 0) == UMI_STATUS_OK);
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiUiRecordingCount(r) == 1U);
        CHECK(UmiUiRecordingSetActive(r, 1) == UMI_STATUS_OK);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < 4; ++i) CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
        CHECK(!UmiUiRecordingIsActive(r) && UmiUiRecordingLimitReached(r));
        CHECK(UmiUiRecordingSetActive(r, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiUiRecordingCount(r) == 4U);
    } else if (strcmp(test, "copy") == 0) {
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK); strcpy(e.targetId, "changed");
        CHECK(UmiUiRecordingAt(r, 0U, &got) == UMI_STATUS_OK && strcmp(got.targetId, "notes.save") == 0);
        CHECK(UmiUiRecordingAt(r, 1U, &got) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(test, "timestamp") == 0) {
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK); e.elapsedMs = 9U;
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT && UmiUiRecordingCount(r) == 1U);
        e.elapsedMs = UINT64_MAX; CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
    } else if (strcmp(test, "unterminated") == 0) {
        memset(e.detail, 'a', sizeof(e.detail));
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT && UmiUiRecordingCount(r) == 0U);
    } else if (strcmp(test, "utf8") == 0) {
        strcpy(e.detail, "\xc0\xaf"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(e.detail, "\xed\xa0\x80"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(e.detail, "\xf4\x90\x80\x80"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(e.detail, "\xe2"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(e.detail, "Notes: caf\xc3\xa9 \xf0\x9f\x93\x9d"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
    } else if (strcmp(test, "enums") == 0) {
        e.kind = (UmiUiRecordingKind)99; CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        e = Entry(); e.status = (UmiStatus)99; CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        e = Entry(); e.outcomeKnown = 2; CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "attachments") == 0) {
        const char *names[] = {"../secret.png", "https://x.png", "x\".png", "x.svg", ".png", "a/b.png"};
        for (size_t i = 0; i < sizeof(names)/sizeof(names[0]); ++i) {
            strcpy(e.attachment, names[i]); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_INVALID_ARGUMENT);
        }
        strcpy(e.attachment, "frame-000001.png"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
    } else if (strcmp(test, "privacy") == 0) {
        UmiUiAutomationStepResult result = {0}; result.step.operation = UMI_UI_AUTOMATION_TYPE_TEXT;
        strcpy(result.step.target_id, "notes.entry"); strcpy(result.step.value, "DO-NOT-EXPORT-THIS");
        strcpy(result.observation.text, "DO-NOT-EXPORT-THIS"); strcpy(result.message, "DO-NOT-EXPORT-THIS");
        CHECK(UmiUiRecordingAppendAutomation(r, 12U, &result) == UMI_STATUS_OK);
        CHECK(UmiUiRecordingWriteJson(r, Write, b) == UMI_STATUS_OK);
        CHECK(strstr(b->text, "DO-NOT-EXPORT-THIS") == NULL);
        CHECK(strstr(b->text, "notes.entry") != NULL);
    } else if (strcmp(test, "escaping") == 0) {
        strcpy(e.detail, "<script>\"&\\\n\t\001"); CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
        CHECK(UmiUiRecordingWriteJson(r, Write, b) == UMI_STATUS_OK);
        CHECK(strstr(b->text, "\\u0001") != NULL && strstr(b->text, "\\u000a") != NULL);
        memset(b, 0, sizeof(*b)); CHECK(UmiUiRecordingWriteHtml(r, Write, b) == UMI_STATUS_OK);
        CHECK(strstr(b->text, "<script>") == NULL && strstr(b->text, "&lt;script&gt;") != NULL);
        CHECK(strstr(b->text, "Not asserted") != NULL);
    } else if (strcmp(test, "sink-failure") == 0) {
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
        b->failAt = 4U; CHECK(UmiUiRecordingWriteJson(r, Write, b) == UMI_STATUS_IO_ERROR);
        CHECK(b->calls == 4U);
        memset(b, 0, sizeof(*b)); b->failAt = 1U;
        CHECK(UmiUiRecordingWriteHtml(r, Write, b) == UMI_STATUS_IO_ERROR && b->calls == 1U);
        CHECK(UmiUiRecordingCount(r) == 1U);
    } else if (strcmp(test, "streaming") == 0) {
        CHECK(UmiUiRecordingAppend(r, &e) == UMI_STATUS_OK);
        CHECK(UmiUiRecordingWriteEntryJson(r, 0U, Write, b) == UMI_STATUS_OK);
        CHECK(b->text[0] == '{' && b->text[b->used-1U] == '\n');
        CHECK(strstr(b->text, "\"sequence\":1") != NULL);
        CHECK(UmiUiRecordingWriteEntryJson(r, 1U, Write, b) == UMI_STATUS_NOT_FOUND);
    } else { fprintf(stderr, "Unknown case: %s\n", test); return 2; }
    CHECK(UmiUiRecordingAt(NULL, 0U, &got) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUiRecordingWriteJson(r, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiUiRecordingDestroy(r); UmiUiRecordingDestroy(NULL); free(b); return 0;
}
