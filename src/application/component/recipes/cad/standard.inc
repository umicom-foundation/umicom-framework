/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/cad/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom CAD using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CAD_STANDARD_SLOTS[] = {
    {"umicom.cad.viewport", "umicom.cad.viewport", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1,
     1},
    {"umicom.cad.model-tree", "umicom.cad.model-tree", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.cad.properties", "umicom.cad.properties", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.cad.constraints", "umicom.cad.constraints", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
    {"umicom.design.materials", "umicom.design.materials", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.design.render", "umicom.design.render", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U,
     1, 0},
};

static const UmiApplicationComponentRecipe CAD_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.cad.standard",
    "org.umicom.cad",
    "Umicom CAD Standard",
    "The balanced everyday Umicom CAD workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    CAD_STANDARD_SLOTS,
    sizeof(CAD_STANDARD_SLOTS) / sizeof(CAD_STANDARD_SLOTS[0])};
