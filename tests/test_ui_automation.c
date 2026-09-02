/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_automation.c
 *
 * PURPOSE:
 *   Verify that a reusable UAT scenario copies steps, invokes a driver in order
 *   and keeps observable evidence in a heap-owned report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/automation.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct FakeDriverContext {
    size_t calls;
    char text[UMI_UI_DESCRIPTION_CAPACITY];
} FakeDriverContext;

/* Simulate an adapter so the core runner can be tested without opening a GUI. */
static UmiStatus fake_perform(
    void *context,
    const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out_observation,
    char *out_message,
    size_t message_capacity)
{
    FakeDriverContext *fake = (FakeDriverContext *)context;

    if (fake == NULL || step == NULL || out_observation == NULL ||
        out_message == NULL || message_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    fake->calls += 1U;
    if (step->operation == UMI_UI_AUTOMATION_TYPE_TEXT) {
        (void)snprintf(fake->text, sizeof(fake->text), "%s", step->value);
    }

    (void)memset(out_observation, 0, sizeof(*out_observation));
    (void)snprintf(
        out_observation->target_id,
        sizeof(out_observation->target_id),
        "%s",
        step->target_id);
    (void)snprintf(
        out_observation->role_name,
        sizeof(out_observation->role_name),
        "%s",
        "text-field");
    (void)snprintf(
        out_observation->text,
        sizeof(out_observation->text),
        "%s",
        fake->text);
    out_observation->visible = 1;
    out_observation->enabled = 1;
    out_observation->revision = (uint64_t)fake->calls;
    (void)snprintf(out_message, message_capacity, "%s", "simulated");
    return UMI_STATUS_OK;
}

/* Exercise a complete two-step journey without opening a native window. */
int main(void)
{
    UmiUiAutomationScenario *scenario = NULL;
    UmiUiAutomationReport *report = NULL;
    UmiUiAutomationStep step;
    UmiUiAutomationStepResult result;
    UmiUiAutomationDriver driver;
    FakeDriverContext context;

    (void)memset(&context, 0, sizeof(context));
    (void)memset(&driver, 0, sizeof(driver));
    driver.structure_size = (uint32_t)sizeof(driver);
    driver.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)snprintf(driver.driver_id, sizeof(driver.driver_id), "%s", "test.driver");
    driver.context = &context;
    driver.perform = fake_perform;

    assert(umi_ui_automation_scenario_create(
        "studio.welcome.search",
        "Type into global search",
        &scenario) == UMI_STATUS_OK);

    (void)memset(&step, 0, sizeof(step));
    (void)snprintf(step.step_id, sizeof(step.step_id), "%s", "enter-search");
    (void)snprintf(step.target_id, sizeof(step.target_id), "%s", "studio.search");
    step.operation = UMI_UI_AUTOMATION_TYPE_TEXT;
    (void)snprintf(step.value, sizeof(step.value), "%s", "main");
    assert(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);

    (void)snprintf(step.step_id, sizeof(step.step_id), "%s", "read-search");
    step.operation = UMI_UI_AUTOMATION_CAPTURE_EVIDENCE;
    step.value[0] = '\0';
    assert(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);

    assert(umi_ui_automation_run(&driver, scenario, &report) == UMI_STATUS_OK);
    assert(umi_ui_automation_report_total(report) == 2U);
    assert(umi_ui_automation_report_passed(report) == 2U);
    assert(umi_ui_automation_report_failed(report) == 0U);
    assert(umi_ui_automation_report_at(report, 1U, &result) == UMI_STATUS_OK);
    assert(strcmp(result.observation.text, "main") == 0);

    umi_ui_automation_report_destroy(report);
    umi_ui_automation_scenario_destroy(scenario);
    return 0;
}
