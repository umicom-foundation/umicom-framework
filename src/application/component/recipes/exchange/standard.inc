/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/exchange/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Commodity Exchange using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot EXCHANGE_STANDARD_SLOTS[] = {
    {"umicom.shared.exchange", "umicom.shared.exchange", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     5U, 1, 1},
    {"umicom.shared.market-data", "umicom.shared.market-data",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shared.commodity", "umicom.shared.commodity", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     3U, 1, 0},
    {"umicom.trading.orders", "umicom.trading.orders", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
    {"umicom.shared.surveillance", "umicom.shared.surveillance",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.settlement", "umicom.shared.settlement",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe EXCHANGE_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.exchange.standard",
    "org.umicom.exchange",
    "Umicom Commodity Exchange Standard",
    "The balanced everyday Umicom Commodity Exchange workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    EXCHANGE_STANDARD_SLOTS,
    sizeof(EXCHANGE_STANDARD_SLOTS) / sizeof(EXCHANGE_STANDARD_SLOTS[0]),
    "org.umicom.experience.exchange.standard"};
