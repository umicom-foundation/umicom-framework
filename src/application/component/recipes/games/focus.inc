/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/games/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Games using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot GAMES_FOCUS_SLOTS[] = {
    {"umicom.games.viewport", "umicom.games.viewport", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.games.scene", "umicom.games.scene", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.games.profiler", "umicom.games.profiler", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe GAMES_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.games.focus",
    "org.umicom.games",
    "Umicom Games Focus",
    "A distraction-reduced Umicom Games layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    GAMES_FOCUS_SLOTS,
    sizeof(GAMES_FOCUS_SLOTS) / sizeof(GAMES_FOCUS_SLOTS[0])};
