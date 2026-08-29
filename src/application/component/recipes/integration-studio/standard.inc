/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/integration-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Integration Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot INTEGRATION_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.integration.designer", "umicom.integration.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.integration.monitor", "umicom.integration.monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.shared.messaging", "umicom.shared.messaging", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     2U, 1, 0},
    {"umicom.shared.operations", "umicom.shared.operations", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe INTEGRATION_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.integration-studio.standard",
    "org.umicom.integration-studio",
    "Umicom Integration Studio Standard",
    "The balanced everyday Umicom Integration Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    INTEGRATION_STUDIO_STANDARD_SLOTS,
    sizeof(INTEGRATION_STUDIO_STANDARD_SLOTS) / sizeof(INTEGRATION_STUDIO_STANDARD_SLOTS[0])};
