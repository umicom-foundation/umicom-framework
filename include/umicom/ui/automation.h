/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/automation.h
 *
 * PURPOSE:
 *   Define toolkit-neutral user-interface automation scenarios. Applications
 *   describe what a person would do, while a native adapter performs the same
 *   actions against real controls and returns observable evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_AUTOMATION_H
#define UMICOM_UI_AUTOMATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_AUTOMATION_API_VERSION 1U
#define UMI_UI_AUTOMATION_STEP_MAX 128U
#define UMI_UI_AUTOMATION_MESSAGE_CAPACITY 512U

/**
 * Name the actions that an automated acceptance scenario can ask a desktop
 * adapter to perform. The names describe user intent rather than GTK, Windows
 * or another toolkit, so the same scenario can run on several platforms.
 */
typedef enum UmiUiAutomationOperation {
    UMI_UI_AUTOMATION_FOCUS = 1,
    UMI_UI_AUTOMATION_CLICK = 2,
    UMI_UI_AUTOMATION_TYPE_TEXT = 3,
    UMI_UI_AUTOMATION_SELECT = 4,
    UMI_UI_AUTOMATION_TOGGLE = 5,
    UMI_UI_AUTOMATION_OPEN_MENU = 6,
    UMI_UI_AUTOMATION_INVOKE_COMMAND = 7,
    UMI_UI_AUTOMATION_WAIT_VISIBLE = 8,
    UMI_UI_AUTOMATION_WAIT_ENABLED = 9,
    UMI_UI_AUTOMATION_ASSERT_TEXT = 10,
    UMI_UI_AUTOMATION_CAPTURE_EVIDENCE = 11
} UmiUiAutomationOperation;

/**
 * Describe one small user action. target_id is a stable automation identifier,
 * not a translated caption or screen coordinate. value carries typed text, a
 * selection value, a command argument or expected text when an operation needs
 * one. timeout_ms limits wait operations so a broken screen cannot hang a run.
 */
typedef struct UmiUiAutomationStep {
    char step_id[UMI_UI_ID_CAPACITY];
    char target_id[UMI_UI_ID_CAPACITY];
    UmiUiAutomationOperation operation;
    char value[UMI_UI_DESCRIPTION_CAPACITY];
    uint32_t timeout_ms;
} UmiUiAutomationStep;

/**
 * Record what the driver could observe after an action. Reports keep this
 * snapshot as evidence, which makes failures useful without relying only on a
 * screenshot.
 */
typedef struct UmiUiAutomationObservation {
    char target_id[UMI_UI_ID_CAPACITY];
    char role_name[UMI_UI_ID_CAPACITY];
    char text[UMI_UI_DESCRIPTION_CAPACITY];
    int visible;
    int enabled;
    int focused;
    int selected;
    uint64_t revision;
} UmiUiAutomationObservation;

/**
 * Receive a scenario step in a platform adapter. The adapter writes the state
 * it observed and a short explanation suitable for a test report.
 */
typedef UmiStatus (*UmiUiAutomationPerformFn)(
    void *context,
    const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out_observation,
    char *out_message,
    size_t message_capacity);

/**
 * Connect the scenario runner to a native or headless implementation. context
 * remains owned by the adapter and must stay alive for the whole run.
 */
typedef struct UmiUiAutomationDriver {
    uint32_t structure_size;
    uint32_t api_version;
    char driver_id[UMI_UI_ID_CAPACITY];
    void *context;
    UmiUiAutomationPerformFn perform;
} UmiUiAutomationDriver;

/** Keep scenario storage private so callers cannot overrun its bounded list. */
typedef struct UmiUiAutomationScenario UmiUiAutomationScenario;

/** Keep report storage private because a complete evidence report is large. */
typedef struct UmiUiAutomationReport UmiUiAutomationReport;

/**
 * Summarise one completed step without exposing the report's internal storage.
 */
typedef struct UmiUiAutomationStepResult {
    UmiUiAutomationStep step;
    UmiStatus status;
    char message[UMI_UI_AUTOMATION_MESSAGE_CAPACITY];
    UmiUiAutomationObservation observation;
} UmiUiAutomationStepResult;

/** Create an empty scenario that owns copies of every step added to it. */
UmiStatus umi_ui_automation_scenario_create(
    const char *scenario_id,
    const char *title,
    UmiUiAutomationScenario **out_scenario);

/** Release the scenario and all copied step data. */
void umi_ui_automation_scenario_destroy(UmiUiAutomationScenario *scenario);

/**
 * Choose whether later steps should run after a failure. Continuing is useful
 * for a wiring audit; stopping is useful when later actions depend on earlier
 * navigation.
 */
UmiStatus umi_ui_automation_scenario_set_continue_on_failure(
    UmiUiAutomationScenario *scenario,
    int continue_on_failure);

/** Add one validated, copied action to the end of a scenario. */
UmiStatus umi_ui_automation_scenario_add(
    UmiUiAutomationScenario *scenario,
    const UmiUiAutomationStep *step);

/** Return the number of actions currently held by the scenario. */
size_t umi_ui_automation_scenario_count(const UmiUiAutomationScenario *scenario);

/** Copy one scenario action into caller-owned storage. */
UmiStatus umi_ui_automation_scenario_at(
    const UmiUiAutomationScenario *scenario,
    size_t index,
    UmiUiAutomationStep *out_step);

/** Check the version, callback and identifier supplied by an adapter. */
UmiStatus umi_ui_automation_driver_validate(const UmiUiAutomationDriver *driver);

/**
 * Run every scenario action through the driver and allocate a durable report.
 * A completed run returns OK even when an acceptance step failed; callers read
 * the failure count to distinguish product behaviour from runner failure.
 */
UmiStatus umi_ui_automation_run(
    const UmiUiAutomationDriver *driver,
    const UmiUiAutomationScenario *scenario,
    UmiUiAutomationReport **out_report);

/** Release a report and its copied observations. */
void umi_ui_automation_report_destroy(UmiUiAutomationReport *report);

/** Return how many steps were attempted. */
size_t umi_ui_automation_report_total(const UmiUiAutomationReport *report);

/** Return how many attempted steps completed successfully. */
size_t umi_ui_automation_report_passed(const UmiUiAutomationReport *report);

/** Return how many attempted steps reported an error or unmet assertion. */
size_t umi_ui_automation_report_failed(const UmiUiAutomationReport *report);

/** Copy one result into caller-owned storage for a console, panel or HTML report. */
UmiStatus umi_ui_automation_report_at(
    const UmiUiAutomationReport *report,
    size_t index,
    UmiUiAutomationStepResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
