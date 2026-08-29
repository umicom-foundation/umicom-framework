/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/database-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Database Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DATABASE_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.database.query", "umicom.database.query", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.database.explorer", "umicom.database.explorer", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.database.migrations", "umicom.database.migrations",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe DATABASE_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.database-studio.learning",
    "org.umicom.database-studio",
    "Umicom Database Studio Learning",
    "A calm starting layout for learning Umicom Database Studio with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    DATABASE_STUDIO_LEARNING_SLOTS,
    sizeof(DATABASE_STUDIO_LEARNING_SLOTS) / sizeof(DATABASE_STUDIO_LEARNING_SLOTS[0]),
    "org.umicom.experience.database-studio.learning"};
