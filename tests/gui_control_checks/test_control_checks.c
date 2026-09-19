/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_control_checks.c
 *
 * PURPOSE:
 *   Check copied control maps and independent scenario assertions. Controlled
 *   driver responses test the runner boundary; they are not native GUI tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/control_inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

typedef struct Sink { char bytes[32768]; size_t used; size_t calls; size_t failAt; } Sink;
static UmiStatus Write(void *context, const char *bytes, size_t length)
{
    Sink *sink = context;
    ++sink->calls;
    if (sink->failAt != 0U && sink->calls >= sink->failAt) return UMI_STATUS_IO_ERROR;
    if (length >= sizeof(sink->bytes) - sink->used) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(sink->bytes + sink->used, bytes, length); sink->used += length;
    sink->bytes[sink->used] = '\0'; return UMI_STATUS_OK;
}
static UmiUiControlRecord Record(size_t parent, uint32_t scope, const char *id, const char *role)
{
    UmiUiControlRecord r = {0}; r.parentIndex = parent; r.scopeId = scope;
    (void)snprintf(r.automationId, sizeof(r.automationId), "%s", id);
    (void)snprintf(r.roleName, sizeof(r.roleName), "%s", role);
    r.visible = 1; r.enabled = 1; r.interactive = parent != UMI_UI_CONTROL_NO_PARENT;
    return r;
}
static int InventoryCase(const char *name)
{
    UmiUiControlInventory *inventory = NULL;
    UmiUiControlRecord r = Record(UMI_UI_CONTROL_NO_PARENT, 1U, "", "GtkWindow"), copy;
    UmiUiControlInventorySummary summary;
    Sink sink = {{0},0U,0U,0U}; size_t target;
    CHECK(UmiUiControlInventoryCreate(8U, &inventory) == UMI_STATUS_OK);
    CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
    if (strcmp(name, "invalid-capacity") == 0) {
        UmiUiControlInventory *bad = inventory;
        CHECK(UmiUiControlInventoryCreate(0U, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiUiControlInventoryCreate(UMI_UI_CONTROL_INVENTORY_MAX + 1U, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiUiControlInventoryCreate(1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiUiControlInventoryDestroy(NULL);
    } else if (strcmp(name, "invalid-parent") == 0) {
        r = Record(1U, 1U, "notes.save", "GtkButton");
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r.parentIndex = SIZE_MAX - 1U;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiControlInventoryCount(inventory) == 1U);
    } else if (strcmp(name, "scope-boundary") == 0) {
        r = Record(0U, 2U, "notes.save", "GtkButton");
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r.scopeId = 0U;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "scope-duplicate") == 0) {
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiUiControlInventoryCount(inventory) == 1U);
    } else if (strcmp(name, "malformed-fields") == 0) {
        r = Record(0U, 1U, "notes.save", "GtkButton"); memset(r.automationId, 'x', sizeof(r.automationId));
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r = Record(0U, 1U, "notes.save", "GtkButton"); memset(r.roleName, 'x', sizeof(r.roleName));
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r = Record(0U, 1U, "<script>", "GtkButton");
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r = Record(0U, 1U, "notes.save", "\xc0\xaf");
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiControlInventoryCount(inventory) == 1U);
    } else if (strcmp(name, "invalid-flags") == 0) {
        r = Record(0U, 1U, "notes.save", "GtkButton"); r.enabled = 2;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r.enabled = 1; r.focused = -1;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r.focused = 0; r.mapped = 1; r.visible = 0;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "private") == 0) {
        r = Record(0U, 1U, "", "private-control"); r.privateControl = 1;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        CHECK(UmiUiControlInventoryTarget(inventory, 1U, &target) == UMI_STATUS_NOT_FOUND && target == SIZE_MAX);
        r = Record(1U, 1U, "secret.child", "GtkEntry");
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        r = Record(0U, 1U, "secret.value", "private-control"); r.privateControl = 1;
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiControlInventoryWriteJson(inventory, Write, &sink) == UMI_STATUS_OK);
        CHECK(strstr(sink.bytes, "secret") == NULL);
        CHECK(UmiUiControlInventorySummarise(inventory, &summary) == UMI_STATUS_OK && summary.privateControls == 1U);
    } else if (strcmp(name, "inherited") == 0) {
        r = Record(0U, 1U, "notes.editor", "GtkEntry"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        r = Record(1U, 1U, "", "GtkText"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        CHECK(UmiUiControlInventoryTarget(inventory, 2U, &target) == UMI_STATUS_OK && target == 1U);
        CHECK(UmiUiControlInventorySummarise(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.inheritedTargets == 1U && summary.unaddressableInteractive == 0U);
    } else if (strcmp(name, "duplicate-id") == 0 || strcmp(name, "independent-windows") == 0) {
        r = Record(0U, 1U, "notes.save", "GtkButton"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        if (strcmp(name, "independent-windows") == 0) {
            r = Record(UMI_UI_CONTROL_NO_PARENT, 2U, "", "GtkWindow");
            CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
            r = Record(2U, 2U, "notes.save", "GtkButton");
        }
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        CHECK(UmiUiControlInventorySummarise(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.ambiguousTargets == (strcmp(name, "duplicate-id") == 0 ? 2U : 0U));
        CHECK(summary.windows == (strcmp(name, "duplicate-id") == 0 ? 1U : 2U));
    } else if (strcmp(name, "capacity") == 0) {
        r = Record(0U, 1U, "", "GtkButton");
        for (size_t i = 1U; i < 8U; ++i) CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiUiControlInventoryCount(inventory) == 8U);
        CHECK(UmiUiControlInventoryAt(inventory, 0U, &copy) == UMI_STATUS_OK && strcmp(copy.roleName, "GtkWindow") == 0);
    } else if (strcmp(name, "snapshot-copy") == 0) {
        r = Record(0U, 1U, "notes.save", "GtkButton"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        r.automationId[0] = '\0';
        CHECK(UmiUiControlInventoryAt(inventory, 1U, &copy) == UMI_STATUS_OK && strcmp(copy.automationId, "notes.save") == 0);
        copy.automationId[0] = '\0';
        CHECK(UmiUiControlInventoryAt(inventory, 1U, &copy) == UMI_STATUS_OK && strcmp(copy.automationId, "notes.save") == 0);
    } else if (strncmp(name, "export-", 7U) == 0) {
        r = Record(0U, 1U, "notes.save", "GtkButton"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        r = Record(0U, 1U, "", "GtkButton"); CHECK(UmiUiControlInventoryAdd(inventory, &r) == UMI_STATUS_OK);
        if (strcmp(name, "export-json") == 0) {
            CHECK(UmiUiControlInventoryWriteJson(inventory, Write, &sink) == UMI_STATUS_OK);
            CHECK(strstr(sink.bytes, "\"missing_targets\":1") != NULL);
            CHECK(strstr(sink.bytes, "\"values_recorded\":false") != NULL);
            CHECK(strstr(sink.bytes, "\"target_index\":1") != NULL);
            CHECK(strstr(sink.bytes, "\"parent_index\":null") != NULL);
        } else if (strcmp(name, "export-html") == 0) {
            CHECK(UmiUiControlInventoryWriteHtml(inventory, Write, &sink) == UMI_STATUS_OK);
            CHECK(strstr(sink.bytes, "notes.save") != NULL && strstr(sink.bytes, "No test ID") != NULL);
            CHECK(strstr(sink.bytes, "<script") == NULL);
        } else {
            sink.failAt = 7U;
            CHECK(UmiUiControlInventoryWriteJson(inventory, Write, &sink) == UMI_STATUS_IO_ERROR && sink.calls == 7U);
            memset(&sink, 0, sizeof(sink)); sink.failAt = 3U;
            CHECK(UmiUiControlInventoryWriteHtml(inventory, Write, &sink) == UMI_STATUS_IO_ERROR && sink.calls == 3U);
        }
    } else if (strcmp(name, "inventory-lifecycle") == 0) {
        CHECK(UmiUiControlInventoryCount(NULL) == 0U);
        CHECK(UmiUiControlInventoryAt(inventory, 1U, &copy) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiUiControlInventoryAt(inventory, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiControlInventoryTarget(inventory, 99U, &target) == UMI_STATUS_NOT_FOUND && target == SIZE_MAX);
        CHECK(UmiUiControlInventorySummarise(inventory, &summary) == UMI_STATUS_OK && summary.controls == 1U);
        CHECK(UmiUiControlInventorySummarise(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT && summary.controls == 0U);
    } else CHECK(0);
    UmiUiControlInventoryDestroy(inventory); return 0;
}

typedef struct Fixture { UmiUiAutomationObservation observation; UmiStatus status; unsigned calls; int malformed; } Fixture;
static UmiStatus Perform(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *observation, char *message, size_t capacity)
{
    Fixture *fixture = context; (void)step; ++fixture->calls;
    *observation = fixture->observation;
    if (fixture->malformed) memset(observation->text, 'x', sizeof(observation->text));
    (void)snprintf(message, capacity, "Controlled adapter response"); return fixture->status;
}
static UmiUiAutomationStep Step(UmiUiAutomationOperation operation, const char *value)
{
    UmiUiAutomationStep step = {0}; step.operation = operation; step.timeout_ms = 20U;
    (void)snprintf(step.step_id, sizeof(step.step_id), "check-notes");
    (void)snprintf(step.target_id, sizeof(step.target_id), "notes.status");
    (void)snprintf(step.value, sizeof(step.value), "%s", value); return step;
}
static int AssertionCase(const char *name)
{
    UmiUiAutomationScenario *scenario = NULL; UmiUiAutomationReport *report = NULL;
    UmiUiAutomationDriver driver = {0}; UmiUiAutomationStep step = Step(UMI_UI_AUTOMATION_ASSERT_TEXT, "Saved");
    UmiUiAutomationStepResult result;
    Fixture fixture = {0}; int failed = 0;
    driver.structure_size = sizeof(driver); driver.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)snprintf(driver.driver_id, sizeof(driver.driver_id), "controlled-fixture");
    driver.context = &fixture; driver.perform = Perform;
    (void)snprintf(fixture.observation.target_id, sizeof(fixture.observation.target_id), "notes.status");
    (void)snprintf(fixture.observation.role_name, sizeof(fixture.observation.role_name), "GtkLabel");
    (void)snprintf(fixture.observation.text, sizeof(fixture.observation.text), "Saved");
    if (strcmp(name, "invalid-step") == 0) {
        step = Step(UMI_UI_AUTOMATION_ASSERT_VISIBLE, "yes");
        CHECK(UmiUiAutomationStepValidate(&step) == UMI_STATUS_INVALID_ARGUMENT);
        step = Step(UMI_UI_AUTOMATION_ASSERT_TEXT, "\xed\xa0\x80");
        CHECK(UmiUiAutomationStepValidate(&step) == UMI_STATUS_INVALID_ARGUMENT);
        step = Step(UMI_UI_AUTOMATION_ASSERT_TEXT, "\xe2\x82\xac");
        CHECK(UmiUiAutomationStepValidate(&step) == UMI_STATUS_OK);
        step.operation = (UmiUiAutomationOperation)99;
        CHECK(UmiUiAutomationStepValidate(&step) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "assert-text-mismatch") == 0) { fixture.observation.text[0] = '\0'; failed = 1; }
    else if (strcmp(name, "assert-visible") == 0) step = Step(UMI_UI_AUTOMATION_ASSERT_VISIBLE, "false");
    else if (strcmp(name, "assert-enabled") == 0) { step = Step(UMI_UI_AUTOMATION_ASSERT_ENABLED, "true"); failed = 1; }
    else if (strcmp(name, "assert-focused") == 0) { step = Step(UMI_UI_AUTOMATION_ASSERT_FOCUSED, "true"); fixture.observation.focused = 1; }
    else if (strcmp(name, "assert-selected") == 0) { step = Step(UMI_UI_AUTOMATION_ASSERT_SELECTED, "true"); fixture.observation.selected = 1; }
    else if (strcmp(name, "invalid-observation") == 0) { fixture.observation.enabled = 2; failed = 1; }
    else if (strcmp(name, "wrong-target") == 0) { fixture.observation.target_id[0] = '\0'; failed = 1; }
    else if (strcmp(name, "malformed-response") == 0) { fixture.malformed = 1; failed = 1; }
    else if (strcmp(name, "failed-driver") == 0) { fixture.status = UMI_STATUS_PERMISSION_DENIED; failed = 1; }
    else if (strcmp(name, "wait-observation") == 0) { step = Step(UMI_UI_AUTOMATION_WAIT_VISIBLE, ""); failed = 1; }
    CHECK(umi_ui_automation_scenario_create("notes.check", "Check a Notes outcome", &scenario) == UMI_STATUS_OK);
    CHECK(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);
    if (strcmp(name, "continue-stop") == 0) {
        fixture.observation.text[0] = '\0';
        (void)snprintf(step.step_id, sizeof(step.step_id), "second-check");
        CHECK(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);
        CHECK(umi_ui_automation_run(&driver, scenario, &report) == UMI_STATUS_OK);
        CHECK(umi_ui_automation_report_total(report) == 1U && fixture.calls == 1U);
        CHECK(UmiUiAutomationReportRequireSuccess(report, 2U) != UMI_STATUS_OK);
        umi_ui_automation_report_destroy(report); report = NULL;
        CHECK(umi_ui_automation_scenario_set_continue_on_failure(scenario, 1) == UMI_STATUS_OK);
        CHECK(umi_ui_automation_run(&driver, scenario, &report) == UMI_STATUS_OK);
        CHECK(umi_ui_automation_report_total(report) == 2U && umi_ui_automation_report_failed(report) == 2U);
    } else {
        CHECK(umi_ui_automation_run(&driver, scenario, &report) == UMI_STATUS_OK);
        CHECK(umi_ui_automation_report_total(report) == 1U);
        CHECK(umi_ui_automation_report_failed(report) == (failed ? 1U : 0U));
        CHECK((UmiUiAutomationReportRequireSuccess(report, 1U) == UMI_STATUS_OK) == !failed);
        CHECK(umi_ui_automation_report_at(report, 0U, &result) == UMI_STATUS_OK);
        if (strcmp(name, "failed-driver") == 0) CHECK(result.status == UMI_STATUS_PERMISSION_DENIED);
        if (strcmp(name, "malformed-response") == 0) CHECK(result.observation.text[sizeof(result.observation.text)-1U] == '\0');
        if (strcmp(name, "import-assertion") == 0) {
            UmiUiRecording *recording = NULL; UmiUiRecordingEntry entry;
            CHECK(UmiUiRecordingCreate("notes", "fixture", 2U, &recording) == UMI_STATUS_OK);
            CHECK(UmiUiRecordingSetActive(recording, 1) == UMI_STATUS_OK);
            result.step.operation = UMI_UI_AUTOMATION_ASSERT_SELECTED;
            CHECK(UmiUiRecordingAppendAutomation(recording, 0U, &result) == UMI_STATUS_OK);
            CHECK(UmiUiRecordingAt(recording, 0U, &entry) == UMI_STATUS_OK);
            CHECK(strstr(entry.detail, "Saved") == NULL && entry.outcomeKnown);
            UmiUiRecordingDestroy(recording);
        }
    }
    umi_ui_automation_report_destroy(report); umi_ui_automation_scenario_destroy(scenario); return 0;
}
int main(int argc, char **argv)
{
    const char *name;
    if (argc != 2) return 2;
    name = argv[1];
    if (strncmp(name, "assert-", 7U) == 0 || strcmp(name, "invalid-step") == 0 ||
        strcmp(name, "invalid-observation") == 0 || strcmp(name, "wrong-target") == 0 ||
        strcmp(name, "malformed-response") == 0 || strcmp(name, "failed-driver") == 0 ||
        strcmp(name, "wait-observation") == 0 || strcmp(name, "continue-stop") == 0 ||
        strcmp(name, "import-assertion") == 0) return AssertionCase(name);
    return InventoryCase(name);
}
