/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/games/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Games using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot GAMES_STANDARD_SLOTS[] = {
    {"umicom.games.scene", "umicom.games.scene", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1,
     1},
    {"umicom.games.viewport", "umicom.games.viewport", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 0},
    {"umicom.games.assets", "umicom.games.assets", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.games.behaviour", "umicom.games.behaviour", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.games.profiler", "umicom.games.profiler", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U,
     1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe GAMES_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.games.standard",
    "org.umicom.games",
    "Umicom Games Standard",
    "The balanced everyday Umicom Games workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    GAMES_STANDARD_SLOTS,
    sizeof(GAMES_STANDARD_SLOTS) / sizeof(GAMES_STANDARD_SLOTS[0])};
