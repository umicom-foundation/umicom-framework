/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/trading/trading_market_depth.inc
 *
 * PURPOSE:
 *   Define the umicom.trading.market-depth reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.trading.market-depth", "trading", "Market Depth", "umicom.trading",
         "Price ladder, order book imbalance and liquidity inspection.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "market order portfolio chart"),
