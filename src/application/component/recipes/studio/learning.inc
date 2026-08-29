/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Studio IDE using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot STUDIO_LEARNING_SLOTS[] = {
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.build", "umicom.development.build",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.studio.learning",
    "org.umicom.studio",
    "Umicom Studio IDE Learning",
    "A calm starting layout for learning Umicom Studio IDE with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    STUDIO_LEARNING_SLOTS,
    sizeof(STUDIO_LEARNING_SLOTS) / sizeof(STUDIO_LEARNING_SLOTS[0])};
