/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/database-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Database Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DATABASE_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.database.query", "umicom.database.query", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.database.explorer", "umicom.database.explorer", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     2U, 1, 0},
    {"umicom.database.migrations", "umicom.database.migrations",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.terminal", "umicom.shared.terminal", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe DATABASE_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.database-studio.standard",
    "org.umicom.database-studio",
    "Umicom Database Studio Standard",
    "The balanced everyday Umicom Database Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    DATABASE_STUDIO_STANDARD_SLOTS,
    sizeof(DATABASE_STUDIO_STANDARD_SLOTS) / sizeof(DATABASE_STUDIO_STANDARD_SLOTS[0]),
    "org.umicom.experience.database-studio.standard"};
