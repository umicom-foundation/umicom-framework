/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/kitchen-designer/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Kitchen Designer using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot KITCHEN_DESIGNER_FOCUS_SLOTS[] = {
    {"umicom.kitchen.planner", "umicom.kitchen.planner", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.kitchen.catalogue", "umicom.kitchen.catalogue", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     2U, 1, 0},
    {"umicom.kitchen.costing", "umicom.kitchen.costing", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe KITCHEN_DESIGNER_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.kitchen-designer.focus",
    "org.umicom.kitchen-designer",
    "Umicom Kitchen Designer Focus",
    "A distraction-reduced Umicom Kitchen Designer layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    KITCHEN_DESIGNER_FOCUS_SLOTS,
    sizeof(KITCHEN_DESIGNER_FOCUS_SLOTS) / sizeof(KITCHEN_DESIGNER_FOCUS_SLOTS[0])};
