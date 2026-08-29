/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/kitchen-designer/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Kitchen Designer using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot KITCHEN_DESIGNER_LEARNING_SLOTS[] = {
    {"umicom.kitchen.planner", "umicom.kitchen.planner", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     4U, 1, 1},
    {"umicom.kitchen.catalogue", "umicom.kitchen.catalogue", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.kitchen.costing", "umicom.kitchen.costing", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe KITCHEN_DESIGNER_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.kitchen-designer.learning",
    "org.umicom.kitchen-designer",
    "Umicom Kitchen Designer Learning",
    "A calm starting layout for learning Umicom Kitchen Designer with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    KITCHEN_DESIGNER_LEARNING_SLOTS,
    sizeof(KITCHEN_DESIGNER_LEARNING_SLOTS) / sizeof(KITCHEN_DESIGNER_LEARNING_SLOTS[0]),
    "org.umicom.experience.kitchen-designer.learning"};
