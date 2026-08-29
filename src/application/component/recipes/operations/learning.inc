/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/operations/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Operations using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OPERATIONS_LEARNING_SLOTS[] = {
    {"umicom.operations.health", "umicom.operations.health",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.operations.logs", "umicom.operations.logs", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
    {"umicom.operations.metrics", "umicom.operations.metrics",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe OPERATIONS_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.operations.learning",
    "org.umicom.operations",
    "Umicom Operations Learning",
    "A calm starting layout for learning Umicom Operations with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    OPERATIONS_LEARNING_SLOTS,
    sizeof(OPERATIONS_LEARNING_SLOTS) / sizeof(OPERATIONS_LEARNING_SLOTS[0])};
