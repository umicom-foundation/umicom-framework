/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/trader/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Trader using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TRADER_LEARNING_SLOTS[] = {
    {"umicom.trading.chart", "umicom.trading.chart", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.trading.watchlist", "umicom.trading.watchlist", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.trading.order-ticket", "umicom.trading.order-ticket",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe TRADER_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.trader.learning",
    "org.umicom.trader",
    "Umicom Trader Learning",
    "A calm starting layout for learning Umicom Trader with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    TRADER_LEARNING_SLOTS,
    sizeof(TRADER_LEARNING_SLOTS) / sizeof(TRADER_LEARNING_SLOTS[0]),
    "org.umicom.experience.trader.learning"};
