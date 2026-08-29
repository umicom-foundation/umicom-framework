/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/cad/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom CAD using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CAD_FOCUS_SLOTS[] = {
    {"umicom.cad.viewport", "umicom.cad.viewport", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U, 1,
     1},
    {"umicom.cad.model-tree", "umicom.cad.model-tree", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.cad.properties", "umicom.cad.properties", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
};

static const UmiApplicationComponentRecipe CAD_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.cad.focus",
    "org.umicom.cad",
    "Umicom CAD Focus",
    "A distraction-reduced Umicom CAD layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    CAD_FOCUS_SLOTS,
    sizeof(CAD_FOCUS_SLOTS) / sizeof(CAD_FOCUS_SLOTS[0])};
