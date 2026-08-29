/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/trader/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Trader using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TRADER_STANDARD_SLOTS[] = {
    {"umicom.trading.chart", "umicom.trading.chart", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U,
     1, 1},
    {"umicom.trading.watchlist", "umicom.trading.watchlist", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.trading.market-depth", "umicom.trading.market-depth",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.trading.order-ticket", "umicom.trading.order-ticket",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.trading.orders", "umicom.trading.orders", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
    {"umicom.trading.executions", "umicom.trading.executions",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.trading.portfolio", "umicom.trading.portfolio",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.trading.risk", "umicom.trading.risk", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
    {"umicom.trading.strategy", "umicom.trading.strategy", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     3U, 1, 0},
};

static const UmiApplicationComponentRecipe TRADER_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.trader.standard",
    "org.umicom.trader",
    "Umicom Trader Standard",
    "The balanced everyday Umicom Trader workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    TRADER_STANDARD_SLOTS,
    sizeof(TRADER_STANDARD_SLOTS) / sizeof(TRADER_STANDARD_SLOTS[0])};
