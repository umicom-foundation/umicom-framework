/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/database-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Database Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DATABASE_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.database.query", "umicom.database.query", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U,
     1, 1},
    {"umicom.database.explorer", "umicom.database.explorer", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     2U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe DATABASE_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.database-studio.focus",
    "org.umicom.database-studio",
    "Umicom Database Studio Focus",
    "A distraction-reduced Umicom Database Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    DATABASE_STUDIO_FOCUS_SLOTS,
    sizeof(DATABASE_STUDIO_FOCUS_SLOTS) / sizeof(DATABASE_STUDIO_FOCUS_SLOTS[0]),
    "org.umicom.experience.database-studio.focus"};
