/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/exchange/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Commodity Exchange using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot EXCHANGE_LEARNING_SLOTS[] = {
    {"umicom.shared.exchange", "umicom.shared.exchange", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     4U, 1, 1},
    {"umicom.shared.market-data", "umicom.shared.market-data",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.trading.orders", "umicom.trading.orders", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe EXCHANGE_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.exchange.learning",
    "org.umicom.exchange",
    "Umicom Commodity Exchange Learning",
    "A calm starting layout for learning Umicom Commodity Exchange with only the essential "
    "surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    EXCHANGE_LEARNING_SLOTS,
    sizeof(EXCHANGE_LEARNING_SLOTS) / sizeof(EXCHANGE_LEARNING_SLOTS[0]),
    "org.umicom.experience.exchange.learning"};
