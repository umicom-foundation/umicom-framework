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
};

/* Check fixed text storage before any string function searches past its end. */
static int automation_has_terminator(const char *text, size_t capacity)
{
    return text != NULL && capacity > 0U &&
           memchr(text, '\0', capacity) != NULL;
}

/* Confirm that an operation is one of the public values before a driver sees it. */
static int automation_operation_is_valid(UmiUiAutomationOperation operation)
{
    return operation >= UMI_UI_AUTOMATION_FOCUS &&
           operation <= UMI_UI_AUTOMATION_CAPTURE_EVIDENCE;
}

/*
 * Validate a step at the storage boundary. Stable identifiers are mandatory
 * because captions can be translated and screen coordinates move with layout.
 */
static UmiStatus automation_step_validate(const UmiUiAutomationStep *step)
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

    return UMI_STATUS_OK;
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

    status = automation_step_validate(step);
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

/* Execute scenario steps in order and retain every attempted result as evidence. */
UmiStatus umi_ui_automation_run(
    const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario,
    UmiUiAutomationReport **out_report)
{
    UmiUiAutomationReport *report;
    size_t index;
    UmiStatus status;

    if (out_report == NULL || scenario == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_report = NULL;

    status = umi_ui_automation_driver_validate(driver);
    if (status != UMI_STATUS_OK) return status;

    /* Reports are deliberately heap-owned because their evidence can be large. */
    report = (UmiUiAutomationReport *)calloc(1U, sizeof(*report));
    if (report == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    if (!umi_ui_copy_text(
            report->scenario_id,
            sizeof(report->scenario_id),
            scenario->scenario_id) ||
        !umi_ui_copy_text(
            report->driver_id,
            sizeof(report->driver_id),
            driver->driver_id)) {
        free(report);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < scenario->step_count; ++index) {
        UmiUiAutomationStepResult *result = &report->results[report->total];

        result->step = scenario->steps[index];
        result->status = driver->perform(
            driver->context,
            &result->step,
            &result->observation,
            result->message,
            sizeof(result->message));
        /* A third-party adapter cannot leave report strings unterminated. */
        result->message[sizeof(result->message) - 1U] = '\0';
        result->observation.target_id[
            sizeof(result->observation.target_id) - 1U] = '\0';
        result->observation.role_name[
            sizeof(result->observation.role_name) - 1U] = '\0';
        result->observation.text[
            sizeof(result->observation.text) - 1U] = '\0';
        report->total += 1U;

        if (result->status == UMI_STATUS_OK) {
            report->passed += 1U;
        } else {
            report->failed += 1U;

            /* Stop only when later steps are likely to depend on this failure. */
            if (!scenario->continue_on_failure) break;
        }
    }

    *out_report = report;
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
