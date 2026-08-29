/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/integration-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Integration Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot INTEGRATION_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.integration.designer", "umicom.integration.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U, 1, 1},
    {"umicom.integration.monitor", "umicom.integration.monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.shared.messaging", "umicom.shared.messaging", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe INTEGRATION_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.integration-studio.focus",
    "org.umicom.integration-studio",
    "Umicom Integration Studio Focus",
    "A distraction-reduced Umicom Integration Studio layout centred on the main task and its "
    "closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    INTEGRATION_STUDIO_FOCUS_SLOTS,
    sizeof(INTEGRATION_STUDIO_FOCUS_SLOTS) / sizeof(INTEGRATION_STUDIO_FOCUS_SLOTS[0]),
    "org.umicom.experience.integration-studio.focus"};
