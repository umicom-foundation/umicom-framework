/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/trading/trading_research.inc
 *
 * PURPOSE:
 *   Register provider-neutral trading research surfaces for analysis,
 *   simulation and evidence shared across finance applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Research outputs are read-only evidence until a separate permissioned
 * workflow explicitly approves an order or external action. */
COMP("umicom.trading.research", "trading", "Trading Research",
     "umicom.trading.research",
     "Scanners, backtests, replay, events, fundamentals and performance.",
     UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_FOUNDATION, 1, 1,
                 "market order portfolio chart"),
