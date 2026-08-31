/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_render_plan.c
 * PURPOSE: Verify canonical Trader geometry becomes deterministic GTK-ready stacks.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include "umicom/application/suite_layout/suite_layout.h"
int main(void)
{
    UmiUiWorkspaceLayout layout;
    UmiApplicationSuiteLayoutRenderPlan plan;
    assert(umi_application_suite_layout_project_default(
        "org.umicom.trader", &layout) == UMI_STATUS_OK);
    assert(umi_application_suite_layout_render_plan_build(
        &layout, &plan) == UMI_STATUS_OK);
    assert(plan.visible_window_count == 8U);
    assert(umi_application_suite_layout_render_plan_count_placement(
        &plan, UMI_UI_PLACEMENT_LEFT) == 1U);
    assert(umi_application_suite_layout_render_plan_count_placement(
        &plan, UMI_UI_PLACEMENT_CENTRE) == 1U);
    assert(umi_application_suite_layout_render_plan_count_placement(
        &plan, UMI_UI_PLACEMENT_RIGHT) == 1U);
    assert(umi_application_suite_layout_render_plan_count_placement(
        &plan, UMI_UI_PLACEMENT_BOTTOM) == 1U);
    assert(layout.windows[0].placement_id[0] != '\0');
    assert(layout.windows[0].stack_id[0] != '\0');
    /* A linked context is independent from placement and does not change the
     * region selected by the renderer. */
    (void)snprintf(layout.windows[0].context_group_id,
                   sizeof(layout.windows[0].context_group_id), "market-red");
    assert(umi_application_suite_layout_render_plan_build(
        &layout, &plan) == UMI_STATUS_OK);
    return 0;
}
