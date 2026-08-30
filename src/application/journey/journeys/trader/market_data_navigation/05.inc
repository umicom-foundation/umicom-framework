/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/market_data_navigation/05.inc
 *
 * PURPOSE:
 *   Declare the reusable 05 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 5 for trader.market-data-navigation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-timestamps", "Verify market-data latency timestamps", "output",
     "trader.latency.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
