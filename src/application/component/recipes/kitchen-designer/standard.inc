/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/kitchen-designer/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Kitchen Designer using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot KITCHEN_DESIGNER_STANDARD_SLOTS[] = {
    {"umicom.kitchen.planner", "umicom.kitchen.planner", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     5U, 1, 1},
    {"umicom.kitchen.catalogue", "umicom.kitchen.catalogue", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     2U, 1, 0},
    {"umicom.kitchen.costing", "umicom.kitchen.costing", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.cad.properties", "umicom.cad.properties", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
    {"umicom.design.materials", "umicom.design.materials", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.design.render", "umicom.design.render", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U,
     1, 0},
};

static const UmiApplicationComponentRecipe KITCHEN_DESIGNER_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.kitchen-designer.standard",
    "org.umicom.kitchen-designer",
    "Umicom Kitchen Designer Standard",
    "The balanced everyday Umicom Kitchen Designer workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    KITCHEN_DESIGNER_STANDARD_SLOTS,
    sizeof(KITCHEN_DESIGNER_STANDARD_SLOTS) / sizeof(KITCHEN_DESIGNER_STANDARD_SLOTS[0])};
