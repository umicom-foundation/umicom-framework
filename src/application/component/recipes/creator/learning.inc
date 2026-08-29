/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/creator/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom AI Creator using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CREATOR_LEARNING_SLOTS[] = {
    {"umicom.creator.canvas", "umicom.creator.canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.creator.prompt", "umicom.creator.prompt", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1,
     0},
    {"umicom.creator.variants", "umicom.creator.variants", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe CREATOR_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.creator.learning",
    "org.umicom.creator",
    "Umicom AI Creator Learning",
    "A calm starting layout for learning Umicom AI Creator with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    CREATOR_LEARNING_SLOTS,
    sizeof(CREATOR_LEARNING_SLOTS) / sizeof(CREATOR_LEARNING_SLOTS[0]),
    "org.umicom.experience.creator.learning"};
