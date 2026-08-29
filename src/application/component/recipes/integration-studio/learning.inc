/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/integration-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Integration Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot INTEGRATION_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.integration.designer", "umicom.integration.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.integration.monitor", "umicom.integration.monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.messaging", "umicom.shared.messaging", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe INTEGRATION_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.integration-studio.learning",
    "org.umicom.integration-studio",
    "Umicom Integration Studio Learning",
    "A calm starting layout for learning Umicom Integration Studio with only the essential "
    "surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    INTEGRATION_STUDIO_LEARNING_SLOTS,
    sizeof(INTEGRATION_STUDIO_LEARNING_SLOTS) / sizeof(INTEGRATION_STUDIO_LEARNING_SLOTS[0])};
