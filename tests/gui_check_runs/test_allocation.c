/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_check_runs/test_allocation.c
 *
 * PURPOSE:
 *   Inject allocation failures at session/report boundaries and check that
 *   failure performs no actions, writes no report and leaks no owned storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/automation_session.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static size_t allocationToFail;
static size_t callbacks;
void *__real_calloc(size_t count, size_t size);
/* The fixture enables this only after building the scenario. */
void *__wrap_calloc(size_t count, size_t size)
{
    if (allocationToFail != 0U && --allocationToFail == 0U) return NULL;
    return __real_calloc(count, size);
}
/* A counter proves that failed construction did not request any UI action. */
static UmiStatus Perform(void *context, const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out, char *message, size_t capacity)
{
    (void)context; (void)step; (void)out; (void)message; (void)capacity;
    ++callbacks;
    return UMI_STATUS_OK;
}
/* A counter proves that failed export did not write a misleading header. */
static UmiStatus Write(void *context, const char *bytes, size_t length)
{
    size_t *calls = context;
    (void)bytes; (void)length;
    ++*calls;
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiUiAutomationScenario *scenario = NULL;
    UmiUiAutomationSession *session = NULL;
    UmiUiAutomationReport *report = NULL;
    UmiUiAutomationStep step = {0};
    UmiUiAutomationDriver driver = {0};
    size_t i, writes = 0U;
    driver.structure_size = (uint32_t)sizeof(driver);
    driver.api_version = UMI_UI_AUTOMATION_API_VERSION;
    strcpy(driver.driver_id, "allocation.fixture"); driver.perform = Perform;
    strcpy(step.step_id, "capture"); strcpy(step.target_id, "notes.editor");
    step.operation = UMI_UI_AUTOMATION_CAPTURE_EVIDENCE;
    REQUIRE(umi_ui_automation_scenario_create("allocation", "Allocation checks", &scenario) == UMI_STATUS_OK);
    REQUIRE(umi_ui_automation_scenario_add(scenario, &step) == UMI_STATUS_OK);
    for (i = 1U; i <= 2U; ++i) {
        allocationToFail = i;
        REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "run", &session) == UMI_STATUS_OUT_OF_MEMORY);
        allocationToFail = 0U;
        REQUIRE(session == NULL && callbacks == 0U);
        allocationToFail = i;
        REQUIRE(umi_ui_automation_run(&driver, scenario, &report) == UMI_STATUS_OUT_OF_MEMORY);
        allocationToFail = 0U;
        REQUIRE(report == NULL && callbacks == 0U);
    }
    REQUIRE(UmiUiAutomationSessionCreate(&driver, scenario, "run", &session) == UMI_STATUS_OK);
    allocationToFail = 1U;
    REQUIRE(UmiUiAutomationReportWriteJson(UmiUiAutomationSessionReport(session), 0, Write, &writes) == UMI_STATUS_OUT_OF_MEMORY);
    allocationToFail = 0U;
    REQUIRE(writes == 0U && callbacks == 0U);
    REQUIRE(UmiUiAutomationSessionAdvance(session) == UMI_STATUS_OK && callbacks == 1U);
    REQUIRE(UmiUiAutomationReportWriteJson(UmiUiAutomationSessionReport(session), 0, Write, &writes) == UMI_STATUS_OK && writes != 0U);
    REQUIRE(UmiUiAutomationSessionDestroy(session) == UMI_STATUS_OK);
    umi_ui_automation_scenario_destroy(scenario);
    puts("PASS allocation failures and successful retry");
    return 0;
}
