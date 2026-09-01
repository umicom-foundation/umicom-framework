/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/trading/trading_options.inc
 *
 * PURPOSE:
 *   Register the reusable options-analysis surface independently from guarded
 *   order submission and live brokerage adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Analysis may prepare a reviewed strategy, but only the permissioned order
 * workflow can transform that strategy into an executable request. */
COMP("umicom.trading.options", "trading", "Options Analysis",
     "umicom.trading.options",
     "Chains, sensitivities, volatility, scenarios and strategy review.",
     UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_PLANNED, 1, 1),
