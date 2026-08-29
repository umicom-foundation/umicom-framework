/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/trader/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Trader using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TRADER_FOCUS_SLOTS[] = {
    {"umicom.trading.chart", "umicom.trading.chart", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.trading.order-ticket", "umicom.trading.order-ticket",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.trading.executions", "umicom.trading.executions",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe TRADER_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.trader.focus",
    "org.umicom.trader",
    "Umicom Trader Focus",
    "A distraction-reduced Umicom Trader layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    TRADER_FOCUS_SLOTS,
    sizeof(TRADER_FOCUS_SLOTS) / sizeof(TRADER_FOCUS_SLOTS[0])};
