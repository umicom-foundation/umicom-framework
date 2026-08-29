/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/marketplace/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Marketplace using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MARKETPLACE_LEARNING_SLOTS[] = {
    {"umicom.shared.product", "umicom.shared.product", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.shared.application-federation", "umicom.shared.application-federation",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe MARKETPLACE_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.marketplace.learning",
    "org.umicom.marketplace",
    "Umicom Marketplace Learning",
    "A calm starting layout for learning Umicom Marketplace with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    MARKETPLACE_LEARNING_SLOTS,
    sizeof(MARKETPLACE_LEARNING_SLOTS) / sizeof(MARKETPLACE_LEARNING_SLOTS[0]),
    "org.umicom.experience.marketplace.learning"};
