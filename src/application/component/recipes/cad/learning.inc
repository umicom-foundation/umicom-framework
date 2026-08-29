/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/cad/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom CAD using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CAD_LEARNING_SLOTS[] = {
    {"umicom.cad.viewport", "umicom.cad.viewport", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1,
     1},
    {"umicom.cad.model-tree", "umicom.cad.model-tree", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1,
     0},
    {"umicom.cad.properties", "umicom.cad.properties", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe CAD_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.cad.learning",
    "org.umicom.cad",
    "Umicom CAD Learning",
    "A calm starting layout for learning Umicom CAD with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    CAD_LEARNING_SLOTS,
    sizeof(CAD_LEARNING_SLOTS) / sizeof(CAD_LEARNING_SLOTS[0])};
