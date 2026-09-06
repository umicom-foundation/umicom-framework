/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_trader.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiApplicationSuiteLayoutRuntime *r;
    UmiApplicationSuiteLayoutSnapshot s;
    char expected_active_layout[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int written;
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.trader");
    const UmiExperienceLayoutDefinition *default_layout;
    const UmiExperienceLayoutDefinition *research;

    /* Derive expectations from Trader's immutable recipes so the test remains
     * correct when a new workspace panel is added. */
    UMI_TEST_REQUIRE(experience != NULL);
    default_layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    UMI_TEST_REQUIRE(default_layout != NULL);
    research = umi_application_experience_layout_find(experience, "research");
    UMI_TEST_REQUIRE(research != NULL);
    /* Keep the embedded workspace layout in heap storage so this test does
     * not depend on the native stack size selected by the host process. */
    r = (UmiApplicationSuiteLayoutRuntime *)calloc(1U, sizeof(*r));
    UMI_TEST_REQUIRE(r != NULL);
    umi_application_suite_layout_runtime_init(r);
    UMI_TEST_REQUIRE(umi_application_suite_layout_runtime_load(
                         r, "org.umicom.trader") == UMI_STATUS_OK);
    s=umi_application_suite_layout_runtime_snapshot(r);
    UMI_TEST_REQUIRE(s.loaded);
    UMI_TEST_REQUIRE(s.window_count == default_layout->panel_count);
    /* Runtime snapshots expose the qualified ID used by persisted layouts;
     * derive it from the same application and default recipe metadata. */
    written = snprintf(expected_active_layout, sizeof(expected_active_layout),
                       "%s.%s", experience->application_id,
                       experience->default_layout_id);
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(expected_active_layout));
    UMI_TEST_REQUIRE(strcmp(s.active_layout_id, expected_active_layout) == 0);
    UMI_TEST_REQUIRE(umi_application_suite_layout_runtime_select(
                         r, "research") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_suite_layout_runtime_active(r) != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_runtime_active(
                         r)->window_count == research->panel_count);
    free(r);
    return 0;
}
