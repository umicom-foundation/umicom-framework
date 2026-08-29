/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/operations/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Operations using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OPERATIONS_STANDARD_SLOTS[] = {
    {"umicom.operations.health", "umicom.operations.health",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.operations.logs", "umicom.operations.logs", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     2U, 1, 0},
    {"umicom.operations.metrics", "umicom.operations.metrics",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.shared.operations", "umicom.shared.operations", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.shared.resilience", "umicom.shared.resilience", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.security", "umicom.shared.security", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe OPERATIONS_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.operations.standard",
    "org.umicom.operations",
    "Umicom Operations Standard",
    "The balanced everyday Umicom Operations workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    OPERATIONS_STANDARD_SLOTS,
    sizeof(OPERATIONS_STANDARD_SLOTS) / sizeof(OPERATIONS_STANDARD_SLOTS[0]),
    "org.umicom.experience.operations.standard"};
