/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/marketplace/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Marketplace using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MARKETPLACE_FOCUS_SLOTS[] = {
    {"umicom.shared.product", "umicom.shared.product", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U,
     1, 1},
    {"umicom.shared.application-federation", "umicom.shared.application-federation",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MARKETPLACE_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.marketplace.focus",
    "org.umicom.marketplace",
    "Umicom Marketplace Focus",
    "A distraction-reduced Umicom Marketplace layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    MARKETPLACE_FOCUS_SLOTS,
    sizeof(MARKETPLACE_FOCUS_SLOTS) / sizeof(MARKETPLACE_FOCUS_SLOTS[0]),
    "org.umicom.experience.marketplace.focus"};
