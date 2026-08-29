/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/operations/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Operations using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OPERATIONS_FOCUS_SLOTS[] = {
    {"umicom.operations.health", "umicom.operations.health",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U, 1, 1},
    {"umicom.operations.metrics", "umicom.operations.metrics",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.operations.logs", "umicom.operations.logs", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     2U, 1, 0},
};

static const UmiApplicationComponentRecipe OPERATIONS_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.operations.focus",
    "org.umicom.operations",
    "Umicom Operations Focus",
    "A distraction-reduced Umicom Operations layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    OPERATIONS_FOCUS_SLOTS,
    sizeof(OPERATIONS_FOCUS_SLOTS) / sizeof(OPERATIONS_FOCUS_SLOTS[0])};
