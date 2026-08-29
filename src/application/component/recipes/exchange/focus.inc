/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/exchange/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Commodity Exchange using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot EXCHANGE_FOCUS_SLOTS[] = {
    {"umicom.shared.exchange", "umicom.shared.exchange", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.shared.market-data", "umicom.shared.market-data",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.shared.surveillance", "umicom.shared.surveillance",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe EXCHANGE_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.exchange.focus",
    "org.umicom.exchange",
    "Umicom Commodity Exchange Focus",
    "A distraction-reduced Umicom Commodity Exchange layout centred on the main task and its "
    "closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    EXCHANGE_FOCUS_SLOTS,
    sizeof(EXCHANGE_FOCUS_SLOTS) / sizeof(EXCHANGE_FOCUS_SLOTS[0]),
    "org.umicom.experience.exchange.focus"};
