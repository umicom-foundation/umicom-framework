/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_render_all.c
 * PURPOSE: Ensure every canonical Application Suite default layout has a render plan.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
int main(void)
{
    size_t i;
    for (i = 0U; i < umi_application_experience_catalogue_count(); ++i) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(i);
        UmiUiWorkspaceLayout layout;
        UmiApplicationSuiteLayoutRenderPlan plan;
        assert(experience != NULL);
        assert(umi_application_suite_layout_project_default(
            experience->application_id, &layout) == UMI_STATUS_OK);
        assert(umi_application_suite_layout_render_plan_build(
            &layout, &plan) == UMI_STATUS_OK);
        assert(plan.visible_window_count > 0U);
    }
    return 0;
}
