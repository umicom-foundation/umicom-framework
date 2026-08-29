/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_projection.c
 *
 * PURPOSE:
 *   Verify a Studio workspace recipe becomes one ordered frontend-neutral plan
 *   containing real panel and window specifications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiApplicationPresentationPlan plan;

    assert(umi_application_presentation_project(
               "org.umicom.workspace.studio.standard", &plan) == UMI_STATUS_OK);
    assert(plan.recipe != NULL);
    assert(plan.window != NULL);
    assert(plan.placement_count == plan.recipe->slot_count);
    assert(plan.visible_count > 0U);
    assert(plan.placements[0].panel != NULL);
    assert(plan.placements[0].slot != NULL);
    assert(plan.placements[0].order == 0U);
    assert(strcmp(plan.placements[0].panel->component_id,
                  plan.placements[0].slot->component_id) == 0);
    return 0;
}
