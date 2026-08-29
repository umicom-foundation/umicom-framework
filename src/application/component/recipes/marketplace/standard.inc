/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/marketplace/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Marketplace using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MARKETPLACE_STANDARD_SLOTS[] = {
    {"umicom.shared.product", "umicom.shared.product", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.shared.application-federation", "umicom.shared.application-federation",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.shared.web", "umicom.shared.web", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.shared.exchange", "umicom.shared.exchange", UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
     1U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MARKETPLACE_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.marketplace.standard",
    "org.umicom.marketplace",
    "Umicom Marketplace Standard",
    "The balanced everyday Umicom Marketplace workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    MARKETPLACE_STANDARD_SLOTS,
    sizeof(MARKETPLACE_STANDARD_SLOTS) / sizeof(MARKETPLACE_STANDARD_SLOTS[0]),
    "org.umicom.experience.marketplace.standard"};
