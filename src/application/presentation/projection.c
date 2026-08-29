/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/projection.c
 *
 * PURPOSE:
 *   Join workspace recipes, panel specifications and window specifications
 *   into an ordered frontend-neutral application presentation plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/projection.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/presentation/panel_catalogue.h"

UmiStatus umi_application_presentation_project(
    const char *recipe_id,
    UmiApplicationPresentationPlan *out_plan)
{
    const UmiApplicationComponentRecipe *recipe;
    const UmiApplicationPresentationWindowSpec *window;
    size_t index;

    if (recipe_id == NULL || recipe_id[0] == '\0' || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    recipe = umi_application_component_recipe_catalogue_find(recipe_id);
    if (recipe == NULL) return UMI_STATUS_NOT_FOUND;
    window = umi_application_presentation_window_catalogue_find(recipe_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (recipe->slot_count > UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_plan->recipe = recipe;
    out_plan->window = window;
    for (index = 0U; index < recipe->slot_count; ++index) {
        const UmiApplicationComponentRecipeSlot *slot = &recipe->slots[index];
        const UmiApplicationPresentationPanelSpec *panel =
            umi_application_presentation_panel_catalogue_find(slot->component_id);
        UmiApplicationPresentationPanelPlacement *placement;
        if (panel == NULL) {
            (void)memset(out_plan, 0, sizeof(*out_plan));
            return UMI_STATUS_NOT_FOUND;
        }
        placement = &out_plan->placements[out_plan->placement_count++];
        placement->panel = panel;
        placement->slot = slot;
        placement->order = index;
        if (slot->visible) out_plan->visible_count += 1U;
        if (slot->locked) out_plan->locked_count += 1U;
    }
    return UMI_STATUS_OK;
}
