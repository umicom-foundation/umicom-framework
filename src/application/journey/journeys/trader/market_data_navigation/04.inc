/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/market_data_navigation/04.inc
 *
 * PURPOSE:
 *   Declare the reusable 04 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 4 for trader.market-data-navigation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-context", "Verify linked instrument context", "context-inspector",
     "trader.context.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
