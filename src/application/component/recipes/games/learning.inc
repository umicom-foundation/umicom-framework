/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/games/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Games using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot GAMES_LEARNING_SLOTS[] = {
    {"umicom.games.scene", "umicom.games.scene", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1,
     1},
    {"umicom.games.assets", "umicom.games.assets", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.games.viewport", "umicom.games.viewport", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
};

static const UmiApplicationComponentRecipe GAMES_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.games.learning",
    "org.umicom.games",
    "Umicom Games Learning",
    "A calm starting layout for learning Umicom Games with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    GAMES_LEARNING_SLOTS,
    sizeof(GAMES_LEARNING_SLOTS) / sizeof(GAMES_LEARNING_SLOTS[0]),
    "org.umicom.experience.games.learning"};
