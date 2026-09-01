/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_render_all.c
 * PURPOSE: Ensure every canonical Application Suite layout has a render plan.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"

/* Project every named layout, including alternatives that are not selected by
 * default, so all application workspaces remain executable recipes. */
int main(void)
{
    UmiUiWorkspaceLayout *layout =
        (UmiUiWorkspaceLayout *)calloc(1U, sizeof(*layout));
    UmiApplicationSuiteLayoutRenderPlan *plan =
        (UmiApplicationSuiteLayoutRenderPlan *)calloc(1U, sizeof(*plan));
    size_t application_index;

    UMI_TEST_REQUIRE(layout != NULL && plan != NULL);
    for (application_index = 0U;
         application_index < umi_application_experience_catalogue_count();
         ++application_index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(application_index);
        size_t layout_index;
        UMI_TEST_REQUIRE(experience != NULL);
        for (layout_index = 0U;
             layout_index < experience->layout_count;
             ++layout_index) {
            const UmiExperienceLayoutDefinition *definition =
                &experience->layouts[layout_index];
            UMI_TEST_REQUIRE(umi_application_suite_layout_project(
                experience, definition->layout_id, layout) == UMI_STATUS_OK);
            UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
                layout, plan) == UMI_STATUS_OK);
            UMI_TEST_REQUIRE(plan->visible_window_count ==
                             definition->panel_count);
        }
    }
    free(plan);
    free(layout);
    return 0;
}
